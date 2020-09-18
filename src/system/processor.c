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

#include "./processor_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static inline uint8_t
dmg_processor_fetch(
	__inout dmg_processor_t *processor
	)
{
	return dmg_runtime_read(processor->pc.word++);
}

static inline uint16_t
dmg_processor_fetch_word(
	__inout dmg_processor_t *processor
	)
{
	return (dmg_processor_fetch(processor) | (dmg_processor_fetch(processor) << CHAR_BIT));
}

/*static inline uint8_t
dmg_processor_pop(
	__inout dmg_processor_t *processor
	)
{
	return dmg_runtime_read(processor->sp.word++);
}

static inline uint16_t
dmg_processor_pop_word(
	__inout dmg_processor_t *processor
	)
{
	return (dmg_processor_pop(processor) | (dmg_processor_pop(processor) << CHAR_BIT));
}*/

static inline void
dmg_processor_push(
	__inout dmg_processor_t *processor,
	__in uint8_t value
	)
{
	dmg_runtime_write(--processor->sp.word, value);
}

static inline void
dmg_processor_push_word(
	__inout dmg_processor_t *processor,
	__in uint16_t value
	)
{
	dmg_processor_push(processor, value >> CHAR_BIT);
	dmg_processor_push(processor, value);
}

static uint32_t
dmg_processor_instruction_nop(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	return instruction->cycle;
}

static const dmg_instruction_cb INSTRUCTION_HANDLER[] = {
	dmg_processor_instruction_nop, /* INSTRUCTION_NOP */

	// TODO: ADD ADDITIONAL INSTRUCTIONS

	};

static uint32_t
dmg_processor_instruction_extended_rlc(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t carry = {}, value = {};

	switch(instruction->opcode) {

		// TODO: IMPLEMENT RLC INSTRUCTION

		case INSTRUCTION_EXTENDED_RLC_B:
			value.low = processor->bc.high;
			break;

		// TODO: IMPLEMENT RLC INSTRUCTION

		default:
			break;
	}

	carry.low_lsb = processor->af.flag.carry;
	processor->af.flag.carry = value.low_msb;
	value.low <<= 1;
	value.low |= carry.low_lsb;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = !value.low;

	switch(instruction->opcode) {

		// TODO: IMPLEMENT RLC INSTRUCTION

		case INSTRUCTION_EXTENDED_RLC_B:
			processor->bc.high = value.low;
			break;

		// TODO: IMPLEMENT RLC INSTRUCTION

		default:
			break;
	}

	return instruction->cycle;
}

static const dmg_instruction_cb INSTRUCTION_EXTENDED_HANDLER[] = {
	dmg_processor_instruction_extended_rlc, /* INSTRUCTION_EXTENDED_RLC_B */

	// TODO: ADD ADDITIONAL EXTENDED INSTRUCTIONS

	};

static uint32_t
dmg_processor_execute(
	__inout dmg_processor_t *processor
	)
{
	uint32_t result = 0;

	if(!processor->halt && !processor->stop) {
		uint8_t opcode;
		dmg_register_t operand = {};
		const dmg_instruction_cb *handler;
		const dmg_instruction_t *instruction;

		if((opcode = dmg_processor_fetch(processor)) == INSTRUCTION_EXTENDED_PREFIX) {
			opcode = dmg_processor_fetch(processor);
			instruction = &INSTRUCTION_EXTENDED[opcode];
			handler = &INSTRUCTION_EXTENDED_HANDLER[opcode];
		} else {
			instruction = &INSTRUCTION[opcode];
			handler = &INSTRUCTION_HANDLER[opcode];
		}

		switch(instruction->operand) {
			case OPERAND_BYTE:
				operand.low = dmg_processor_fetch(processor);
				break;
			case OPERAND_WORD:
				operand.word = dmg_processor_fetch_word(processor);
				break;
			default:
				break;
		}

		result += (*handler)(processor, instruction, &operand);
	} else {
		result += CYCLE_IDLE;
	}

	return result;
}

