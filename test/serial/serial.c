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

#include "../../include/system/serial.h"
#include "../../src/system/serial_type.h"
#include "../include/common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
	dmg_t configuration;
	dmg_serial_t serial;
	bool interrupt;
} dmg_serial_test_t;

dmg_serial_test_t g_serial = {};

void
dmg_runtime_interrupt(
	__in int type
	)
{
	g_serial.interrupt = (type == INTERRUPT_SERIAL);
}

void
dmg_test_serial_initialize(void)
{
	memset(&g_serial, 0, sizeof(g_serial));
}

int
dmg_test_serial_load(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_serial_initialize();

	if(ASSERT_SUCCESS(dmg_serial_load(&g_serial.serial, &g_serial.configuration)) != EXIT_SUCCESS) {
		result = EXIT_FAILURE;
	}

	if(ASSERT(g_serial.serial.control.raw == POST_CONTROL)
			|| ASSERT(g_serial.serial.cycle == 0)
			|| ASSERT(g_serial.serial.data.raw == POST_DATA)
			|| ASSERT(g_serial.serial.remaining == 0)
			|| ASSERT(g_serial.serial.transfer == NULL)) {
		result = EXIT_FAILURE;
	}

	dmg_test_serial_initialize();
	g_serial.configuration.bootrom.data = (uint8_t *)1;
	g_serial.configuration.output.transfer = (dmg_serial_cb)2;

	if(ASSERT_SUCCESS(dmg_serial_load(&g_serial.serial, &g_serial.configuration)) != EXIT_SUCCESS) {
		result = EXIT_FAILURE;
	}

	if(ASSERT(g_serial.serial.control.raw == 0)
			|| ASSERT(g_serial.serial.cycle == 0)
			|| ASSERT(g_serial.serial.data.raw == 0)
			|| ASSERT(g_serial.serial.remaining == 0)
			|| ASSERT(g_serial.serial.transfer == g_serial.configuration.output.transfer)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_serial_read(void)
{
	uint8_t value = rand();
	int result = EXIT_SUCCESS;

	dmg_test_serial_initialize();
	g_serial.serial.control.raw = value;

	if(ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_CONTROL) == (value & CONTROL_MASK))) {
		result = EXIT_FAILURE;
	}

	dmg_test_serial_initialize();
	g_serial.serial.data.raw = value;

	if(ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_DATA) == value)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_serial_step(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_serial_initialize();
	dmg_serial_load(&g_serial.serial, &g_serial.configuration);

	// TODO

	TRACE_TEST(result);

	return result;
}

int
dmg_test_serial_unload(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_serial_initialize();
	dmg_serial_load(&g_serial.serial, &g_serial.configuration);
	dmg_serial_unload(&g_serial.serial);

	if(ASSERT(g_serial.serial.control.raw == 0)
			|| ASSERT(g_serial.serial.cycle == 0)
			|| ASSERT(g_serial.serial.data.raw == 0)
			|| ASSERT(g_serial.serial.remaining == 0)
			|| ASSERT(g_serial.serial.transfer == NULL)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_serial_write(void)
{
	uint8_t value = rand();
	int result = EXIT_SUCCESS;

	dmg_test_serial_initialize();
	dmg_serial_write(&g_serial.serial, ADDRESS_SERIAL_CONTROL, value);

	if(ASSERT(g_serial.serial.control.raw == (value & CONTROL_MASK))) {
		result = EXIT_FAILURE;
	}

	dmg_test_serial_initialize();
	dmg_serial_write(&g_serial.serial, ADDRESS_SERIAL_DATA, value);

	if(ASSERT(g_serial.serial.data.raw == value)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

static dmg_test_cb TEST[] = {
	dmg_test_serial_load,
	dmg_test_serial_read,
	dmg_test_serial_step,
	dmg_test_serial_unload,
	dmg_test_serial_write,
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
