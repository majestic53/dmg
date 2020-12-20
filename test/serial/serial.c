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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../../include/system/serial.h"
#include "../../src/system/serial_type.h"
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
	dmg_serial_t serial;
	bool interrupt;
} dmg_serial_test_t;

static dmg_serial_test_t g_serial = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_service_export_data(
	__in FILE *file,
	__in const void *data,
	__in uint32_t length
	)
{
	g_serial.export_file = file;
	g_serial.export_data = data;
	g_serial.export_length = length;

	return g_serial.export;
}

int
dmg_service_import_data(
	__in FILE *file,
	__in void *data,
	__in uint32_t length
	)
{
	g_serial.import_file = file;
	g_serial.import_data = data;
	g_serial.import_length = length;

	return g_serial.import;
}

unsigned
dmg_transfer_out(
	__in unsigned in
	)
{
	return in;
}

void
dmg_runtime_interrupt(
	__in int type
	)
{
	g_serial.interrupt = (type == INTERRUPT_SERIAL);
}

static void
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
			|| ASSERT(g_serial.serial.out == NULL)) {
		result = EXIT_FAILURE;
	}

	dmg_test_serial_initialize();
	g_serial.configuration.bootrom.data = (void *)1;
	g_serial.configuration.serial_out = (dmg_serial_out)2;

	if(ASSERT_SUCCESS(dmg_serial_load(&g_serial.serial, &g_serial.configuration)) != EXIT_SUCCESS) {
		result = EXIT_FAILURE;
	}

	if(ASSERT(g_serial.serial.control.raw == 0)
			|| ASSERT(g_serial.serial.cycle == 0)
			|| ASSERT(g_serial.serial.data.raw == 0)
			|| ASSERT(g_serial.serial.remaining == 0)
			|| ASSERT(g_serial.serial.out == g_serial.configuration.serial_out)) {
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

	if(ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_DATA - 1) == UINT8_MAX)) {
		result = EXIT_FAILURE;
	}

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
	uint8_t value = rand();
	int result = EXIT_SUCCESS;

	for(int select = SELECT_INTERNAL; select < (SELECT_MAX - 1); ++select) {
		dmg_serial_data_t data = {};
		dmg_serial_control_t control = {};

		dmg_test_serial_initialize();
		dmg_serial_load(&g_serial.serial, &g_serial.configuration);
		dmg_serial_write(&g_serial.serial, ADDRESS_SERIAL_DATA, value);
		control.enable = false;
		control.select = select;
		dmg_serial_write(&g_serial.serial, ADDRESS_SERIAL_CONTROL, control.raw);

		for(uint8_t bit = 0; bit < CHAR_BIT; ++bit) {

			for(uint32_t cycle = 0; cycle < (SELECT_CYC[select] - CYCLE); cycle += CYCLE) {
				dmg_serial_step(&g_serial.serial, CYCLE);

				if(ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_CONTROL) == (g_serial.serial.control.raw & CONTROL_MASK))
						|| ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_DATA) == value)
						|| ASSERT(g_serial.serial.remaining == CHAR_BIT)) {
					result = EXIT_FAILURE;
					goto exit;
				}
			}

			dmg_serial_step(&g_serial.serial, CYCLE);

			if(ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_CONTROL) == (g_serial.serial.control.raw & CONTROL_MASK))
					|| ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_DATA) == value)
					|| ASSERT(g_serial.serial.remaining == CHAR_BIT)) {
				result = EXIT_FAILURE;
				goto exit;
			}
		}

		if(ASSERT(g_serial.interrupt == false)) {
			result = EXIT_FAILURE;
			goto exit;
		}

		dmg_test_serial_initialize();
		dmg_serial_load(&g_serial.serial, &g_serial.configuration);
		data.raw = value;
		dmg_serial_write(&g_serial.serial, ADDRESS_SERIAL_DATA, data.raw);
		control.enable = true;
		control.select = select;
		dmg_serial_write(&g_serial.serial, ADDRESS_SERIAL_CONTROL, control.raw);

		for(uint8_t bit = 0; bit < CHAR_BIT; ++bit) {

			for(uint32_t cycle = 0; cycle < (SELECT_CYC[select] - CYCLE); cycle += CYCLE) {
				dmg_serial_step(&g_serial.serial, CYCLE);

				if(ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_CONTROL) == (g_serial.serial.control.raw & CONTROL_MASK))
						|| ASSERT(g_serial.interrupt == false)) {
					result = EXIT_FAILURE;
					goto exit;
				}

				if(control.select == SELECT_INTERNAL) {

					if((control.select == SELECT_INTERNAL) && (ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_DATA) == data.raw)
							|| ASSERT(g_serial.serial.remaining == (CHAR_BIT - bit)))) {
						result = EXIT_FAILURE;
						goto exit;
					}
				}
			}

			data.raw = ((g_serial.serial.data.raw << 1) | 1);
			dmg_serial_step(&g_serial.serial, CYCLE);

			if(ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_CONTROL) == (g_serial.serial.control.raw & CONTROL_MASK))) {
				result = EXIT_FAILURE;
				goto exit;
			}

			if((control.select == SELECT_INTERNAL) && (ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_DATA) == data.raw)
					|| ASSERT(g_serial.serial.remaining == (CHAR_BIT - bit))
					|| ASSERT(g_serial.interrupt == (bit == (CHAR_BIT - 1))))) {
				result = EXIT_FAILURE;
				goto exit;
			}
		}

		if((control.select == SELECT_INTERNAL)
				&& (ASSERT(g_serial.serial.control.enable == false) || ASSERT(g_serial.interrupt == true))) {
			result = EXIT_FAILURE;
			goto exit;
		}

		dmg_test_serial_initialize();
		g_serial.configuration.serial_out = dmg_transfer_out;
		dmg_serial_load(&g_serial.serial, &g_serial.configuration);
		data.raw = value;
		dmg_serial_write(&g_serial.serial, ADDRESS_SERIAL_DATA, data.raw);
		control.enable = true;
		control.select = select;
		dmg_serial_write(&g_serial.serial, ADDRESS_SERIAL_CONTROL, control.raw);

		for(uint8_t bit = 0; bit < CHAR_BIT; ++bit) {

			for(uint32_t cycle = 0; cycle < (SELECT_CYC[select] - CYCLE); cycle += CYCLE) {
				dmg_serial_step(&g_serial.serial, CYCLE);

				if(ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_CONTROL) == (g_serial.serial.control.raw & CONTROL_MASK))
						|| ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_DATA) == data.raw)
						|| ASSERT(g_serial.serial.remaining == (CHAR_BIT - bit))
						|| ASSERT(g_serial.interrupt == false)) {
					result = EXIT_FAILURE;
					goto exit;
				}
			}

			data.raw = ((g_serial.serial.data.raw << 1) | data.msb);
			dmg_serial_step(&g_serial.serial, CYCLE);

			if(ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_CONTROL) == (g_serial.serial.control.raw & CONTROL_MASK))
					|| ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_DATA) == data.raw)
					|| ASSERT(g_serial.serial.remaining == (CHAR_BIT - bit))
					|| ASSERT(g_serial.interrupt == (bit == (CHAR_BIT - 1)))) {
				result = EXIT_FAILURE;
				goto exit;
			}
		}

		if(ASSERT(g_serial.serial.control.enable == false)
				|| ASSERT(g_serial.interrupt == true)) {
			result = EXIT_FAILURE;
			goto exit;
		}
	}

