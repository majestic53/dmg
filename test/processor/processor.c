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

#include "../../include/system/processor.h"
#include "../../src/system/processor_type.h"
#include "../include/common.h"

#define STEP_ADDRESS 0x1000

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
	dmg_processor_t processor;
	uint8_t read_address;
	uint8_t read_value;
	uint16_t write_address;
	uint8_t write_value;
} dmg_processor_test_t;

static dmg_processor_test_t g_processor = {};

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
	g_processor.export_file = file;
	g_processor.export_data = data;
	g_processor.export_length = length;

	return g_processor.export;
}

int
dmg_service_import_data(
	__in FILE *file,
	__in void *data,
	__in uint32_t length
	)
{
	g_processor.import_file = file;
	g_processor.import_data = data;
	g_processor.import_length = length;

	return g_processor.import;
}

uint8_t
dmg_runtime_read(
	__in uint16_t address
	)
{
	g_processor.read_address = address;

	return g_processor.read_value;
}

void
dmg_runtime_write(
	__in uint16_t address,
	__in uint8_t value
	)
{
	g_processor.write_address = address;
	g_processor.write_value = value;
}

static void
dmg_test_processor_initialize(void)
{
	memset(&g_processor, 0, sizeof(g_processor));
}

int
dmg_test_processor_load(void)
{
	int result = DMG_STATUS_SUCCESS;

	dmg_test_processor_initialize();

	if(ASSERT_SUCCESS(dmg_processor_load(&g_processor.processor, &g_processor.configuration)) != DMG_STATUS_SUCCESS) {
		result = DMG_STATUS_FAILURE;
	}

	if(ASSERT(g_processor.processor.af.word == POST_AF)
			|| ASSERT(g_processor.processor.bc.word == POST_BC)
			|| ASSERT(g_processor.processor.de.word == POST_DE)
			|| ASSERT(g_processor.processor.hl.word == POST_HL)
			|| ASSERT(g_processor.processor.pc.word == POST_PC)
			|| ASSERT(g_processor.processor.sp.word == POST_SP)
			|| ASSERT(g_processor.processor.interrupts_enable == false)
			|| ASSERT(g_processor.processor.interrupts_enable_state == INTERRUPTS_STATE_NONE)
			|| ASSERT(g_processor.processor.interrupt_enable.raw == 0)
			|| ASSERT(g_processor.processor.interrupt_flag.raw == POST_IF)
			|| ASSERT(g_processor.processor.halt == false)
			|| ASSERT(g_processor.processor.stop == false)) {
		result = DMG_STATUS_FAILURE;
	}

	dmg_test_processor_initialize();
	g_processor.configuration.bootrom.data = (void *)1;

	if(ASSERT_SUCCESS(dmg_processor_load(&g_processor.processor, &g_processor.configuration)) != DMG_STATUS_SUCCESS) {
		result = DMG_STATUS_FAILURE;
	}

	if(ASSERT(g_processor.processor.af.word == 0)
			|| ASSERT(g_processor.processor.bc.word == 0)
			|| ASSERT(g_processor.processor.de.word == 0)
			|| ASSERT(g_processor.processor.hl.word == 0)
			|| ASSERT(g_processor.processor.pc.word == 0)
			|| ASSERT(g_processor.processor.sp.word == 0)
			|| ASSERT(g_processor.processor.interrupts_enable == false)
			|| ASSERT(g_processor.processor.interrupts_enable_state == INTERRUPTS_STATE_NONE)
			|| ASSERT(g_processor.processor.interrupt_enable.raw == 0)
			|| ASSERT(g_processor.processor.interrupt_flag.raw == POST_IF)
			|| ASSERT(g_processor.processor.halt == false)
			|| ASSERT(g_processor.processor.stop == false)) {
		result = DMG_STATUS_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_processor_read(void)
{
	uint8_t value = rand();
	int result = DMG_STATUS_SUCCESS;

	dmg_test_processor_initialize();

	if(ASSERT(dmg_processor_read(&g_processor.processor, ADDRESS_PROCESSOR_INTERRUPT_ENABLE - 1) == UINT8_MAX)) {
		result = DMG_STATUS_FAILURE;
	}

	dmg_test_processor_initialize();
	g_processor.processor.interrupt_enable.raw = value;

	if(ASSERT(dmg_processor_read(&g_processor.processor, ADDRESS_PROCESSOR_INTERRUPT_ENABLE) == value)) {
		result = DMG_STATUS_FAILURE;
	}

	dmg_test_processor_initialize();
	g_processor.processor.interrupt_flag.raw = value;

	if(ASSERT(dmg_processor_read(&g_processor.processor, ADDRESS_PROCESSOR_INTERRUPT_FLAG) == value)) {
		result = DMG_STATUS_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_processor_read_register(void)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_action_t request = {}, response = {};

	request.type = DMG_ACTION_READ;
	request.data.dword = UINT32_MAX;

	for(request.address = DMG_REGISTER_PROCESSOR_A; request.address <= DMG_REGISTER_PROCESSOR_STOP; ++request.address) {
		uint16_t value = rand();
		uint32_t length = sizeof(uint8_t);

		dmg_test_processor_initialize();

		switch(request.address) {
			case DMG_REGISTER_PROCESSOR_A:
				value &= UINT8_MAX;
				g_processor.processor.af.high = value;
				break;
			case DMG_REGISTER_PROCESSOR_AF:
				g_processor.processor.af.word = value;
				length = sizeof(uint16_t);
				break;
			case DMG_REGISTER_PROCESSOR_B:
				value &= UINT8_MAX;
				g_processor.processor.bc.high = value;
				break;
			case DMG_REGISTER_PROCESSOR_BC:
				g_processor.processor.bc.word = value;
				length = sizeof(uint16_t);
				break;
			case DMG_REGISTER_PROCESSOR_C:
				value &= UINT8_MAX;
				g_processor.processor.bc.low = value;
				break;
			case DMG_REGISTER_PROCESSOR_D:
				value &= UINT8_MAX;
				g_processor.processor.de.high = value;
				break;
			case DMG_REGISTER_PROCESSOR_DE:
				g_processor.processor.de.word = value;
				length = sizeof(uint16_t);
				break;
			case DMG_REGISTER_PROCESSOR_E:
				value &= UINT8_MAX;
				g_processor.processor.de.low = value;
				break;
			case DMG_REGISTER_PROCESSOR_F:
				value &= UINT8_MAX;
				g_processor.processor.af.low = value;
				break;
			case DMG_REGISTER_PROCESSOR_H:
				value &= UINT8_MAX;
				g_processor.processor.hl.high = value;
				break;
			case DMG_REGISTER_PROCESSOR_HALT:
				value &= true;
				g_processor.processor.halt = value;
				break;
			case DMG_REGISTER_PROCESSOR_HL:
				g_processor.processor.hl.word = value;
				length = sizeof(uint16_t);
				break;
			case DMG_REGISTER_PROCESSOR_IE:
				value &= UINT8_MAX;
				g_processor.processor.interrupt_enable.raw = value;
				break;
			case DMG_REGISTER_PROCESSOR_IF:
				value &= UINT8_MAX;
				g_processor.processor.interrupt_flag.raw = value;
				break;
			case DMG_REGISTER_PROCESSOR_IME:
				value &= true;
				g_processor.processor.interrupts_enable = value;
				break;
			case DMG_REGISTER_PROCESSOR_L:
				value &= UINT8_MAX;
				g_processor.processor.hl.low = value;
				break;
			case DMG_REGISTER_PROCESSOR_PC:
				g_processor.processor.pc.word = value;
				length = sizeof(uint16_t);
				break;
			case DMG_REGISTER_PROCESSOR_SP:
				g_processor.processor.sp.word = value;
				length = sizeof(uint16_t);
				break;
			case DMG_REGISTER_PROCESSOR_STOP:
				value &= true;
				g_processor.processor.stop = value;
				break;
			default:
				break;
		}

		memset(&response, 0, sizeof(response));
		dmg_processor_read_register(&g_processor.processor, &request, &response);

		if(ASSERT(response.length == length)) {
			result = DMG_STATUS_FAILURE;
			break;
		}

		if(length <= sizeof(uint8_t)) {

			if(ASSERT(response.data.byte == value)) {
				result = DMG_STATUS_FAILURE;
				break;
			}
		} else if(ASSERT(response.data.word == value)) {
			result = DMG_STATUS_FAILURE;
			break;
		}
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_processor_step(void)
{
	int result = DMG_STATUS_SUCCESS;
	uint8_t opcode = INSTRUCTION_NOP;
	const dmg_processor_instruction_t *instruction = dmg_processor_instruction(opcode, false);

	dmg_test_processor_initialize();
	g_processor.processor.pc.word = STEP_ADDRESS;
	g_processor.read_value = opcode;

	if(ASSERT(dmg_processor_step(&g_processor.processor) == instruction->cycle)
			|| ASSERT(g_processor.processor.pc.word == (STEP_ADDRESS + instruction->operand + 1))) {
		result = DMG_STATUS_FAILURE;
	}

	for(int interrupt = INTERRUPT_VBLANK; interrupt < INTERRUPT_MAX; ++interrupt) {
		dmg_test_processor_initialize();
		g_processor.processor.pc.word = STEP_ADDRESS;
		g_processor.processor.interrupts_enable = false;
		g_processor.processor.interrupt_enable.raw = (1 << interrupt);
		g_processor.processor.interrupt_flag.raw = (1 << interrupt);
		g_processor.read_value = opcode;

		if(ASSERT(dmg_processor_step(&g_processor.processor) == instruction->cycle)
				|| ASSERT(g_processor.processor.pc.word == (STEP_ADDRESS + instruction->operand + 1))) {
			result = DMG_STATUS_FAILURE;
		}

		dmg_test_processor_initialize();
		g_processor.processor.pc.word = STEP_ADDRESS;
		g_processor.processor.interrupts_enable = true;
		g_processor.processor.interrupt_flag.raw = (1 << interrupt);
		g_processor.read_value = opcode;

		if(ASSERT(dmg_processor_step(&g_processor.processor) == instruction->cycle)
				|| ASSERT(g_processor.processor.pc.word == (STEP_ADDRESS + instruction->operand + 1))) {
			result = DMG_STATUS_FAILURE;
		}

		dmg_test_processor_initialize();
		g_processor.processor.pc.word = STEP_ADDRESS;
		g_processor.processor.interrupts_enable = true;
		g_processor.processor.interrupt_enable.raw = (1 << interrupt);
		g_processor.processor.interrupt_flag.raw = (1 << interrupt);
		g_processor.read_value = opcode;

		if(ASSERT(dmg_processor_step(&g_processor.processor) == (instruction->cycle + CYCLE_INTERRUPT))
				|| ASSERT(g_processor.processor.pc.word == (INTERRUPT_ADDR[interrupt] + instruction->operand + 1))) {
			result = DMG_STATUS_FAILURE;
		}
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_processor_unload(void)
{
	int result = DMG_STATUS_SUCCESS;

	dmg_test_processor_initialize();
	dmg_processor_load(&g_processor.processor, &g_processor.configuration);
	dmg_processor_unload(&g_processor.processor);

	if(ASSERT(g_processor.processor.af.word == 0)
			|| ASSERT(g_processor.processor.bc.word == 0)
			|| ASSERT(g_processor.processor.de.word == 0)
			|| ASSERT(g_processor.processor.hl.word == 0)
			|| ASSERT(g_processor.processor.pc.word == 0)
			|| ASSERT(g_processor.processor.sp.word == 0)
			|| ASSERT(g_processor.processor.interrupts_enable == false)
			|| ASSERT(g_processor.processor.interrupts_enable_state == INTERRUPTS_STATE_NONE)
			|| ASSERT(g_processor.processor.interrupt_enable.raw == 0)
			|| ASSERT(g_processor.processor.interrupt_flag.raw == 0)
			|| ASSERT(g_processor.processor.halt == false)
			|| ASSERT(g_processor.processor.stop == false)) {
		result = DMG_STATUS_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_processor_write(void)
{
	uint8_t value = rand();
	int result = DMG_STATUS_SUCCESS;

	dmg_test_processor_initialize();
	dmg_processor_write(&g_processor.processor, ADDRESS_PROCESSOR_INTERRUPT_ENABLE, value);

	if(ASSERT(g_processor.processor.interrupt_enable.raw == value)) {
		result = DMG_STATUS_FAILURE;
	}

	dmg_test_processor_initialize();
	dmg_processor_write(&g_processor.processor, ADDRESS_PROCESSOR_INTERRUPT_FLAG, value);

	if(ASSERT(g_processor.processor.interrupt_flag.raw == (POST_IF | (value & INTERRUPT_FLAG_MASK)))) {
		result = DMG_STATUS_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_processor_write_register(void)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_action_t request = {}, response = {};

	request.type = DMG_ACTION_WRITE;

	for(request.address = DMG_REGISTER_PROCESSOR_A; request.address <= DMG_REGISTER_PROCESSOR_STOP; ++request.address) {
		uint16_t expected = rand(), value = 0;

		request.data.dword = UINT32_MAX;

		switch(request.address) {
			case DMG_REGISTER_PROCESSOR_A:
			case DMG_REGISTER_PROCESSOR_B:
			case DMG_REGISTER_PROCESSOR_C:
			case DMG_REGISTER_PROCESSOR_D:
			case DMG_REGISTER_PROCESSOR_E:
			case DMG_REGISTER_PROCESSOR_F:
			case DMG_REGISTER_PROCESSOR_H:
			case DMG_REGISTER_PROCESSOR_IE:
			case DMG_REGISTER_PROCESSOR_IF:
			case DMG_REGISTER_PROCESSOR_L:
				request.data.byte = expected;
				break;
			case DMG_REGISTER_PROCESSOR_HALT:
			case DMG_REGISTER_PROCESSOR_IME:
			case DMG_REGISTER_PROCESSOR_STOP:
				request.data.byte = (expected % 2);
				break;
			case DMG_REGISTER_PROCESSOR_AF:
			case DMG_REGISTER_PROCESSOR_BC:
			case DMG_REGISTER_PROCESSOR_DE:
			case DMG_REGISTER_PROCESSOR_HL:
			case DMG_REGISTER_PROCESSOR_PC:
			case DMG_REGISTER_PROCESSOR_SP:
				request.data.word = expected;
				break;
			default:
				break;
		}

		dmg_test_processor_initialize();
		dmg_processor_write_register(&g_processor.processor, &request, &response);

		switch(request.address) {
			case DMG_REGISTER_PROCESSOR_A:
				expected &= UINT8_MAX;
				value = g_processor.processor.af.high;
				break;
			case DMG_REGISTER_PROCESSOR_AF:
				value = g_processor.processor.af.word;
				break;
			case DMG_REGISTER_PROCESSOR_B:
				expected &= UINT8_MAX;
				value = g_processor.processor.bc.high;
				break;
			case DMG_REGISTER_PROCESSOR_BC:
				value = g_processor.processor.bc.word;
				break;
			case DMG_REGISTER_PROCESSOR_C:
				expected &= UINT8_MAX;
				value = g_processor.processor.bc.low;
				break;
			case DMG_REGISTER_PROCESSOR_D:
				expected &= UINT8_MAX;
				value = g_processor.processor.de.high;
				break;
			case DMG_REGISTER_PROCESSOR_DE:
				value = g_processor.processor.de.word;
				break;
			case DMG_REGISTER_PROCESSOR_E:
				expected &= UINT8_MAX;
				value = g_processor.processor.de.low;
				break;
			case DMG_REGISTER_PROCESSOR_F:
				expected &= UINT8_MAX;
				value = g_processor.processor.af.low;
				break;
			case DMG_REGISTER_PROCESSOR_H:
				expected &= UINT8_MAX;
				value = g_processor.processor.hl.high;
				break;
			case DMG_REGISTER_PROCESSOR_HALT:
				expected &= true;
				value = g_processor.processor.halt;
				break;
			case DMG_REGISTER_PROCESSOR_HL:
				value = g_processor.processor.hl.word;
				break;
			case DMG_REGISTER_PROCESSOR_IE:
				expected &= UINT8_MAX;
				value = g_processor.processor.interrupt_enable.raw;
				break;
			case DMG_REGISTER_PROCESSOR_IF:
				expected &= UINT8_MAX;
				value = g_processor.processor.interrupt_flag.raw;
				break;
			case DMG_REGISTER_PROCESSOR_IME:
				expected &= true;
				value = g_processor.processor.interrupts_enable;
				break;
			case DMG_REGISTER_PROCESSOR_L:
				expected &= UINT8_MAX;
				value = g_processor.processor.hl.low;
				break;
			case DMG_REGISTER_PROCESSOR_PC:
				value = g_processor.processor.pc.word;
				break;
			case DMG_REGISTER_PROCESSOR_SP:
				value = g_processor.processor.sp.word;
				break;
			case DMG_REGISTER_PROCESSOR_STOP:
				expected &= true;
				value = g_processor.processor.stop;
				break;
			default:
				break;
		}

		if(ASSERT(value == expected)) {

fprintf(stdout, "%u %04x %04x\n", request.address, value, expected);

			result = DMG_STATUS_FAILURE;
			break;
		}
	}

	TRACE_TEST(result);

	return result;
}

static const dmg_test TEST[] = {
	dmg_test_processor_load,
	dmg_test_processor_read,
	dmg_test_processor_read_register,
	dmg_test_processor_step,
	dmg_test_processor_unload,
	dmg_test_processor_write,
	dmg_test_processor_write_register,
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
