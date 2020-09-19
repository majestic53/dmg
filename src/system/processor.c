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
	dmg_processor_instruction_nop, /* 0x00 */

	// TODO: ADD ADDITIONAL INSTRUCTIONS

	};

static uint32_t
dmg_processor_instruction_extended_bit(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_BIT_0_A:
		case INSTRUCTION_EXTENDED_BIT_1_A:
		case INSTRUCTION_EXTENDED_BIT_2_A:
		case INSTRUCTION_EXTENDED_BIT_3_A:
		case INSTRUCTION_EXTENDED_BIT_4_A:
		case INSTRUCTION_EXTENDED_BIT_5_A:
		case INSTRUCTION_EXTENDED_BIT_6_A:
		case INSTRUCTION_EXTENDED_BIT_7_A:
			processor->af.flag.zero = !(processor->af.high
							& (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_BIT_0_A) / CHAR_BIT)));
			break;
		case INSTRUCTION_EXTENDED_BIT_0_B:
		case INSTRUCTION_EXTENDED_BIT_1_B:
		case INSTRUCTION_EXTENDED_BIT_2_B:
		case INSTRUCTION_EXTENDED_BIT_3_B:
		case INSTRUCTION_EXTENDED_BIT_4_B:
		case INSTRUCTION_EXTENDED_BIT_5_B:
		case INSTRUCTION_EXTENDED_BIT_6_B:
		case INSTRUCTION_EXTENDED_BIT_7_B:
			processor->af.flag.zero = !(processor->bc.high
							& (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_BIT_0_B) / CHAR_BIT)));
			break;
		case INSTRUCTION_EXTENDED_BIT_0_C:
		case INSTRUCTION_EXTENDED_BIT_1_C:
		case INSTRUCTION_EXTENDED_BIT_2_C:
		case INSTRUCTION_EXTENDED_BIT_3_C:
		case INSTRUCTION_EXTENDED_BIT_4_C:
		case INSTRUCTION_EXTENDED_BIT_5_C:
		case INSTRUCTION_EXTENDED_BIT_6_C:
		case INSTRUCTION_EXTENDED_BIT_7_C:
			processor->af.flag.zero = !(processor->bc.low
							& (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_BIT_0_C) / CHAR_BIT)));
			break;
		case INSTRUCTION_EXTENDED_BIT_0_D:
		case INSTRUCTION_EXTENDED_BIT_1_D:
		case INSTRUCTION_EXTENDED_BIT_2_D:
		case INSTRUCTION_EXTENDED_BIT_3_D:
		case INSTRUCTION_EXTENDED_BIT_4_D:
		case INSTRUCTION_EXTENDED_BIT_5_D:
		case INSTRUCTION_EXTENDED_BIT_6_D:
		case INSTRUCTION_EXTENDED_BIT_7_D:
			processor->af.flag.zero = !(processor->de.high
							& (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_BIT_0_D) / CHAR_BIT)));
			break;
		case INSTRUCTION_EXTENDED_BIT_0_E:
		case INSTRUCTION_EXTENDED_BIT_1_E:
		case INSTRUCTION_EXTENDED_BIT_2_E:
		case INSTRUCTION_EXTENDED_BIT_3_E:
		case INSTRUCTION_EXTENDED_BIT_4_E:
		case INSTRUCTION_EXTENDED_BIT_5_E:
		case INSTRUCTION_EXTENDED_BIT_6_E:
		case INSTRUCTION_EXTENDED_BIT_7_E:
			processor->af.flag.zero = !(processor->de.low
							& (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_BIT_0_E) / CHAR_BIT)));
			break;
		case INSTRUCTION_EXTENDED_BIT_0_H:
		case INSTRUCTION_EXTENDED_BIT_1_H:
		case INSTRUCTION_EXTENDED_BIT_2_H:
		case INSTRUCTION_EXTENDED_BIT_3_H:
		case INSTRUCTION_EXTENDED_BIT_4_H:
		case INSTRUCTION_EXTENDED_BIT_5_H:
		case INSTRUCTION_EXTENDED_BIT_6_H:
		case INSTRUCTION_EXTENDED_BIT_7_H:
			processor->af.flag.zero = !(processor->hl.high
							& (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_BIT_0_H) / CHAR_BIT)));
			break;
		case INSTRUCTION_EXTENDED_BIT_0_HL_IND:
		case INSTRUCTION_EXTENDED_BIT_1_HL_IND:
		case INSTRUCTION_EXTENDED_BIT_2_HL_IND:
		case INSTRUCTION_EXTENDED_BIT_3_HL_IND:
		case INSTRUCTION_EXTENDED_BIT_4_HL_IND:
		case INSTRUCTION_EXTENDED_BIT_5_HL_IND:
		case INSTRUCTION_EXTENDED_BIT_6_HL_IND:
		case INSTRUCTION_EXTENDED_BIT_7_HL_IND:
			processor->af.flag.zero = !(dmg_runtime_read(processor->hl.word)
							& (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_BIT_0_HL_IND) / CHAR_BIT)));
			break;
		case INSTRUCTION_EXTENDED_BIT_0_L:
		case INSTRUCTION_EXTENDED_BIT_1_L:
		case INSTRUCTION_EXTENDED_BIT_2_L:
		case INSTRUCTION_EXTENDED_BIT_3_L:
		case INSTRUCTION_EXTENDED_BIT_4_L:
		case INSTRUCTION_EXTENDED_BIT_5_L:
		case INSTRUCTION_EXTENDED_BIT_6_L:
		case INSTRUCTION_EXTENDED_BIT_7_L:
			processor->af.flag.zero = !(processor->hl.low
							& (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_BIT_0_L) / CHAR_BIT)));
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	processor->af.flag.carry_half = true;
	processor->af.flag.subtract = false;

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_extended_res(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_RES_0_A:
		case INSTRUCTION_EXTENDED_RES_1_A:
		case INSTRUCTION_EXTENDED_RES_2_A:
		case INSTRUCTION_EXTENDED_RES_3_A:
		case INSTRUCTION_EXTENDED_RES_4_A:
		case INSTRUCTION_EXTENDED_RES_5_A:
		case INSTRUCTION_EXTENDED_RES_6_A:
		case INSTRUCTION_EXTENDED_RES_7_A:
			processor->af.high &= ~(1 << ((instruction->opcode - INSTRUCTION_EXTENDED_RES_0_A) / CHAR_BIT));
			break;
		case INSTRUCTION_EXTENDED_RES_0_B:
		case INSTRUCTION_EXTENDED_RES_1_B:
		case INSTRUCTION_EXTENDED_RES_2_B:
		case INSTRUCTION_EXTENDED_RES_3_B:
		case INSTRUCTION_EXTENDED_RES_4_B:
		case INSTRUCTION_EXTENDED_RES_5_B:
		case INSTRUCTION_EXTENDED_RES_6_B:
		case INSTRUCTION_EXTENDED_RES_7_B:
			processor->bc.high &= ~(1 << ((instruction->opcode - INSTRUCTION_EXTENDED_RES_0_B) / CHAR_BIT));
			break;
		case INSTRUCTION_EXTENDED_RES_0_C:
		case INSTRUCTION_EXTENDED_RES_1_C:
		case INSTRUCTION_EXTENDED_RES_2_C:
		case INSTRUCTION_EXTENDED_RES_3_C:
		case INSTRUCTION_EXTENDED_RES_4_C:
		case INSTRUCTION_EXTENDED_RES_5_C:
		case INSTRUCTION_EXTENDED_RES_6_C:
		case INSTRUCTION_EXTENDED_RES_7_C:
			processor->bc.low &= ~(1 << ((instruction->opcode - INSTRUCTION_EXTENDED_RES_0_C) / CHAR_BIT));
			break;
		case INSTRUCTION_EXTENDED_RES_0_D:
		case INSTRUCTION_EXTENDED_RES_1_D:
		case INSTRUCTION_EXTENDED_RES_2_D:
		case INSTRUCTION_EXTENDED_RES_3_D:
		case INSTRUCTION_EXTENDED_RES_4_D:
		case INSTRUCTION_EXTENDED_RES_5_D:
		case INSTRUCTION_EXTENDED_RES_6_D:
		case INSTRUCTION_EXTENDED_RES_7_D:
			processor->de.high &= ~(1 << ((instruction->opcode - INSTRUCTION_EXTENDED_RES_0_D) / CHAR_BIT));
			break;
		case INSTRUCTION_EXTENDED_RES_0_E:
		case INSTRUCTION_EXTENDED_RES_1_E:
		case INSTRUCTION_EXTENDED_RES_2_E:
		case INSTRUCTION_EXTENDED_RES_3_E:
		case INSTRUCTION_EXTENDED_RES_4_E:
		case INSTRUCTION_EXTENDED_RES_5_E:
		case INSTRUCTION_EXTENDED_RES_6_E:
		case INSTRUCTION_EXTENDED_RES_7_E:
			processor->de.low &= ~(1 << ((instruction->opcode - INSTRUCTION_EXTENDED_RES_0_E) / CHAR_BIT));
			break;
		case INSTRUCTION_EXTENDED_RES_0_H:
		case INSTRUCTION_EXTENDED_RES_1_H:
		case INSTRUCTION_EXTENDED_RES_2_H:
		case INSTRUCTION_EXTENDED_RES_3_H:
		case INSTRUCTION_EXTENDED_RES_4_H:
		case INSTRUCTION_EXTENDED_RES_5_H:
		case INSTRUCTION_EXTENDED_RES_6_H:
		case INSTRUCTION_EXTENDED_RES_7_H:
			processor->hl.high &= ~(1 << ((instruction->opcode - INSTRUCTION_EXTENDED_RES_0_H) / CHAR_BIT));
			break;
		case INSTRUCTION_EXTENDED_RES_0_HL_IND:
		case INSTRUCTION_EXTENDED_RES_1_HL_IND:
		case INSTRUCTION_EXTENDED_RES_2_HL_IND:
		case INSTRUCTION_EXTENDED_RES_3_HL_IND:
		case INSTRUCTION_EXTENDED_RES_4_HL_IND:
		case INSTRUCTION_EXTENDED_RES_5_HL_IND:
		case INSTRUCTION_EXTENDED_RES_6_HL_IND:
		case INSTRUCTION_EXTENDED_RES_7_HL_IND:
			dmg_runtime_write(processor->hl.word, dmg_runtime_read(processor->hl.word)
								& ~(1 << ((instruction->opcode - INSTRUCTION_EXTENDED_RES_0_HL_IND) / CHAR_BIT)));
			break;
		case INSTRUCTION_EXTENDED_RES_0_L:
		case INSTRUCTION_EXTENDED_RES_1_L:
		case INSTRUCTION_EXTENDED_RES_2_L:
		case INSTRUCTION_EXTENDED_RES_3_L:
		case INSTRUCTION_EXTENDED_RES_4_L:
		case INSTRUCTION_EXTENDED_RES_5_L:
		case INSTRUCTION_EXTENDED_RES_6_L:
		case INSTRUCTION_EXTENDED_RES_7_L:
			processor->hl.low &= ~(1 << ((instruction->opcode - INSTRUCTION_EXTENDED_RES_0_L) / CHAR_BIT));
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_extended_rl(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t carry = {}, value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_RL_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_EXTENDED_RL_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_EXTENDED_RL_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_EXTENDED_RL_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_EXTENDED_RL_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_EXTENDED_RL_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_EXTENDED_RL_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_EXTENDED_RL_L:
			value.low = processor->hl.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
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
		case INSTRUCTION_EXTENDED_RL_A:
			processor->af.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RL_B:
			processor->bc.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RL_C:
			processor->bc.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_RL_D:
			processor->de.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RL_E:
			processor->de.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_RL_H:
			processor->hl.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RL_HL_IND:
			dmg_runtime_write(processor->hl.word, value.low);
			break;
		case INSTRUCTION_EXTENDED_RL_L:
			processor->hl.low = value.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_extended_rlc(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t carry = {}, value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_RLC_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_EXTENDED_RLC_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_EXTENDED_RLC_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_EXTENDED_RLC_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_EXTENDED_RLC_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_EXTENDED_RLC_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_EXTENDED_RLC_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_EXTENDED_RLC_L:
			value.low = processor->hl.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	carry.low_lsb = value.low_msb;
	processor->af.flag.carry = carry.low_lsb;
	value.low <<= 1;
	value.low |= carry.low_lsb;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = !value.low;

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_RLC_A:
			processor->af.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RLC_B:
			processor->bc.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RLC_C:
			processor->bc.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_RLC_D:
			processor->de.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RLC_E:
			processor->de.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_RLC_H:
			processor->hl.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RLC_HL_IND:
			dmg_runtime_write(processor->hl.word, value.low);
			break;
		case INSTRUCTION_EXTENDED_RLC_L:
			processor->hl.low = value.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_extended_rr(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t carry = {}, value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_RR_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_EXTENDED_RR_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_EXTENDED_RR_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_EXTENDED_RR_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_EXTENDED_RR_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_EXTENDED_RR_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_EXTENDED_RR_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_EXTENDED_RR_L:
			value.low = processor->hl.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	carry.low_msb = processor->af.flag.carry;
	processor->af.flag.carry = value.low_lsb;
	value.low >>= 1;
	value.low |= carry.low_msb;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = !value.low;

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_RR_A:
			processor->af.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RR_B:
			processor->bc.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RR_C:
			processor->bc.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_RR_D:
			processor->de.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RR_E:
			processor->de.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_RR_H:
			processor->hl.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RR_HL_IND:
			dmg_runtime_write(processor->hl.word, value.low);
			break;
		case INSTRUCTION_EXTENDED_RR_L:
			processor->hl.low = value.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_extended_rrc(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t carry = {}, value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_RRC_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_EXTENDED_RRC_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_EXTENDED_RRC_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_EXTENDED_RRC_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_EXTENDED_RRC_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_EXTENDED_RRC_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_EXTENDED_RRC_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_EXTENDED_RRC_L:
			value.low = processor->hl.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	carry.low_msb = value.low_lsb;
	processor->af.flag.carry = carry.low_msb;
	value.low >>= 1;
	value.low |= carry.low_msb;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = !value.low;

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_RRC_A:
			processor->af.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RRC_B:
			processor->bc.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RRC_C:
			processor->bc.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_RRC_D:
			processor->de.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RRC_E:
			processor->de.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_RRC_H:
			processor->hl.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_RRC_HL_IND:
			dmg_runtime_write(processor->hl.word, value.low);
			break;
		case INSTRUCTION_EXTENDED_RRC_L:
			processor->hl.low = value.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_extended_set(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_SET_0_A:
		case INSTRUCTION_EXTENDED_SET_1_A:
		case INSTRUCTION_EXTENDED_SET_2_A:
		case INSTRUCTION_EXTENDED_SET_3_A:
		case INSTRUCTION_EXTENDED_SET_4_A:
		case INSTRUCTION_EXTENDED_SET_5_A:
		case INSTRUCTION_EXTENDED_SET_6_A:
		case INSTRUCTION_EXTENDED_SET_7_A:
			processor->af.high |= (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_SET_0_A) / CHAR_BIT));
			break;
		case INSTRUCTION_EXTENDED_SET_0_B:
		case INSTRUCTION_EXTENDED_SET_1_B:
		case INSTRUCTION_EXTENDED_SET_2_B:
		case INSTRUCTION_EXTENDED_SET_3_B:
		case INSTRUCTION_EXTENDED_SET_4_B:
		case INSTRUCTION_EXTENDED_SET_5_B:
		case INSTRUCTION_EXTENDED_SET_6_B:
		case INSTRUCTION_EXTENDED_SET_7_B:
			processor->bc.high |= (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_SET_0_B) / CHAR_BIT));
			break;
		case INSTRUCTION_EXTENDED_SET_0_C:
		case INSTRUCTION_EXTENDED_SET_1_C:
		case INSTRUCTION_EXTENDED_SET_2_C:
		case INSTRUCTION_EXTENDED_SET_3_C:
		case INSTRUCTION_EXTENDED_SET_4_C:
		case INSTRUCTION_EXTENDED_SET_5_C:
		case INSTRUCTION_EXTENDED_SET_6_C:
		case INSTRUCTION_EXTENDED_SET_7_C:
			processor->bc.low |= (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_SET_0_C) / CHAR_BIT));
			break;
		case INSTRUCTION_EXTENDED_SET_0_D:
		case INSTRUCTION_EXTENDED_SET_1_D:
		case INSTRUCTION_EXTENDED_SET_2_D:
		case INSTRUCTION_EXTENDED_SET_3_D:
		case INSTRUCTION_EXTENDED_SET_4_D:
		case INSTRUCTION_EXTENDED_SET_5_D:
		case INSTRUCTION_EXTENDED_SET_6_D:
		case INSTRUCTION_EXTENDED_SET_7_D:
			processor->de.high |= (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_SET_0_D) / CHAR_BIT));
			break;
		case INSTRUCTION_EXTENDED_SET_0_E:
		case INSTRUCTION_EXTENDED_SET_1_E:
		case INSTRUCTION_EXTENDED_SET_2_E:
		case INSTRUCTION_EXTENDED_SET_3_E:
		case INSTRUCTION_EXTENDED_SET_4_E:
		case INSTRUCTION_EXTENDED_SET_5_E:
		case INSTRUCTION_EXTENDED_SET_6_E:
		case INSTRUCTION_EXTENDED_SET_7_E:
			processor->de.low |= (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_SET_0_E) / CHAR_BIT));
			break;
		case INSTRUCTION_EXTENDED_SET_0_H:
		case INSTRUCTION_EXTENDED_SET_1_H:
		case INSTRUCTION_EXTENDED_SET_2_H:
		case INSTRUCTION_EXTENDED_SET_3_H:
		case INSTRUCTION_EXTENDED_SET_4_H:
		case INSTRUCTION_EXTENDED_SET_5_H:
		case INSTRUCTION_EXTENDED_SET_6_H:
		case INSTRUCTION_EXTENDED_SET_7_H:
			processor->hl.high |= (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_SET_0_H) / CHAR_BIT));
			break;
		case INSTRUCTION_EXTENDED_SET_0_HL_IND:
		case INSTRUCTION_EXTENDED_SET_1_HL_IND:
		case INSTRUCTION_EXTENDED_SET_2_HL_IND:
		case INSTRUCTION_EXTENDED_SET_3_HL_IND:
		case INSTRUCTION_EXTENDED_SET_4_HL_IND:
		case INSTRUCTION_EXTENDED_SET_5_HL_IND:
		case INSTRUCTION_EXTENDED_SET_6_HL_IND:
		case INSTRUCTION_EXTENDED_SET_7_HL_IND:
			dmg_runtime_write(processor->hl.word, dmg_runtime_read(processor->hl.word)
								| (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_SET_0_HL_IND) / CHAR_BIT)));
			break;
		case INSTRUCTION_EXTENDED_SET_0_L:
		case INSTRUCTION_EXTENDED_SET_1_L:
		case INSTRUCTION_EXTENDED_SET_2_L:
		case INSTRUCTION_EXTENDED_SET_3_L:
		case INSTRUCTION_EXTENDED_SET_4_L:
		case INSTRUCTION_EXTENDED_SET_5_L:
		case INSTRUCTION_EXTENDED_SET_6_L:
		case INSTRUCTION_EXTENDED_SET_7_L:
			processor->hl.low |= (1 << ((instruction->opcode - INSTRUCTION_EXTENDED_SET_0_L) / CHAR_BIT));
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_extended_sla(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_SLA_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_EXTENDED_SLA_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_EXTENDED_SLA_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_EXTENDED_SLA_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_EXTENDED_SLA_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_EXTENDED_SLA_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_EXTENDED_SLA_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_EXTENDED_SLA_L:
			value.low = processor->hl.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	processor->af.flag.carry = value.low_msb;
	value.low <<= 1;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = !value.low;

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_SLA_A:
			processor->af.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SLA_B:
			processor->bc.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SLA_C:
			processor->bc.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_SLA_D:
			processor->de.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SLA_E:
			processor->de.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_SLA_H:
			processor->hl.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SLA_HL_IND:
			dmg_runtime_write(processor->hl.word, value.low);
			break;
		case INSTRUCTION_EXTENDED_SLA_L:
			processor->hl.low = value.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_extended_sra(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t carry = {}, value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_SRA_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_EXTENDED_SRA_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_EXTENDED_SRA_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_EXTENDED_SRA_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_EXTENDED_SRA_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_EXTENDED_SRA_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_EXTENDED_SRA_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_EXTENDED_SRA_L:
			value.low = processor->hl.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	carry.low_msb = value.low_msb;
	processor->af.flag.carry = value.low_lsb;
	value.low >>= 1;
	value.low_msb = carry.low_msb;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = !value.low;

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_SRA_A:
			processor->af.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SRA_B:
			processor->bc.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SRA_C:
			processor->bc.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_SRA_D:
			processor->de.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SRA_E:
			processor->de.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_SRA_H:
			processor->hl.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SRA_HL_IND:
			dmg_runtime_write(processor->hl.word, value.low);
			break;
		case INSTRUCTION_EXTENDED_SRA_L:
			processor->hl.low = value.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_extended_srl(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_SRL_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_EXTENDED_SRL_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_EXTENDED_SRL_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_EXTENDED_SRL_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_EXTENDED_SRL_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_EXTENDED_SRL_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_EXTENDED_SRL_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_EXTENDED_SRL_L:
			value.low = processor->hl.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	processor->af.flag.carry = value.low_lsb;
	value.low >>= 1;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = !value.low;

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_SRL_A:
			processor->af.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SRL_B:
			processor->bc.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SRL_C:
			processor->bc.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_SRL_D:
			processor->de.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SRL_E:
			processor->de.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_SRL_H:
			processor->hl.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SRL_HL_IND:
			dmg_runtime_write(processor->hl.word, value.low);
			break;
		case INSTRUCTION_EXTENDED_SRL_L:
			processor->hl.low = value.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_extended_swap(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_SWAP_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_EXTENDED_SWAP_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_EXTENDED_SWAP_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_EXTENDED_SWAP_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_EXTENDED_SWAP_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_EXTENDED_SWAP_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_EXTENDED_SWAP_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_EXTENDED_SWAP_L:
			value.low = processor->hl.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	value.low = ((value.low << NIBBLE_BIT) | (value.low >> NIBBLE_BIT));
	processor->af.flag.carry = false;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = !value.low;

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_SWAP_A:
			processor->af.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SWAP_B:
			processor->bc.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SWAP_C:
			processor->bc.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_SWAP_D:
			processor->de.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SWAP_E:
			processor->de.low = value.low;
			break;
		case INSTRUCTION_EXTENDED_SWAP_H:
			processor->hl.high = value.low;
			break;
		case INSTRUCTION_EXTENDED_SWAP_HL_IND:
			dmg_runtime_write(processor->hl.word, value.low);
			break;
		case INSTRUCTION_EXTENDED_SWAP_L:
			processor->hl.low = value.low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	return instruction->cycle;
}

static const dmg_instruction_cb INSTRUCTION_EXTENDED_HANDLER[] = {
	dmg_processor_instruction_extended_rlc, /* 0x00 */
	dmg_processor_instruction_extended_rlc,
	dmg_processor_instruction_extended_rlc,
	dmg_processor_instruction_extended_rlc,
	dmg_processor_instruction_extended_rlc,
	dmg_processor_instruction_extended_rlc,
	dmg_processor_instruction_extended_rlc,
	dmg_processor_instruction_extended_rlc,
	dmg_processor_instruction_extended_rrc, /* 0x08 */
	dmg_processor_instruction_extended_rrc,
	dmg_processor_instruction_extended_rrc,
	dmg_processor_instruction_extended_rrc,
	dmg_processor_instruction_extended_rrc,
	dmg_processor_instruction_extended_rrc,
	dmg_processor_instruction_extended_rrc,
	dmg_processor_instruction_extended_rrc,
	dmg_processor_instruction_extended_rl, /* 0x10 */
	dmg_processor_instruction_extended_rl,
	dmg_processor_instruction_extended_rl,
	dmg_processor_instruction_extended_rl,
	dmg_processor_instruction_extended_rl,
	dmg_processor_instruction_extended_rl,
	dmg_processor_instruction_extended_rl,
	dmg_processor_instruction_extended_rl,
	dmg_processor_instruction_extended_rr, /* 0x18 */
	dmg_processor_instruction_extended_rr,
	dmg_processor_instruction_extended_rr,
	dmg_processor_instruction_extended_rr,
	dmg_processor_instruction_extended_rr,
	dmg_processor_instruction_extended_rr,
	dmg_processor_instruction_extended_rr,
	dmg_processor_instruction_extended_rr,
	dmg_processor_instruction_extended_sla, /* 0x20 */
	dmg_processor_instruction_extended_sla,
	dmg_processor_instruction_extended_sla,
	dmg_processor_instruction_extended_sla,
	dmg_processor_instruction_extended_sla,
	dmg_processor_instruction_extended_sla,
	dmg_processor_instruction_extended_sla,
	dmg_processor_instruction_extended_sla,
	dmg_processor_instruction_extended_sra, /* 0x28 */
	dmg_processor_instruction_extended_sra,
	dmg_processor_instruction_extended_sra,
	dmg_processor_instruction_extended_sra,
	dmg_processor_instruction_extended_sra,
	dmg_processor_instruction_extended_sra,
	dmg_processor_instruction_extended_sra,
	dmg_processor_instruction_extended_sra,
	dmg_processor_instruction_extended_swap, /* 0x30 */
	dmg_processor_instruction_extended_swap,
	dmg_processor_instruction_extended_swap,
	dmg_processor_instruction_extended_swap,
	dmg_processor_instruction_extended_swap,
	dmg_processor_instruction_extended_swap,
	dmg_processor_instruction_extended_swap,
	dmg_processor_instruction_extended_swap,
	dmg_processor_instruction_extended_srl, /* 0x38 */
	dmg_processor_instruction_extended_srl,
	dmg_processor_instruction_extended_srl,
	dmg_processor_instruction_extended_srl,
	dmg_processor_instruction_extended_srl,
	dmg_processor_instruction_extended_srl,
	dmg_processor_instruction_extended_srl,
	dmg_processor_instruction_extended_srl,
	dmg_processor_instruction_extended_bit, /* 0x40 */
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit, /* 0x48 */
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit, /* 0x50 */
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit, /* 0x58 */
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit, /* 0x60 */
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit, /* 0x68 */
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit, /* 0x70 */
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit, /* 0x78 */
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_bit,
	dmg_processor_instruction_extended_res, /* 0x80 */
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res, /* 0x88 */
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res, /* 0x90 */
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res, /* 0x98 */
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res, /* 0xa0 */
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res, /* 0xa8 */
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res, /* 0xb0 */
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res, /* 0xb8 */
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_res,
	dmg_processor_instruction_extended_set, /* 0xc0 */
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set, /* 0xc8 */
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set, /* 0xd0 */
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set, /* 0xd8 */
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set, /* 0xe0 */
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set, /* 0xe8 */
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set, /* 0xf0 */
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set, /* 0xf8 */
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
	dmg_processor_instruction_extended_set,
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