static uint32_t
dmg_processor_service(
	__inout dmg_processor_t *processor
	)
{
	uint32_t result = 0;

	for(int type = 0; type < INTERRUPT_MAX; ++type) {

		if(processor->interrupt_enable.raw & processor->interrupt_flag.raw & (1 << type)) {

			if(processor->interrupts_enable) {
				processor->interrupt_flag.raw &= ~(1 << type);
				processor->interrupts_enable = false;
				dmg_processor_push_word(processor, processor->pc.word);
				processor->pc.word = INTERRUPT_ADDR[type];
				result += CYCLE_INTERRUPT;
			}

			if(processor->halt) {
				processor->halt = false;
				result += CYCLE_INTERRUPT_HALT;

				TRACE_FORMAT(LEVEL_VERBOSE, "Processor exiting halt state [%04x]", processor->pc.word);
			}
		}
	}

	return result;
}

int
dmg_processor_load(
	__inout dmg_processor_t *processor,
	__in const dmg_buffer_t *bootrom
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Processor loading");

	if(!bootrom->data) {
		processor->af.word = POST_AF;
		processor->bc.word = POST_BC;
		processor->de.word = POST_DE;
		processor->hl.word = POST_HL;
		processor->pc.word = POST_PC;
		processor->sp.word = POST_SP;
	}

	processor->interrupt_flag.raw = POST_IF;

	TRACE_FORMAT(LEVEL_VERBOSE, "Processor AF=%04x", processor->af.word);
	TRACE_FORMAT(LEVEL_VERBOSE, "Processor BC=%04x", processor->bc.word);
	TRACE_FORMAT(LEVEL_VERBOSE, "Processor DE=%04x", processor->de.word);
	TRACE_FORMAT(LEVEL_VERBOSE, "Processor HL=%04x", processor->hl.word);
	TRACE_FORMAT(LEVEL_VERBOSE, "Processor PC=%04x", processor->pc.word);
	TRACE_FORMAT(LEVEL_VERBOSE, "Processor SP=%04x", processor->sp.word);
	TRACE_FORMAT(LEVEL_VERBOSE, "Processor IME=%x", processor->interrupts_enable);
	TRACE_FORMAT(LEVEL_VERBOSE, "Processor IE=%02x", processor->interrupt_enable.raw);
	TRACE_FORMAT(LEVEL_VERBOSE, "Processor IF=%02x", processor->interrupt_flag.raw);
	TRACE_FORMAT(LEVEL_VERBOSE, "Processor Halt=%x", processor->halt);
	TRACE_FORMAT(LEVEL_VERBOSE, "Processor Stop=%x", processor->stop);
	TRACE(LEVEL_INFORMATION, "Processor loaded");

	return result;
}

uint8_t
dmg_processor_read(
	__in const dmg_processor_t *processor,
	__in uint16_t address
	)
{
	uint8_t result = 0;

	switch(address) {
		case ADDRESS_INTERRUPT_ENABLE:
			result = processor->interrupt_enable.raw;
			break;
		case ADDRESS_INTERRUPT_FLAG:
			result = processor->interrupt_flag.raw;
			break;
		default:
			result = UINT8_MAX;

			TRACE_FORMAT(LEVEL_WARNING, "Unsupported processor read [%04x]->%02x", address, result);
			break;
	}

	return result;
}

uint32_t
dmg_processor_step(
	__inout dmg_processor_t *processor
	)
{
	return (dmg_processor_service(processor) + dmg_processor_execute(processor));
}

void
dmg_processor_unload(
	__inout dmg_processor_t *processor
	)
{
	TRACE(LEVEL_INFORMATION, "Processor unloading");

	memset(processor, 0, sizeof(*processor));

	TRACE(LEVEL_INFORMATION, "Processor unloaded");
}

void
dmg_processor_write(
	__inout dmg_processor_t *processor,
	__in uint16_t address,
	__in uint8_t value
	)
{

	switch(address) {
		case ADDRESS_INTERRUPT_ENABLE:
			processor->interrupt_enable.raw = value;
			break;
		case ADDRESS_INTERRUPT_FLAG:
			processor->interrupt_flag.raw |= (value & INTERRUPT_FLAG_MASK);

			if(processor->stop && processor->interrupt_flag.joypad) {
				processor->stop = false;

				TRACE_FORMAT(LEVEL_VERBOSE, "Processor exiting stop state [%04x]", processor->pc.word);
			}
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported processor write [%04x]<-%02x", address, value);
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