exit:
	TRACE_TEST(result);

	return result;
}

int
dmg_test_serial_transfer(void)
{
	uint8_t value = rand();
	int result = EXIT_SUCCESS;
	dmg_serial_control_t control = {};
	dmg_serial_data_t data_out = {}, data_in = {}, in = {}, out = {};

	dmg_test_serial_initialize();
	dmg_serial_load(&g_serial.serial, &g_serial.configuration);
	dmg_serial_write(&g_serial.serial, ADDRESS_SERIAL_DATA, value);
	control.enable = false;
	control.select = SELECT_EXTERNAL;
	dmg_serial_write(&g_serial.serial, ADDRESS_SERIAL_CONTROL, control.raw);
	in.raw = rand();
	out.raw = 0;

	out = dmg_serial_transfer(&g_serial.serial, in);

	if(ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_CONTROL) == (g_serial.serial.control.raw & CONTROL_MASK))
			|| ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_DATA) == value)
			|| ASSERT(g_serial.serial.remaining == CHAR_BIT)
			|| ASSERT(!out.raw)) {
		result = EXIT_FAILURE;
		goto exit;
	}

	dmg_test_serial_initialize();
	dmg_serial_load(&g_serial.serial, &g_serial.configuration);
	dmg_serial_write(&g_serial.serial, ADDRESS_SERIAL_DATA, value);
	control.enable = true;
	control.select = SELECT_EXTERNAL;
	dmg_serial_write(&g_serial.serial, ADDRESS_SERIAL_CONTROL, control.raw);
	in.raw = rand();
	out.raw = 0;

	for(uint8_t bit = 0; bit < CHAR_BIT; ++bit) {
		data_out.raw = dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_DATA);
		out = dmg_serial_transfer(&g_serial.serial, in);
		data_in.raw = ((data_out.raw << 1) | in.lsb);

		if(ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_CONTROL) == (g_serial.serial.control.raw & CONTROL_MASK))
				|| ASSERT(dmg_serial_read(&g_serial.serial, ADDRESS_SERIAL_DATA) == data_in.raw)
				|| ASSERT(out.lsb == data_out.msb)
				|| ASSERT(g_serial.serial.remaining == (CHAR_BIT - bit - 1))
				|| ASSERT(g_serial.interrupt == (bit == (CHAR_BIT - 1)))) {
			result = EXIT_FAILURE;
			goto exit;
		}

		in.raw >>= 1;
	}

	if(ASSERT(g_serial.serial.control.enable == false)
			|| ASSERT(g_serial.interrupt == true)) {
		result = EXIT_FAILURE;
		goto exit;
	}

exit:
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
			|| ASSERT(g_serial.serial.out == NULL)) {
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
	dmg_serial_control_t control = {};

	dmg_test_serial_initialize();
	dmg_serial_write(&g_serial.serial, ADDRESS_SERIAL_CONTROL, value);

	if(ASSERT(g_serial.serial.control.raw == (value & CONTROL_MASK))) {
		result = EXIT_FAILURE;
	}

	dmg_test_serial_initialize();
	g_serial.serial.cycle = rand();
	control.enable = true;
	dmg_serial_write(&g_serial.serial, ADDRESS_SERIAL_CONTROL, control.raw);

	if(ASSERT(g_serial.serial.control.raw == (control.raw & CONTROL_MASK))
			|| ASSERT(g_serial.serial.cycle == 0)
			|| ASSERT(g_serial.serial.remaining == CHAR_BIT)) {
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

static const dmg_test TEST[] = {
	dmg_test_serial_load,
	dmg_test_serial_read,
	dmg_test_serial_step,
	dmg_test_serial_transfer,
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
