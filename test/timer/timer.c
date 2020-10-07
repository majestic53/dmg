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

#include "../../include/system/timer.h"
#include "../../src/system/timer_type.h"
#include "../include/common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
	dmg_t configuration;
	dmg_timer_t timer;
	bool interrupt;
} dmg_timer_test_t;

dmg_timer_test_t g_timer = {};

void
dmg_runtime_interrupt(
	__in int type
	)
{
	g_timer.interrupt = (type == INTERRUPT_TIMER);
}

void
dmg_test_timer_initialize(void)
{
	memset(&g_timer, 0, sizeof(g_timer));
}

int
dmg_test_timer_load(void)
{
	int result = EXIT_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Timer load test started");

	dmg_test_timer_initialize();

	if(ASSERT_SUCCESS(dmg_timer_load(&g_timer.timer, &g_timer.configuration)) != EXIT_SUCCESS) {
		TRACE(LEVEL_ERROR, "Timer load test #1 FAIL");
		result = EXIT_FAILURE;
	}

	if(ASSERT(g_timer.timer.control.raw == POST_CONTROL)
			|| ASSERT(g_timer.timer.counter == POST_COUNTER)
			|| ASSERT(g_timer.timer.cycle == 0)
			|| ASSERT(g_timer.timer.divider == POST_DIVIDER)
			|| ASSERT(g_timer.timer.modulo == POST_MODULO)) {
		TRACE(LEVEL_ERROR, "Timer load test #2 FAIL");
		result = EXIT_FAILURE;
	}

	dmg_test_timer_initialize();
	g_timer.configuration.bootrom.data = (uint8_t *)1;

	if(ASSERT_SUCCESS(dmg_timer_load(&g_timer.timer, &g_timer.configuration)) != EXIT_SUCCESS) {
		TRACE(LEVEL_ERROR, "Timer load test #3 FAIL");
		result = EXIT_FAILURE;
	}

	if(ASSERT(g_timer.timer.control.raw == 0)
			|| ASSERT(g_timer.timer.counter == 0)
			|| ASSERT(g_timer.timer.cycle == 0)
			|| ASSERT(g_timer.timer.divider == 0)
			|| ASSERT(g_timer.timer.modulo == 0)) {
		TRACE(LEVEL_ERROR, "Timer load test #4 FAIL");
		result = EXIT_FAILURE;
	}

	TRACE_FORMAT(LEVEL_INFORMATION, "Timer load test %s", (result == EXIT_SUCCESS) ? "PASS" : "FAIL");

	return result;
}

int
dmg_test_timer_read(void)
{
	uint16_t value = rand();
	int result = EXIT_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Timer read test started");

	dmg_test_timer_initialize();
	g_timer.timer.control.raw = value;

	if(ASSERT(dmg_timer_read(&g_timer.timer, ADDRESS_TIMER_CONTROL) == (value & CONTROL_MASK))) {
		TRACE(LEVEL_ERROR, "Timer read test #1 FAIL");
		result = EXIT_FAILURE;
	}

	dmg_test_timer_initialize();
	g_timer.timer.counter = value;

	if(ASSERT(dmg_timer_read(&g_timer.timer, ADDRESS_TIMER_COUNTER) == (value & UINT8_MAX))) {
		TRACE(LEVEL_ERROR, "Timer read test #2 FAIL");
		result = EXIT_FAILURE;
	}

	dmg_test_timer_initialize();
	g_timer.timer.divider = value;

	if(ASSERT(dmg_timer_read(&g_timer.timer, ADDRESS_TIMER_DIVIDER) == (value >> CHAR_BIT))) {
		TRACE(LEVEL_ERROR, "Timer read test #3 FAIL");
		result = EXIT_FAILURE;
	}

	dmg_test_timer_initialize();
	g_timer.timer.modulo = value;

	if(ASSERT(dmg_timer_read(&g_timer.timer, ADDRESS_TIMER_MODULO) == (value & UINT8_MAX))) {
		TRACE(LEVEL_ERROR, "Timer read test #4 FAIL");
		result = EXIT_FAILURE;
	}

	TRACE_FORMAT(LEVEL_INFORMATION, "Timer read test %s", (result == EXIT_SUCCESS) ? "PASS" : "FAIL");

	return result;
}

