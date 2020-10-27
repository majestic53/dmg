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

#include "../../include/system/processor.h"
#include "../../src/system/processor_type.h"
#include "../include/common.h"

#define STEP_ADDRESS 0x1000

typedef struct {
	dmg_t configuration;
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
	int result = EXIT_SUCCESS;

	dmg_test_processor_initialize();

	if(ASSERT_SUCCESS(dmg_processor_load(&g_processor.processor, &g_processor.configuration)) != EXIT_SUCCESS) {
		result = EXIT_FAILURE;
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
		result = EXIT_FAILURE;
	}

	dmg_test_processor_initialize();
	g_processor.configuration.bootrom.data = (void *)1;

	if(ASSERT_SUCCESS(dmg_processor_load(&g_processor.processor, &g_processor.configuration)) != EXIT_SUCCESS) {
		result = EXIT_FAILURE;
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
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_processor_read(void)
{
	uint8_t value = rand();
	int result = EXIT_SUCCESS;

	dmg_test_processor_initialize();

	if(ASSERT(dmg_processor_read(&g_processor.processor, ADDRESS_PROCESSOR_INTERRUPT_ENABLE - 1) == UINT8_MAX)) {
		result = EXIT_FAILURE;
	}

	dmg_test_processor_initialize();
	g_processor.processor.interrupt_enable.raw = value;

	if(ASSERT(dmg_processor_read(&g_processor.processor, ADDRESS_PROCESSOR_INTERRUPT_ENABLE) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_processor_initialize();
	g_processor.processor.interrupt_flag.raw = value;

	if(ASSERT(dmg_processor_read(&g_processor.processor, ADDRESS_PROCESSOR_INTERRUPT_FLAG) == value)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_processor_step(void)
{
	int result = EXIT_SUCCESS;
	uint8_t opcode = INSTRUCTION_NOP;
	const dmg_processor_instruction_t *instruction = &INSTRUCTION[opcode];

	dmg_test_processor_initialize();
	g_processor.processor.pc.word = STEP_ADDRESS;
	g_processor.read_value = opcode;

	if(ASSERT(dmg_processor_step(&g_processor.processor) == instruction->cycle)
			|| ASSERT(g_processor.processor.pc.word == (STEP_ADDRESS + instruction->operand + 1))) {
		result = EXIT_FAILURE;
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
			result = EXIT_FAILURE;
		}

		dmg_test_processor_initialize();
		g_processor.processor.pc.word = STEP_ADDRESS;
		g_processor.processor.interrupts_enable = true;
		g_processor.processor.interrupt_flag.raw = (1 << interrupt);
		g_processor.read_value = opcode;

		if(ASSERT(dmg_processor_step(&g_processor.processor) == instruction->cycle)
				|| ASSERT(g_processor.processor.pc.word == (STEP_ADDRESS + instruction->operand + 1))) {
			result = EXIT_FAILURE;
		}

		dmg_test_processor_initialize();
		g_processor.processor.pc.word = STEP_ADDRESS;
		g_processor.processor.interrupts_enable = true;
		g_processor.processor.interrupt_enable.raw = (1 << interrupt);
		g_processor.processor.interrupt_flag.raw = (1 << interrupt);
		g_processor.read_value = opcode;

		if(ASSERT(dmg_processor_step(&g_processor.processor) == (instruction->cycle + CYCLE_INTERRUPT))
				|| ASSERT(g_processor.processor.pc.word == (INTERRUPT_ADDR[interrupt] + instruction->operand + 1))) {
			result = EXIT_FAILURE;
		}
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_processor_unload(void)
{
	int result = EXIT_SUCCESS;

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
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_processor_write(void)
{
	uint8_t value = rand();
	int result = EXIT_SUCCESS;

	dmg_test_processor_initialize();
	dmg_processor_write(&g_processor.processor, ADDRESS_PROCESSOR_INTERRUPT_ENABLE, value);

	if(ASSERT(g_processor.processor.interrupt_enable.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_processor_initialize();
	dmg_processor_write(&g_processor.processor, ADDRESS_PROCESSOR_INTERRUPT_FLAG, value);

	if(ASSERT(g_processor.processor.interrupt_flag.raw == (POST_IF | (value & INTERRUPT_FLAG_MASK)))) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

static const dmg_test_cb TEST[] = {
	dmg_test_processor_load,
	dmg_test_processor_read,
	dmg_test_processor_step,
	dmg_test_processor_unload,
	dmg_test_processor_write,
	};

int
main(
	__in int argc,
	__in char *argv[]
	)
{
	int result = EXIT_SUCCESS;

	TEST_SETUP();

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
