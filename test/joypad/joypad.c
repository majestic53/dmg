/**
 * DMG
 * Copyright (C) 2020-2021 David Jolly
 *
 * DMG is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DMG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../../include/system/joypad.h"
#include "../../src/system/joypad_type.h"
#include "../include/common.h"

typedef struct {
	dmg_t configuration;
	int export;
	FILE *export_file;
	const void *export_data;
	uint32_t export_length;
	int import;
	FILE *import_file;
	void *import_data;
	uint32_t import_length;
	dmg_joypad_t joypad;
	bool button[DMG_BUTTON_MAX];
	bool direction[DMG_DIRECTION_MAX];
	bool interrupt;
} dmg_joypad_test_t;

static dmg_joypad_test_t g_joypad = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
dmg_runtime_interrupt(
	__in int type
	)
{
	g_joypad.interrupt = (type == INTERRUPT_JOYPAD);
}

bool
dmg_service_button(
	__in int button
	)
{
	return g_joypad.button[button];
}

bool
dmg_service_direction(
	__in int direction
	)
{
	return g_joypad.direction[direction];
}

int
dmg_service_export_data(
	__in FILE *file,
	__in const void *data,
	__in uint32_t length
	)
{
	g_joypad.export_file = file;
	g_joypad.export_data = data;
	g_joypad.export_length = length;

	return g_joypad.export;
}

int
dmg_service_import_data(
	__in FILE *file,
	__in void *data,
	__in uint32_t length
	)
{
	g_joypad.import_file = file;
	g_joypad.import_data = data;
	g_joypad.import_length = length;

	return g_joypad.import;
}

static void
dmg_test_joypad_initialize(void)
{
	memset(&g_joypad, 0, sizeof(g_joypad));
}

int
dmg_test_joypad_load(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_joypad_initialize();

	if(ASSERT_SUCCESS(dmg_joypad_load(&g_joypad.joypad, &g_joypad.configuration)) != EXIT_SUCCESS) {
		result = EXIT_FAILURE;
	}

	if(ASSERT(g_joypad.joypad.state.raw == POST_STATE)
			|| ASSERT(g_joypad.joypad.cycle == 0)) {
		result = EXIT_FAILURE;
	}

	for(int button = 0; button < DMG_BUTTON_MAX; ++button) {

		if(ASSERT(g_joypad.joypad.button[button] == false)) {
			result = EXIT_FAILURE;
		}
	}

	for(int direction = 0; direction < DMG_DIRECTION_MAX; ++direction) {

		if(ASSERT(g_joypad.joypad.direction[direction] == false)) {
			result = EXIT_FAILURE;
		}
	}

	dmg_test_joypad_initialize();
	g_joypad.configuration.bootrom.data = (void *)1;

	if(ASSERT_SUCCESS(dmg_joypad_load(&g_joypad.joypad, &g_joypad.configuration)) != EXIT_SUCCESS) {
		result = EXIT_FAILURE;
	}

	if(ASSERT(g_joypad.joypad.state.raw == POST_STATE)
			|| ASSERT(g_joypad.joypad.cycle == 0)) {
		result = EXIT_FAILURE;
	}

	for(int button = 0; button < DMG_BUTTON_MAX; ++button) {

		if(ASSERT(g_joypad.joypad.button[button] == false)) {
			result = EXIT_FAILURE;
		}
	}

	for(int direction = 0; direction < DMG_DIRECTION_MAX; ++direction) {

		if(ASSERT(g_joypad.joypad.direction[direction] == false)) {
			result = EXIT_FAILURE;
		}
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_joypad_read(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_joypad_initialize();

	if(ASSERT(dmg_joypad_read(&g_joypad.joypad, ADDRESS_JOYPAD_STATE - 1) == UINT8_MAX)) {
		result = EXIT_FAILURE;
	}

	dmg_test_joypad_initialize();
	g_joypad.joypad.state.raw = rand();

	if(ASSERT(dmg_joypad_read(&g_joypad.joypad, ADDRESS_JOYPAD_STATE) == (g_joypad.joypad.state.raw | STATE_READ))) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_joypad_step(void)
{
	int result = EXIT_SUCCESS;
	dmg_joypad_state_t expected = {};

	dmg_test_joypad_initialize();
	dmg_joypad_load(&g_joypad.joypad, &g_joypad.configuration);

	for(uint32_t cycle = 0; cycle < (STATE_CYCLE - CYCLE); cycle += CYCLE) {
		dmg_joypad_step(&g_joypad.joypad, CYCLE);

		if(ASSERT(dmg_joypad_read(&g_joypad.joypad, ADDRESS_JOYPAD_STATE) == POST_STATE)
				|| ASSERT(g_joypad.interrupt == false)) {
			result = EXIT_FAILURE;
			goto exit;
		}
	}

	expected.raw = POST_STATE;
	expected.button = false;
	g_joypad.joypad.state.raw = STATE_MASK;
	g_joypad.joypad.state.button = false;

	for(int button = 0; button < DMG_BUTTON_MAX; ++button) {
		g_joypad.button[button] = true;
		expected.raw &= ~(1 << button);
	}

	dmg_joypad_step(&g_joypad.joypad, CYCLE);

	if(ASSERT(dmg_joypad_read(&g_joypad.joypad, ADDRESS_JOYPAD_STATE) == expected.raw)
			|| ASSERT(g_joypad.interrupt == true)) {
		result = EXIT_FAILURE;
		goto exit;
	}

	dmg_test_joypad_initialize();
	dmg_joypad_load(&g_joypad.joypad, &g_joypad.configuration);

	for(uint32_t cycle = 0; cycle < (STATE_CYCLE - CYCLE); cycle += CYCLE) {
		dmg_joypad_step(&g_joypad.joypad, CYCLE);

		if(ASSERT(dmg_joypad_read(&g_joypad.joypad, ADDRESS_JOYPAD_STATE) == POST_STATE)
				|| ASSERT(g_joypad.interrupt == false)) {
			result = EXIT_FAILURE;
			goto exit;
		}
	}

	expected.raw = POST_STATE;
	expected.direction = false;
	g_joypad.joypad.state.raw = STATE_MASK;
	g_joypad.joypad.state.direction = false;

	for(int direction = 0; direction < DMG_DIRECTION_MAX; ++direction) {
		g_joypad.direction[direction] = true;
		expected.raw &= ~(1 << direction);
	}

	dmg_joypad_step(&g_joypad.joypad, CYCLE);

	if(ASSERT(dmg_joypad_read(&g_joypad.joypad, ADDRESS_JOYPAD_STATE) == expected.raw)
			|| ASSERT(g_joypad.interrupt == true)) {
		result = EXIT_FAILURE;
		goto exit;
	}

	expected.raw = POST_STATE;
	expected.button = false;
	expected.direction = false;
	g_joypad.joypad.state.raw = STATE_MASK;
	g_joypad.joypad.state.button = false;
	g_joypad.joypad.state.direction = false;

	for(int button = 0; button < DMG_BUTTON_MAX; ++button) {
		g_joypad.button[button] = true;
		expected.raw &= ~(1 << button);
	}

	for(int direction = 0; direction < DMG_DIRECTION_MAX; ++direction) {
		g_joypad.direction[direction] = true;
		expected.raw &= ~(1 << direction);
	}

	dmg_joypad_step(&g_joypad.joypad, CYCLE);

	if(ASSERT(dmg_joypad_read(&g_joypad.joypad, ADDRESS_JOYPAD_STATE) == expected.raw)
			|| ASSERT(g_joypad.interrupt == true)) {
		result = EXIT_FAILURE;
		goto exit;
	}

exit:
	TRACE_TEST(result);

	return result;
}

int
dmg_test_joypad_unload(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_joypad_initialize();
	dmg_joypad_load(&g_joypad.joypad, &g_joypad.configuration);
	dmg_joypad_unload(&g_joypad.joypad);

	if(ASSERT(g_joypad.joypad.state.raw == 0)
			|| ASSERT(g_joypad.joypad.cycle == 0)) {
		result = EXIT_FAILURE;
	}

	for(int button = 0; button < DMG_BUTTON_MAX; ++button) {

		if(ASSERT(g_joypad.joypad.button[button] == false)) {
			result = EXIT_FAILURE;
		}
	}

	for(int direction = 0; direction < DMG_DIRECTION_MAX; ++direction) {

		if(ASSERT(g_joypad.joypad.direction[direction] == false)) {
			result = EXIT_FAILURE;
		}
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_joypad_write(void)
{
	int result = EXIT_SUCCESS;
	dmg_joypad_state_t expected = {}, value = {};

	dmg_test_joypad_initialize();
	expected.raw = POST_STATE;
	value.raw = STATE_MASK;
	dmg_joypad_write(&g_joypad.joypad, ADDRESS_JOYPAD_STATE, value.raw);

	if(ASSERT(dmg_joypad_read(&g_joypad.joypad, ADDRESS_JOYPAD_STATE) == expected.raw)) {
		result = EXIT_FAILURE;
	}

	dmg_test_joypad_initialize();
	expected.raw = POST_STATE;
	expected.button = false;
	value.raw = STATE_MASK;
	value.button = false;

	for(int button = 0; button < DMG_BUTTON_MAX; ++button) {

		if((g_joypad.button[button] = (rand() % 2))) {
			expected.raw &= ~(1 << button);
		}
	}

	dmg_joypad_write(&g_joypad.joypad, ADDRESS_JOYPAD_STATE, value.raw);
	value.raw = dmg_joypad_read(&g_joypad.joypad, ADDRESS_JOYPAD_STATE);

	if(ASSERT(value.raw == expected.raw)
			|| ASSERT(value.button == expected.button)) {
		result = EXIT_FAILURE;
	}

	dmg_test_joypad_initialize();
	expected.raw = POST_STATE;
	expected.direction = false;
	value.raw = STATE_MASK;
	value.direction = false;

	for(int direction = 0; direction < DMG_DIRECTION_MAX; ++direction) {

		if((g_joypad.direction[direction] = (rand() % 2))) {
			expected.raw &= ~(1 << direction);
		}
	}

	dmg_joypad_write(&g_joypad.joypad, ADDRESS_JOYPAD_STATE, value.raw);
	value.raw = dmg_joypad_read(&g_joypad.joypad, ADDRESS_JOYPAD_STATE);

	if(ASSERT(value.raw == expected.raw)
			|| ASSERT(value.direction == expected.direction)) {
		result = EXIT_FAILURE;
	}

	dmg_test_joypad_initialize();
	expected.raw = POST_STATE;
	expected.button = false;
	expected.direction = false;
	value.raw = STATE_MASK;
	value.button = false;
	value.direction = false;

	for(int button = 0; button < DMG_BUTTON_MAX; ++button) {

		if((g_joypad.button[button] = (rand() % 2))) {
			expected.raw &= ~(1 << button);
		}
	}

	for(int direction = 0; direction < DMG_DIRECTION_MAX; ++direction) {

		if((g_joypad.direction[direction] = (rand() % 2))) {
			expected.raw &= ~(1 << direction);
		}
	}

	dmg_joypad_write(&g_joypad.joypad, ADDRESS_JOYPAD_STATE, value.raw);
	value.raw = dmg_joypad_read(&g_joypad.joypad, ADDRESS_JOYPAD_STATE);

	if(ASSERT(value.raw == expected.raw)
			|| ASSERT(value.button == expected.button)
			|| ASSERT(value.direction == expected.direction)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

static const dmg_test TEST[] = {
	dmg_test_joypad_load,
	dmg_test_joypad_read,
	dmg_test_joypad_step,
	dmg_test_joypad_unload,
	dmg_test_joypad_write,
	};

int
main(
	__in int argc,
	__in char *argv[]
	)
{
	int result = EXIT_SUCCESS;

	if(argc > 1) {
		TEST_SEED(strtol(argv[1], NULL, 16));
	} else {
		TEST_SEED(time(NULL));
	}

	for(size_t trial = 0; trial < TEST_TRIALS; ++trial) {
		TRACE_TEST_TRIAL(trial);

		for(size_t test = 0; test < TEST_COUNT(TEST); ++test) {

			if(TEST[test]() != EXIT_SUCCESS) {
				result = EXIT_FAILURE;
			}
		}
	}

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
