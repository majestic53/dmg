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

#include "../../tool/include/assembler/generator.h"
#include "../../tool/src/assembler/generator_type.h"
#include "../include/common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SOURCE_MAX 64

static int
dmg_test_generator_instruction(void)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_generator_t generator = {};

	for(uint32_t opcode = 0; opcode <= UINT8_MAX; ++opcode) {
		const char *format;
		dmg_buffer_t buffer = {};
		char source[SOURCE_MAX] = {};
		dmg_assembler_scalar_t operand = {};
		const dmg_tool_syntax_instruction_t *instruction;

		if(opcode == INSTRUCTION_EXTENDED_PREFIX) {
			continue;
		}

		format = dmg_tool_syntax_instruction_string(opcode, false);
		instruction = dmg_tool_syntax_instruction(opcode, false);

		switch(instruction->operand) {
			case OPERAND_WORD:
				operand.word = rand();

				switch(opcode) {
					case INSTRUCTION_LD_A_U16_IND:
					case INSTRUCTION_LD_U16_IND_A:

						if(operand.word >= ADDRESS_IO_BASE) {
							operand.word = (ADDRESS_IO_BASE - 1);
						}
						break;
					default:
						break;
				}

				snprintf(source, SOURCE_MAX, format, operand.word);
				break;
			case OPERAND_BYTE:
				operand.low = rand();
				snprintf(source, SOURCE_MAX, format, operand.low);
				break;
			default:
				snprintf(source, SOURCE_MAX, format, "");
				break;
		}

		buffer.data = source;
		buffer.length = strlen(source);

		if((result = dmg_assembler_generator_load(&generator, &buffer, NULL, NULL)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_generator_run(&generator)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(ASSERT(generator.banks.bank[0].data[0] == opcode)) {
			result = DMG_STATUS_FAILURE;
			goto exit;
		}

		switch(instruction->operand) {
			case OPERAND_WORD:

				if(ASSERT(generator.banks.bank[0].data[1] == operand.low)
						&& ASSERT(generator.banks.bank[0].data[2] == operand.high)) {
					result = DMG_STATUS_FAILURE;
					goto exit;
				}
				break;
			case OPERAND_BYTE:

				if(ASSERT(generator.banks.bank[0].data[1] == ((opcode != INSTRUCTION_STOP) ? operand.low : 0))) {
					result = DMG_STATUS_FAILURE;
					goto exit;
				}
				break;
			default:
				break;
		}

		dmg_assembler_generator_unload(&generator);
	}

exit:
	dmg_assembler_generator_unload(&generator);
	TRACE_TEST(result);

	return result;
}

static int
dmg_test_generator_instruction_extended(void)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_generator_t generator = {};

	for(uint32_t opcode = 0; opcode <= UINT8_MAX; ++opcode) {
		dmg_buffer_t buffer = {};
		char source[SOURCE_MAX] = {};

		snprintf(source, SOURCE_MAX, dmg_tool_syntax_instruction_string(opcode, true), "");
		buffer.data = source;
		buffer.length = strlen(source);

		if((result = dmg_assembler_generator_load(&generator, &buffer, NULL, NULL)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_generator_run(&generator)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(ASSERT(generator.banks.bank[0].data[0] == INSTRUCTION_EXTENDED_PREFIX)
				&& ASSERT(generator.banks.bank[0].data[1] == opcode)) {
			result = DMG_STATUS_FAILURE;
			goto exit;
		}

		dmg_assembler_generator_unload(&generator);
	}

exit:
	dmg_assembler_generator_unload(&generator);
	TRACE_TEST(result);

	return result;
}

static const dmg_test TEST[] = {
	dmg_test_generator_instruction,
	dmg_test_generator_instruction_extended,
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
