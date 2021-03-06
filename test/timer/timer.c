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

#include "../../include/system/timer.h"
#include "../../src/system/timer_type.h"
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
	dmg_timer_t timer;
	bool interrupt;
} dmg_timer_test_t;

static dmg_timer_test_t g_timer = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
dmg_runtime_interrupt(
	__in int type
	)
{
	g_timer.interrupt = (type == INTERRUPT_TIMER);
}

int
dmg_service_export_data(
	__in FILE *file,
	__in const void *data,
	__in uint32_t length
	)
{
	g_timer.export_file = file;
	g_timer.export_data = data;
	g_timer.export_length = length;

	return g_timer.export;
}

int
dmg_service_import_data(
	__in FILE *file,
	__in void *data,
	__in uint32_t length
	)
{
	g_timer.import_file = file;
	g_timer.import_data = data;
	g_timer.import_length = length;

	return g_timer.import;
}

static void
dmg_test_timer_initialize(void)
{
	memset(&g_timer, 0, sizeof(g_timer));
}

int
dmg_test_timer_load(void)
{
	int result = DMG_STATUS_SUCCESS;

	dmg_test_timer_initialize();

	if(ASSERT_SUCCESS(dmg_timer_load(&g_timer.timer, &g_timer.configuration)) != DMG_STATUS_SUCCESS) {
		result = DMG_STATUS_FAILURE;
	}

	if(ASSERT(g_timer.timer.control.raw == POST_CONTROL)
			|| ASSERT(g_timer.timer.counter == POST_COUNTER)
			|| ASSERT(g_timer.timer.cycle == 0)
			|| ASSERT(g_timer.timer.divider.raw == POST_DIVIDER)
			|| ASSERT(g_timer.timer.modulo == POST_MODULO)) {
		result = DMG_STATUS_FAILURE;
	}

	dmg_test_timer_initialize();
	g_timer.configuration.bootrom.data = (void *)1;

	if(ASSERT_SUCCESS(dmg_timer_load(&g_timer.timer, &g_timer.configuration)) != DMG_STATUS_SUCCESS) {
		result = DMG_STATUS_FAILURE;
	}

	if(ASSERT(g_timer.timer.control.raw == 0)
			|| ASSERT(g_timer.timer.counter == 0)
			|| ASSERT(g_timer.timer.cycle == 0)
			|| ASSERT(g_timer.timer.divider.raw == 0)
			|| ASSERT(g_timer.timer.modulo == 0)) {
		result = DMG_STATUS_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_timer_read(void)
{
	uint16_t value = rand();
	int result = DMG_STATUS_SUCCESS;

	dmg_test_timer_initialize();

	if(ASSERT(dmg_timer_read(&g_timer.timer, ADDRESS_TIMER_DIVIDER - 1) == UINT8_MAX)) {
		result = DMG_STATUS_FAILURE;
	}

	dmg_test_timer_initialize();
	g_timer.timer.control.raw = value;

	if(ASSERT(dmg_timer_read(&g_timer.timer, ADDRESS_TIMER_CONTROL) == (value & CONTROL_MASK))) {
		result = DMG_STATUS_FAILURE;
	}

	dmg_test_timer_initialize();
	g_timer.timer.counter = value;

	if(ASSERT(dmg_timer_read(&g_timer.timer, ADDRESS_TIMER_COUNTER) == (value & UINT8_MAX))) {
		result = DMG_STATUS_FAILURE;
	}

	dmg_test_timer_initialize();
	g_timer.timer.divider.raw = value;

	if(ASSERT(dmg_timer_read(&g_timer.timer, ADDRESS_TIMER_DIVIDER) == (value >> CHAR_BIT))) {
		result = DMG_STATUS_FAILURE;
	}

	dmg_test_timer_initialize();
	g_timer.timer.modulo = value;

	if(ASSERT(dmg_timer_read(&g_timer.timer, ADDRESS_TIMER_MODULO) == (value & UINT8_MAX))) {
		result = DMG_STATUS_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_timer_step(void)
{
	int result = DMG_STATUS_SUCCESS;

	dmg_test_timer_initialize();
	dmg_timer_load(&g_timer.timer, &g_timer.configuration);
	g_timer.timer.divider.raw = 0;

	for(uint16_t divider = 1; divider <= (UINT8_MAX + 1); ++divider) {

		for(uint32_t cycle = 0; cycle < (SELECT_CYC[SELECT_16386] - CYCLE); cycle += CYCLE) {
			dmg_timer_step(&g_timer.timer, CYCLE);

			if(ASSERT(dmg_timer_read(&g_timer.timer, ADDRESS_TIMER_DIVIDER) == ((divider - 1) & UINT8_MAX))) {
				result = DMG_STATUS_FAILURE;
				goto exit;
			}
		}

		dmg_timer_step(&g_timer.timer, CYCLE);

		if(ASSERT(dmg_timer_read(&g_timer.timer, ADDRESS_TIMER_DIVIDER) == (divider & UINT8_MAX))) {
			result = DMG_STATUS_FAILURE;
			goto exit;
		}
	}

	for(int select = SELECT_4096; select < SELECT_MAX; ++select) {
		dmg_test_timer_initialize();
		dmg_timer_load(&g_timer.timer, &g_timer.configuration);
		g_timer.timer.modulo = rand();
		g_timer.timer.counter = g_timer.timer.modulo;
		g_timer.timer.control.enable = true;
		g_timer.timer.control.select = select;

		for(uint16_t counter = g_timer.timer.modulo; counter <= UINT8_MAX; ++counter) {

			for(uint32_t cycle = 0; cycle < (SELECT_CYC[select] - CYCLE); cycle += CYCLE) {
				dmg_timer_step(&g_timer.timer, CYCLE);

				if(ASSERT(dmg_timer_read(&g_timer.timer, ADDRESS_TIMER_COUNTER) == (counter & UINT8_MAX))) {
					result = DMG_STATUS_FAILURE;
					goto exit;
				}
			}

			dmg_timer_step(&g_timer.timer, CYCLE);

			if(ASSERT(dmg_timer_read(&g_timer.timer, ADDRESS_TIMER_COUNTER) == ((counter + 1) & UINT8_MAX))) {
				result = DMG_STATUS_FAILURE;
				goto exit;
			}
		}

		dmg_timer_step(&g_timer.timer, CYCLE);

		if(ASSERT(dmg_timer_read(&g_timer.timer, ADDRESS_TIMER_COUNTER) == g_timer.timer.modulo)
				|| ASSERT(g_timer.interrupt == true)) {
			result = DMG_STATUS_FAILURE;
			goto exit;
		}

		g_timer.interrupt = false;
	}

exit:
	TRACE_TEST(result);

	return result;
}

int
dmg_test_timer_unload(void)
{
	int result = DMG_STATUS_SUCCESS;

	dmg_test_timer_initialize();
	dmg_timer_load(&g_timer.timer, &g_timer.configuration);
	dmg_timer_unload(&g_timer.timer);

	if(ASSERT(g_timer.timer.control.raw == 0)
			|| ASSERT(g_timer.timer.counter == 0)
			|| ASSERT(g_timer.timer.cycle == 0)
			|| ASSERT(g_timer.timer.divider.raw == 0)
			|| ASSERT(g_timer.timer.modulo == 0)) {
		result = DMG_STATUS_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_timer_write(void)
{
	uint16_t value = rand();
	int result = DMG_STATUS_SUCCESS;

	dmg_test_timer_initialize();
	dmg_timer_write(&g_timer.timer, ADDRESS_TIMER_CONTROL, value);

	if(ASSERT(g_timer.timer.control.raw == (value & CONTROL_MASK))) {
		result = DMG_STATUS_FAILURE;
	}

	dmg_test_timer_initialize();
	dmg_timer_write(&g_timer.timer, ADDRESS_TIMER_COUNTER, value);

	if(ASSERT(g_timer.timer.counter == (value & UINT8_MAX))) {
		result = DMG_STATUS_FAILURE;
	}

	dmg_test_timer_initialize();
	dmg_timer_write(&g_timer.timer, ADDRESS_TIMER_DIVIDER, value);

	if(ASSERT(g_timer.timer.divider.raw == 0)) {
		result = DMG_STATUS_FAILURE;
	}

	dmg_test_timer_initialize();
	dmg_timer_write(&g_timer.timer, ADDRESS_TIMER_MODULO, value);

	if(ASSERT(g_timer.timer.modulo == (value & UINT8_MAX))) {
		result = DMG_STATUS_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

static const dmg_test TEST[] = {
	dmg_test_timer_load,
	dmg_test_timer_read,
	dmg_test_timer_step,
	dmg_test_timer_unload,
	dmg_test_timer_write,
	};

int
main(
	__in int argc,
	__in char *argv[]
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(argc > 1) {
		TEST_SEED(strtol(argv[1], NULL, 16));
	} else {
		TEST_SEED(time(NULL));
	}

	for(size_t trial = 0; trial < TEST_TRIALS; ++trial) {
		TRACE_TEST_TRIAL(trial);

		for(size_t test = 0; test < TEST_COUNT(TEST); ++test) {

			if(TEST[test]() != DMG_STATUS_SUCCESS) {
				result = DMG_STATUS_FAILURE;
			}
		}
	}

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
