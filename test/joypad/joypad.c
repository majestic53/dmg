/**
 * DMG
 * Copyright (C) 2020 David Jolly
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../../include/system/joypad.h"
#include "../../src/system/joypad_type.h"
#include "../include/common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
	dmg_t configuration;
	dmg_joypad_t joypad;
	bool button[DMG_BUTTON_MAX];
	bool direction[DMG_DIRECTION_MAX];
	bool interrupt;
} dmg_joypad_test_t;

dmg_joypad_test_t g_joypad = {};

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

void
dmg_test_joypad_initialize(void)
{
	memset(&g_joypad, 0, sizeof(g_joypad));
}

int
dmg_test_joypad_load(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_joypad_initialize();

	// TODO

	TRACE_TEST(result);

	return result;
}

int
dmg_test_joypad_read(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_joypad_initialize();

	// TODO

	TRACE_TEST(result);

	return result;
}

int
dmg_test_joypad_step(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_joypad_initialize();

	// TODO

	TRACE_TEST(result);

	return result;
}

int
dmg_test_joypad_unload(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_joypad_initialize();

	// TODO

	TRACE_TEST(result);

	return result;
}

int
dmg_test_joypad_write(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_joypad_initialize();

	// TODO

	TRACE_TEST(result);

	return result;
}

static dmg_test_cb TEST[] = {
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

	for(size_t test = 0; test < TEST_COUNT(TEST); ++test) {

		if(TEST[test]() != EXIT_SUCCESS) {
			result = EXIT_FAILURE;
		}
	}

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