int
dmg_test_timer_step(void)
{
	int result = EXIT_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Timer step test started");

	// TODO

	TRACE_FORMAT(LEVEL_INFORMATION, "Timer step test %s", (result == EXIT_SUCCESS) ? "PASS" : "FAIL");

	return result;
}

int
dmg_test_timer_unload(void)
{
	int result = EXIT_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Timer unload test started");

	dmg_test_timer_initialize();
	dmg_timer_load(&g_timer.timer, &g_timer.configuration);
	dmg_timer_unload(&g_timer.timer);

	if(ASSERT(g_timer.timer.control.raw == 0)
			|| ASSERT(g_timer.timer.counter == 0)
			|| ASSERT(g_timer.timer.cycle == 0)
			|| ASSERT(g_timer.timer.divider == 0)
			|| ASSERT(g_timer.timer.modulo == 0)) {
		TRACE(LEVEL_ERROR, "Timer unload test #1 FAIL");
		result = EXIT_FAILURE;
	}

	TRACE_FORMAT(LEVEL_INFORMATION, "Timer unload test %s", (result == EXIT_SUCCESS) ? "PASS" : "FAIL");

	return result;
}

int
dmg_test_timer_write(void)
{
	uint16_t value = rand();
	int result = EXIT_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Timer write test started");

	dmg_test_timer_initialize();
	dmg_timer_write(&g_timer.timer, ADDRESS_TIMER_CONTROL, value);

	if(ASSERT(g_timer.timer.control.raw == (value & CONTROL_MASK))) {
		TRACE(LEVEL_ERROR, "Timer write test #1 FAIL");
		result = EXIT_FAILURE;
	}

	dmg_test_timer_initialize();
	dmg_timer_write(&g_timer.timer, ADDRESS_TIMER_COUNTER, value);

	if(ASSERT(g_timer.timer.counter == (value & UINT8_MAX))) {
		TRACE(LEVEL_ERROR, "Timer write test #2 FAIL");
		result = EXIT_FAILURE;
	}

	dmg_test_timer_initialize();
	dmg_timer_write(&g_timer.timer, ADDRESS_TIMER_DIVIDER, value);

	if(ASSERT(g_timer.timer.divider == 0)) {
		TRACE(LEVEL_ERROR, "Timer write test #3 FAIL");
		result = EXIT_FAILURE;
	}

	dmg_test_timer_initialize();
	dmg_timer_write(&g_timer.timer, ADDRESS_TIMER_MODULO, value);

	if(ASSERT(g_timer.timer.modulo == (value & UINT8_MAX))) {
		TRACE(LEVEL_ERROR, "Timer write test #4 FAIL");
		result = EXIT_FAILURE;
	}

	TRACE_FORMAT(LEVEL_INFORMATION, "Timer write test %s", (result == EXIT_SUCCESS) ? "PASS" : "FAIL");

	return result;
}

static dmg_test_cb TEST[] = {
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
	int result = EXIT_SUCCESS;

	TRACE_ENABLE(&g_timer.timer.cycle);
	TRACE_FORMAT(LEVEL_INFORMATION, "Timer test started (%u cases)", ARRAY_LENGTH(dmg_test_cb, TEST));

	for(size_t test = 0; test < ARRAY_LENGTH(dmg_test_cb, TEST); ++test) {

		if(TEST[test]() != EXIT_SUCCESS) {
			result = EXIT_FAILURE;
		}
	}

	TRACE_FORMAT(LEVEL_INFORMATION, "Timer test %s", (result == EXIT_SUCCESS) ? "PASS" : "FAIL");

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
