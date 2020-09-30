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

static inline uint8_t
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
}

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
dmg_processor_instruction_adc(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t carry = {}, sum = {}, value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_ADC_A_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_ADC_A_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_ADC_A_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_ADC_A_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_ADC_A_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_ADC_A_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_ADC_A_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_ADC_A_L:
			value.low = processor->hl.low;
			break;
		case INSTRUCTION_ADC_A_U8:
			value.low = operand->low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	carry.low_lsb = processor->af.flag.carry;
	sum.word = (processor->af.high + value.low + carry.low_lsb);
	processor->af.flag.carry = (sum.word > UINT8_MAX);
	processor->af.flag.carry_half = (((processor->af.high & NIBBLE_MAX) + (value.low & NIBBLE_MAX) + carry.low_lsb) > NIBBLE_MAX);
	processor->af.high = sum.low;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = !processor->af.high;

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_add(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t carry = {}, sum = {}, value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_ADD_A_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_ADD_A_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_ADD_A_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_ADD_A_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_ADD_A_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_ADD_A_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_ADD_A_L:
			value.low = processor->hl.low;
			break;
		case INSTRUCTION_ADD_A_U8:
			value.low = operand->low;
			break;
		case INSTRUCTION_ADD_HL_BC:
			value.word = processor->bc.word;
			break;
		case INSTRUCTION_ADD_HL_DE:
			value.word = processor->de.word;
			break;
		case INSTRUCTION_ADD_HL_HL:
			value.word = processor->hl.word;
			break;
		case INSTRUCTION_ADD_A_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_ADD_HL_SP:
			value.word = processor->sp.word;
			break;
		case INSTRUCTION_ADD_SP_I8:
			value.low = operand->low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	switch(instruction->opcode) {
		case INSTRUCTION_ADD_A_A:
		case INSTRUCTION_ADD_A_B:
		case INSTRUCTION_ADD_A_C:
		case INSTRUCTION_ADD_A_D:
		case INSTRUCTION_ADD_A_E:
		case INSTRUCTION_ADD_A_H:
		case INSTRUCTION_ADD_A_HL_IND:
		case INSTRUCTION_ADD_A_L:
		case INSTRUCTION_ADD_A_U8:
			sum.word = (processor->af.high + value.low);
			carry.word = (processor->af.high ^ sum.word ^ value.low);
			processor->af.high = sum.low;
			processor->af.flag.carry = ((carry.word & (1 << CHAR_BIT)) == (1 << CHAR_BIT));
			processor->af.flag.carry_half = ((carry.word & (1 << NIBBLE_BIT)) == (1 << NIBBLE_BIT));
			processor->af.flag.subtract = false;
			processor->af.flag.zero = !processor->af.high;
			break;
		case INSTRUCTION_ADD_HL_BC:
		case INSTRUCTION_ADD_HL_DE:
		case INSTRUCTION_ADD_HL_HL:
		case INSTRUCTION_ADD_HL_SP:
			sum.dword = (processor->hl.word + value.word);
			carry.dword = (processor->hl.word ^ sum.dword ^ value.word);
			processor->hl.word = sum.word;
			processor->af.flag.carry = ((carry.dword & (1 << (CHAR_BIT * 2))) == (1 << (CHAR_BIT * 2)));
			processor->af.flag.carry_half = ((carry.dword & (1 << (NIBBLE_BIT * 3))) == (1 << (NIBBLE_BIT * 3)));
			processor->af.flag.subtract = false;
			break;
		case INSTRUCTION_ADD_SP_I8:
			sum.dword = (processor->sp.word + (int8_t)value.low);
			carry.dword = (processor->sp.word ^ sum.dword ^ (int8_t)value.low);
			processor->sp.word = sum.word;
			processor->af.flag.carry = ((carry.word & (1 << CHAR_BIT)) == (1 << CHAR_BIT));
			processor->af.flag.carry_half = ((carry.word & (1 << NIBBLE_BIT)) == (1 << NIBBLE_BIT));
			processor->af.flag.subtract = false;
			processor->af.flag.zero = false;
			break;
		default:
			break;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_and(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_AND_A_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_AND_A_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_AND_A_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_AND_A_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_AND_A_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_AND_A_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_AND_A_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_AND_A_L:
			value.low = processor->hl.low;
			break;
		case INSTRUCTION_AND_A_U8:
			value.low = operand->low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	processor->af.high &= value.low;
	processor->af.flag.carry = false;
	processor->af.flag.carry_half = true;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = !processor->af.high;

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_call(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	bool taken = false;

	switch(instruction->opcode) {
		case INSTRUCTION_CALL_C_U16:
			taken = processor->af.flag.carry;
			break;
		case INSTRUCTION_CALL_NC_U16:
			taken = !processor->af.flag.carry;
			break;
		case INSTRUCTION_CALL_NZ_U16:
			taken = !processor->af.flag.zero;
			break;
		case INSTRUCTION_CALL_U16:
			taken = true;
			break;
		case INSTRUCTION_CALL_Z_U16:
			taken = processor->af.flag.zero;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	if(taken) {
		dmg_processor_push_word(processor, processor->pc.word);
		processor->pc.word = operand->word;
	}

	return (taken ? instruction->cycle_taken : instruction->cycle);
}

static uint32_t
dmg_processor_instruction_ccf(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	processor->af.flag.carry = !processor->af.flag.carry;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_cp(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_CP_A_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_CP_A_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_CP_A_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_CP_A_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_CP_A_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_CP_A_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_CP_A_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_CP_A_L:
			value.low = processor->hl.low;
			break;
		case INSTRUCTION_CP_A_U8:
			value.low = operand->low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	processor->af.flag.carry = (processor->af.high < value.low);
	processor->af.flag.carry_half = ((processor->af.high & NIBBLE_MAX) < ((processor->af.high - value.low) & NIBBLE_MAX));
	processor->af.flag.subtract = true;
	processor->af.flag.zero = (processor->af.high == value.low);

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_cpl(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	processor->af.high = ~processor->af.high;
	processor->af.flag.carry_half = true;
	processor->af.flag.subtract = true;

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_daa(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{

	if(!processor->af.flag.subtract) {

		if(processor->af.flag.carry || (processor->af.high > DAA_MAX_HIGH)) {
			processor->af.high += DAA_OFFSET_HIGH;
			processor->af.flag.carry = true;
		}

		if(processor->af.flag.carry_half || ((processor->af.high & NIBBLE_MAX) > DAA_MAX_LOW)) {
			processor->af.high += DAA_OFFSET_LOW;
		}
	} else {

		if(processor->af.flag.carry) {
			processor->af.high -= DAA_OFFSET_HIGH;
		}

		if(processor->af.flag.carry_half) {
			processor->af.high -= DAA_OFFSET_LOW;
		}
	}

	processor->af.flag.carry_half = false;
	processor->af.flag.zero = !processor->af.high;

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_dec(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_DEC_A:
			value.low = --processor->af.high;
			break;
		case INSTRUCTION_DEC_B:
			value.low = --processor->bc.high;
			break;
		case INSTRUCTION_DEC_BC:
			--processor->bc.word;
			break;
		case INSTRUCTION_DEC_C:
			value.low = --processor->bc.low;
			break;
		case INSTRUCTION_DEC_D:
			value.low = --processor->de.high;
			break;
		case INSTRUCTION_DEC_DE:
			--processor->de.word;
			break;
		case INSTRUCTION_DEC_E:
			value.low = --processor->de.low;
			break;
		case INSTRUCTION_DEC_H:
			value.low = --processor->hl.high;
			break;
		case INSTRUCTION_DEC_HL:
			--processor->hl.word;
			break;
		case INSTRUCTION_DEC_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			dmg_runtime_write(processor->hl.word, --value.low);
			break;
		case INSTRUCTION_DEC_L:
			value.low = --processor->hl.low;
			break;
		case INSTRUCTION_DEC_SP:
			--processor->sp.word;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	switch(instruction->opcode) {
		case INSTRUCTION_DEC_A:
		case INSTRUCTION_DEC_B:
		case INSTRUCTION_DEC_C:
		case INSTRUCTION_DEC_D:
		case INSTRUCTION_DEC_E:
		case INSTRUCTION_DEC_H:
		case INSTRUCTION_DEC_HL_IND:
		case INSTRUCTION_DEC_L:
			processor->af.flag.carry_half = ((value.low & NIBBLE_MAX) == NIBBLE_MAX);
			processor->af.flag.subtract = true;
			processor->af.flag.zero = !value.low;
			break;
		default:
			break;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_di(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{

	if(processor->interrupts_enable) {
		processor->interrupts_enable_state = INTERRUPTS_STATE_PENDING;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_ei(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{

	if(!processor->interrupts_enable) {
		processor->interrupts_enable_state = INTERRUPTS_STATE_PENDING;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_halt(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	processor->halt = true;

	TRACE_FORMAT(LEVEL_VERBOSE, "Processor entering halt state [%04x]", processor->pc.word);

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_inc(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_INC_A:
			value.low = ++processor->af.high;
			break;
		case INSTRUCTION_INC_B:
			value.low = ++processor->bc.high;
			break;
		case INSTRUCTION_INC_BC:
			++processor->bc.word;
			break;
		case INSTRUCTION_INC_C:
			value.low = ++processor->bc.low;
			break;
		case INSTRUCTION_INC_D:
			value.low = ++processor->de.high;
			break;
		case INSTRUCTION_INC_DE:
			++processor->de.word;
			break;
		case INSTRUCTION_INC_E:
			value.low = ++processor->de.low;
			break;
		case INSTRUCTION_INC_H:
			value.low = ++processor->hl.high;
			break;
		case INSTRUCTION_INC_HL:
			++processor->hl.word;
			break;
		case INSTRUCTION_INC_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			dmg_runtime_write(processor->hl.word, ++value.low);
			break;
		case INSTRUCTION_INC_L:
			value.low = ++processor->hl.low;
			break;
		case INSTRUCTION_INC_SP:
			++processor->sp.word;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	switch(instruction->opcode) {
		case INSTRUCTION_INC_A:
		case INSTRUCTION_INC_B:
		case INSTRUCTION_INC_C:
		case INSTRUCTION_INC_D:
		case INSTRUCTION_INC_E:
		case INSTRUCTION_INC_H:
		case INSTRUCTION_INC_HL_IND:
		case INSTRUCTION_INC_L:
			processor->af.flag.carry_half = !(value.low & NIBBLE_MAX);
			processor->af.flag.subtract = false;
			processor->af.flag.zero = !value.low;
			break;
		default:
			break;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_jp(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	bool taken = false;
	dmg_register_t value = {};

	value.word = operand->word;

	switch(instruction->opcode) {
		case INSTRUCTION_JP_C_U16:
			taken = processor->af.flag.carry;
			break;
		case INSTRUCTION_JP_HL:
			value.word = processor->hl.word;
			taken = true;
			break;
		case INSTRUCTION_JP_NC_U16:
			taken = !processor->af.flag.carry;
			break;
		case INSTRUCTION_JP_NZ_U16:
			taken = !processor->af.flag.zero;
			break;
		case INSTRUCTION_JP_U16:
			taken = true;
			break;
		case INSTRUCTION_JP_Z_U16:
			taken = processor->af.flag.zero;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	if(taken) {
		processor->pc.word = value.word;
	}

	return (taken ? instruction->cycle_taken : instruction->cycle);
}

static uint32_t
dmg_processor_instruction_jr(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	bool taken = false;

	switch(instruction->opcode) {
		case INSTRUCTION_JR_C_I8:
			taken = processor->af.flag.carry;
			break;
		case INSTRUCTION_JR_NC_I8:
			taken = !processor->af.flag.carry;
			break;
		case INSTRUCTION_JR_NZ_I8:
			taken = !processor->af.flag.zero;
			break;
		case INSTRUCTION_JR_I8:
			taken = true;
			break;
		case INSTRUCTION_JR_Z_I8:
			taken = processor->af.flag.zero;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	if(taken) {
		processor->pc.word += (int8_t)operand->low;
	}

	return (taken ? instruction->cycle_taken : instruction->cycle);
}

static uint32_t
dmg_processor_instruction_ld(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t carry = {}, sum = {};

	switch(instruction->opcode) {
		case INSTRUCTION_LD_A_A:
			break;
		case INSTRUCTION_LD_A_B:
			processor->af.high = processor->bc.high;
			break;
		case INSTRUCTION_LD_A_BC_IND:
			processor->af.high = dmg_runtime_read(processor->bc.word);
			break;
		case INSTRUCTION_LD_A_C:
			processor->af.high = processor->bc.low;
			break;
		case INSTRUCTION_LD_A_D:
			processor->af.high = processor->de.high;
			break;
		case INSTRUCTION_LD_A_DE_IND:
			processor->af.high = dmg_runtime_read(processor->de.word);
			break;
		case INSTRUCTION_LD_A_E:
			processor->af.high = processor->de.low;
			break;
		case INSTRUCTION_LD_A_FF00_C_IND:
			processor->af.high = dmg_runtime_read(ADDRESS_IO_BASE + processor->bc.low);
			break;
		case INSTRUCTION_LD_A_FF00_U8_IND:
			processor->af.high = dmg_runtime_read(ADDRESS_IO_BASE + operand->low);
			break;
		case INSTRUCTION_LD_A_H:
			processor->af.high = processor->hl.high;
			break;
		case INSTRUCTION_LD_A_HL_IND:
			processor->af.high = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_LD_A_HL_IND_DEC:
			processor->af.high = dmg_runtime_read(processor->hl.word--);
			break;
		case INSTRUCTION_LD_A_HL_IND_INC:
			processor->af.high = dmg_runtime_read(processor->hl.word++);
			break;
		case INSTRUCTION_LD_A_L:
			processor->af.high = processor->hl.low;
			break;
		case INSTRUCTION_LD_A_U16_IND:
			processor->af.high = dmg_runtime_read(operand->word);
			break;
		case INSTRUCTION_LD_A_U8:
			processor->af.high = operand->low;
			break;
		case INSTRUCTION_LD_B_A:
			processor->bc.high = processor->af.high;
			break;
		case INSTRUCTION_LD_B_B:
			break;
		case INSTRUCTION_LD_B_C:
			processor->bc.high = processor->bc.low;
			break;
		case INSTRUCTION_LD_B_D:
			processor->bc.high = processor->de.high;
			break;
		case INSTRUCTION_LD_B_E:
			processor->bc.high = processor->de.low;
			break;
		case INSTRUCTION_LD_B_H:
			processor->bc.high = processor->hl.high;
			break;
		case INSTRUCTION_LD_B_HL_IND:
			processor->bc.high = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_LD_B_L:
			processor->bc.high = processor->hl.low;
			break;
		case INSTRUCTION_LD_B_U8:
			processor->bc.high = operand->low;
			break;
		case INSTRUCTION_LD_BC_IND_A:
			dmg_runtime_write(processor->bc.word, processor->af.high);
			break;
		case INSTRUCTION_LD_BC_U16:
			processor->bc.word = operand->word;
			break;
		case INSTRUCTION_LD_C_A:
			processor->bc.low = processor->af.high;
			break;
		case INSTRUCTION_LD_C_B:
			processor->bc.low = processor->bc.high;
			break;
		case INSTRUCTION_LD_C_C:
			break;
		case INSTRUCTION_LD_C_D:
			processor->bc.low = processor->de.high;
			break;
		case INSTRUCTION_LD_C_E:
			processor->bc.low = processor->de.low;
			break;
		case INSTRUCTION_LD_C_H:
			processor->bc.low = processor->hl.high;
			break;
		case INSTRUCTION_LD_C_HL_IND:
			processor->bc.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_LD_C_L:
			processor->bc.low = processor->hl.low;
			break;
		case INSTRUCTION_LD_C_U8:
			processor->bc.low = operand->low;
			break;
		case INSTRUCTION_LD_D_A:
			processor->de.high = processor->af.high;
			break;
		case INSTRUCTION_LD_D_B:
			processor->de.high = processor->bc.high;
			break;
		case INSTRUCTION_LD_D_C:
			processor->de.high = processor->bc.low;
			break;
		case INSTRUCTION_LD_D_D:
			break;
		case INSTRUCTION_LD_D_E:
			processor->de.high = processor->de.low;
			break;
		case INSTRUCTION_LD_D_H:
			processor->de.high = processor->hl.high;
			break;
		case INSTRUCTION_LD_D_HL_IND:
			processor->de.high = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_LD_D_L:
			processor->de.high = processor->hl.low;
			break;
		case INSTRUCTION_LD_D_U8:
			processor->de.high = operand->low;
			break;
		case INSTRUCTION_LD_DE_IND_A:
			dmg_runtime_write(processor->de.word, processor->af.high);
			break;
		case INSTRUCTION_LD_DE_U16:
			processor->de.word = operand->word;
			break;
		case INSTRUCTION_LD_E_A:
			processor->de.low = processor->af.high;
			break;
		case INSTRUCTION_LD_E_B:
			processor->de.low = processor->bc.high;
			break;
		case INSTRUCTION_LD_E_C:
			processor->de.low = processor->bc.low;
			break;
		case INSTRUCTION_LD_E_D:
			processor->de.low = processor->de.high;
			break;
		case INSTRUCTION_LD_E_E:
			break;
		case INSTRUCTION_LD_E_H:
			processor->de.low = processor->hl.high;
			break;
		case INSTRUCTION_LD_E_HL_IND:
			processor->de.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_LD_E_L:
			processor->de.low = processor->hl.low;
			break;
		case INSTRUCTION_LD_E_U8:
			processor->de.low = operand->low;
			break;
		case INSTRUCTION_LD_FF00_C_IND_A:
			dmg_runtime_write(ADDRESS_IO_BASE + processor->bc.low, processor->af.high);
			break;
		case INSTRUCTION_LD_FF00_U8_IND_A:
			dmg_runtime_write(ADDRESS_IO_BASE + operand->low, processor->af.high);
			break;
		case INSTRUCTION_LD_H_A:
			processor->hl.high = processor->af.high;
			break;
		case INSTRUCTION_LD_H_B:
			processor->hl.high = processor->bc.high;
			break;
		case INSTRUCTION_LD_H_C:
			processor->hl.high = processor->bc.low;
			break;
		case INSTRUCTION_LD_H_D:
			processor->hl.high = processor->de.high;
			break;
		case INSTRUCTION_LD_H_E:
			processor->hl.high = processor->de.low;
			break;
		case INSTRUCTION_LD_H_H:
			break;
		case INSTRUCTION_LD_H_HL_IND:
			processor->hl.high = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_LD_H_L:
			processor->hl.high = processor->hl.low;
			break;
		case INSTRUCTION_LD_H_U8:
			processor->hl.high = operand->low;
			break;
		case INSTRUCTION_LD_HL_IND_A:
			dmg_runtime_write(processor->hl.word, processor->af.high);
			break;
		case INSTRUCTION_LD_HL_IND_B:
			dmg_runtime_write(processor->hl.word, processor->bc.high);
			break;
		case INSTRUCTION_LD_HL_IND_C:
			dmg_runtime_write(processor->hl.word, processor->bc.low);
			break;
		case INSTRUCTION_LD_HL_IND_D:
			dmg_runtime_write(processor->hl.word, processor->de.high);
			break;
		case INSTRUCTION_LD_HL_IND_DEC_A:
			dmg_runtime_write(processor->hl.word--, processor->af.high);
			break;
		case INSTRUCTION_LD_HL_IND_E:
			dmg_runtime_write(processor->hl.word, processor->de.low);
			break;
		case INSTRUCTION_LD_HL_IND_H:
			dmg_runtime_write(processor->hl.word, processor->hl.high);
			break;
		case INSTRUCTION_LD_HL_IND_INC_A:
			dmg_runtime_write(processor->hl.word++, processor->af.high);
			break;
		case INSTRUCTION_LD_HL_IND_L:
			dmg_runtime_write(processor->hl.word, processor->hl.low);
			break;
		case INSTRUCTION_LD_HL_IND_U8:
			dmg_runtime_write(processor->hl.word, operand->low);
			break;
		case INSTRUCTION_LD_HL_SP_I8:
			sum.word = (processor->sp.word + (int8_t)operand->low);
			carry.word = (processor->sp.word ^ sum.word ^ (int8_t)operand->low);
			processor->hl.word = sum.word;
			processor->af.flag.carry = ((carry.word & (1 << CHAR_BIT)) == (1 << CHAR_BIT));
			processor->af.flag.carry_half = ((carry.low & (1 << NIBBLE_BIT)) == (1 << NIBBLE_BIT));
			processor->af.flag.subtract = false;
			processor->af.flag.zero = false;
			break;
		case INSTRUCTION_LD_HL_U16:
			processor->hl.word = operand->word;
			break;
		case INSTRUCTION_LD_L_A:
			processor->hl.low = processor->af.high;
			break;
		case INSTRUCTION_LD_L_B:
			processor->hl.low = processor->bc.high;
			break;
		case INSTRUCTION_LD_L_C:
			processor->hl.low = processor->bc.low;
			break;
		case INSTRUCTION_LD_L_D:
			processor->hl.low = processor->de.high;
			break;
		case INSTRUCTION_LD_L_E:
			processor->hl.low = processor->de.low;
			break;
		case INSTRUCTION_LD_L_H:
			processor->hl.low = processor->hl.high;
			break;
		case INSTRUCTION_LD_L_HL_IND:
			processor->hl.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_LD_L_L:
			break;
		case INSTRUCTION_LD_L_U8:
			processor->hl.low = operand->low;
			break;
		case INSTRUCTION_LD_SP_HL:
			processor->sp.word = processor->hl.word;
			break;
		case INSTRUCTION_LD_SP_U16:
			processor->sp.word = operand->word;
			break;
		case INSTRUCTION_LD_U16_IND_A:
			dmg_runtime_write(operand->word, processor->af.high);
			break;
		case INSTRUCTION_LD_U16_IND_SP:
			dmg_runtime_write(operand->word, processor->sp.low);
			dmg_runtime_write(operand->word + 1, processor->sp.high);
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	return instruction->cycle;
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

static uint32_t
dmg_processor_instruction_or(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_OR_A_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_OR_A_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_OR_A_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_OR_A_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_OR_A_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_OR_A_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_OR_A_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_OR_A_L:
			value.low = processor->hl.low;
			break;
		case INSTRUCTION_OR_A_U8:
			value.low = operand->low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	processor->af.high |= value.low;
	processor->af.flag.carry = false;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = !processor->af.high;

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_pop(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{

	switch(instruction->opcode) {
		case INSTRUCTION_POP_AF:
			processor->af.word = dmg_processor_pop_word(processor);
			processor->af.low &= FLAG_MASK;
			break;
		case INSTRUCTION_POP_BC:
			processor->bc.word = dmg_processor_pop_word(processor);
			break;
		case INSTRUCTION_POP_DE:
			processor->de.word = dmg_processor_pop_word(processor);
			break;
		case INSTRUCTION_POP_HL:
			processor->hl.word = dmg_processor_pop_word(processor);
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_push(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{

	switch(instruction->opcode) {
		case INSTRUCTION_PUSH_AF:
			dmg_processor_push_word(processor, processor->af.word);
			break;
		case INSTRUCTION_PUSH_BC:
			dmg_processor_push_word(processor, processor->bc.word);
			break;
		case INSTRUCTION_PUSH_DE:
			dmg_processor_push_word(processor, processor->de.word);
			break;
		case INSTRUCTION_PUSH_HL:
			dmg_processor_push_word(processor, processor->hl.word);
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_ret(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	bool taken = false;

	switch(instruction->opcode) {
		case INSTRUCTION_RET:
			taken = true;
			break;
		case INSTRUCTION_RET_C:
			taken = processor->af.flag.carry;
			break;
		case INSTRUCTION_RET_NC:
			taken = !processor->af.flag.carry;
			break;
		case INSTRUCTION_RET_NZ:
			taken = !processor->af.flag.zero;
			break;
		case INSTRUCTION_RET_Z:
			taken = processor->af.flag.zero;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	if(taken) {
		processor->pc.word = dmg_processor_pop_word(processor);
	}

	return (taken ? instruction->cycle_taken : instruction->cycle);
}

static uint32_t
dmg_processor_instruction_reti(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	processor->pc.word = dmg_processor_pop_word(processor);
	processor->interrupts_enable = true;
	processor->interrupts_enable_state = INTERRUPTS_STATE_NONE;

	return instruction->cycle_taken;
}

static uint32_t
dmg_processor_instruction_rla(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t carry = {};

	carry.low_lsb = processor->af.flag.carry;
	processor->af.flag.carry = processor->af.high_msb;
	processor->af.high <<= 1;
	processor->af.high_lsb = carry.low_lsb;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = false;

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_rlca(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t carry = {};

	carry.low_lsb = processor->af.high_msb;
	processor->af.flag.carry = carry.low_lsb;
	processor->af.high <<= 1;
	processor->af.high_lsb = carry.low_lsb;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = false;

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_rra(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t carry = {};

	carry.low_msb = processor->af.flag.carry;
	processor->af.flag.carry = processor->af.high_lsb;
	processor->af.high >>= 1;
	processor->af.high_msb = carry.low_msb;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = false;

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_rrca(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t carry = {};

	carry.low_msb = processor->af.high_lsb;
	processor->af.flag.carry = carry.low_msb;
	processor->af.high >>= 1;
	processor->af.high_msb = carry.low_msb;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = false;

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_rst(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{

	switch(instruction->opcode) {
		case INSTRUCTION_RST_00:
		case INSTRUCTION_RST_08:
		case INSTRUCTION_RST_10:
		case INSTRUCTION_RST_18:
		case INSTRUCTION_RST_20:
		case INSTRUCTION_RST_28:
		case INSTRUCTION_RST_30:
		case INSTRUCTION_RST_38:
			dmg_processor_push_word(processor, processor->pc.word);
			processor->pc.word = (instruction->opcode - INSTRUCTION_RST_00);
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_sbc(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t carry = {}, sum = {}, value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_SBC_A_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_SBC_A_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_SBC_A_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_SBC_A_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_SBC_A_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_SBC_A_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_SBC_A_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_SBC_A_L:
			value.low = processor->hl.low;
			break;
		case INSTRUCTION_SBC_A_U8:
			value.low = operand->low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	carry.low_lsb = processor->af.flag.carry;
	sum.word = (processor->af.high - value.low - carry.low_lsb);
	processor->af.flag.carry = (((int16_t)sum.word) < 0);
	processor->af.flag.carry_half = ((int16_t)((processor->af.high & NIBBLE_MAX) - (value.low & NIBBLE_MAX) - carry.low_lsb) < 0);
	processor->af.high = sum.low;
	processor->af.flag.subtract = true;
	processor->af.flag.zero = !processor->af.high;

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_scf(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	processor->af.flag.carry = true;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_stop(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	processor->stop = true;

	TRACE_FORMAT(LEVEL_VERBOSE, "Processor entering stop state [%04x], %02x", processor->pc.word, operand->low);

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_sub(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t carry = {}, sum = {}, value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_SUB_A_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_SUB_A_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_SUB_A_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_SUB_A_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_SUB_A_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_SUB_A_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_SUB_A_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_SUB_A_L:
			value.low = processor->hl.low;
			break;
		case INSTRUCTION_SUB_A_U8:
			value.low = operand->low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	sum.word = (processor->af.high - value.low);
	carry.word = (processor->af.high ^ sum.word ^ value.low);
	processor->af.high = sum.low;
	processor->af.flag.carry = ((carry.word & (1 << CHAR_BIT)) == (1 << CHAR_BIT));
	processor->af.flag.carry_half = ((carry.word & (1 << NIBBLE_BIT)) == (1 << NIBBLE_BIT));
	processor->af.flag.subtract = true;
	processor->af.flag.zero = !processor->af.high;

	return instruction->cycle;
}

static uint32_t
dmg_processor_instruction_unused(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{

	switch(instruction->opcode) {
		case INSTRUCTION_EXTENDED_PREFIX:
		case INSTRUCTION_UNUSED_D3:
		case INSTRUCTION_UNUSED_DB:
		case INSTRUCTION_UNUSED_DD:
		case INSTRUCTION_UNUSED_E3:
		case INSTRUCTION_UNUSED_E4:
		case INSTRUCTION_UNUSED_EB:
		case INSTRUCTION_UNUSED_EC:
		case INSTRUCTION_UNUSED_ED:
		case INSTRUCTION_UNUSED_F4:
		case INSTRUCTION_UNUSED_FC:
		case INSTRUCTION_UNUSED_FD:
			TRACE_FORMAT(LEVEL_WARNING, "Processor encountered unused instruction [%04x]->%02x", processor->pc.word - 1, instruction->opcode);
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	return dmg_processor_instruction_stop(processor, instruction, operand);
}

static uint32_t
dmg_processor_instruction_xor(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	)
{
	dmg_register_t value = {};

	switch(instruction->opcode) {
		case INSTRUCTION_XOR_A_A:
			value.low = processor->af.high;
			break;
		case INSTRUCTION_XOR_A_B:
			value.low = processor->bc.high;
			break;
		case INSTRUCTION_XOR_A_C:
			value.low = processor->bc.low;
			break;
		case INSTRUCTION_XOR_A_D:
			value.low = processor->de.high;
			break;
		case INSTRUCTION_XOR_A_E:
			value.low = processor->de.low;
			break;
		case INSTRUCTION_XOR_A_H:
			value.low = processor->hl.high;
			break;
		case INSTRUCTION_XOR_A_HL_IND:
			value.low = dmg_runtime_read(processor->hl.word);
			break;
		case INSTRUCTION_XOR_A_L:
			value.low = processor->hl.low;
			break;
		case INSTRUCTION_XOR_A_U8:
			value.low = operand->low;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported opcode %02x", instruction->opcode);
			break;
	}

	processor->af.high ^= value.low;
	processor->af.flag.carry = false;
	processor->af.flag.carry_half = false;
	processor->af.flag.subtract = false;
	processor->af.flag.zero = !processor->af.high;

	return instruction->cycle;
}

static const dmg_instruction_cb INSTRUCTION_HANDLER[] = {
	dmg_processor_instruction_nop, /* 0x00 */
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_inc,
	dmg_processor_instruction_inc,
	dmg_processor_instruction_dec,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_rlca,
	dmg_processor_instruction_ld, /* 0x08 */
	dmg_processor_instruction_add,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_dec,
	dmg_processor_instruction_inc,
	dmg_processor_instruction_dec,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_rrca,
	dmg_processor_instruction_stop, /* 0x10 */
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_inc,
	dmg_processor_instruction_inc,
	dmg_processor_instruction_dec,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_rla,
	dmg_processor_instruction_jr, /* 0x18 */
	dmg_processor_instruction_add,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_dec,
	dmg_processor_instruction_inc,
	dmg_processor_instruction_dec,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_rra,
	dmg_processor_instruction_jr, /* 0x20 */
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_inc,
	dmg_processor_instruction_inc,
	dmg_processor_instruction_dec,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_daa,
	dmg_processor_instruction_jr, /* 0x28 */
	dmg_processor_instruction_add,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_dec,
	dmg_processor_instruction_inc,
	dmg_processor_instruction_dec,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_cpl,
	dmg_processor_instruction_jr, /* 0x30 */
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_inc,
	dmg_processor_instruction_inc,
	dmg_processor_instruction_dec,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_scf,
	dmg_processor_instruction_jr, /* 0x38 */
	dmg_processor_instruction_add,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_dec,
	dmg_processor_instruction_inc,
	dmg_processor_instruction_dec,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ccf,
	dmg_processor_instruction_ld, /* 0x40 */
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld, /* 0x48 */
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld, /* 0x50 */
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld, /* 0x58 */
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld, /* 0x60 */
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld, /* 0x68 */
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld, /* 0x70 */
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_halt,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld, /* 0x78 */
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_add, /* 0x80 */
	dmg_processor_instruction_add,
	dmg_processor_instruction_add,
	dmg_processor_instruction_add,
	dmg_processor_instruction_add,
	dmg_processor_instruction_add,
	dmg_processor_instruction_add,
	dmg_processor_instruction_add,
	dmg_processor_instruction_adc, /* 0x88 */
	dmg_processor_instruction_adc,
	dmg_processor_instruction_adc,
	dmg_processor_instruction_adc,
	dmg_processor_instruction_adc,
	dmg_processor_instruction_adc,
	dmg_processor_instruction_adc,
	dmg_processor_instruction_adc,
	dmg_processor_instruction_sub, /* 0x90 */
	dmg_processor_instruction_sub,
	dmg_processor_instruction_sub,
	dmg_processor_instruction_sub,
	dmg_processor_instruction_sub,
	dmg_processor_instruction_sub,
	dmg_processor_instruction_sub,
	dmg_processor_instruction_sub,
	dmg_processor_instruction_sbc, /* 0x98 */
	dmg_processor_instruction_sbc,
	dmg_processor_instruction_sbc,
	dmg_processor_instruction_sbc,
	dmg_processor_instruction_sbc,
	dmg_processor_instruction_sbc,
	dmg_processor_instruction_sbc,
	dmg_processor_instruction_sbc,
	dmg_processor_instruction_and, /* 0xa0 */
	dmg_processor_instruction_and,
	dmg_processor_instruction_and,
	dmg_processor_instruction_and,
	dmg_processor_instruction_and,
	dmg_processor_instruction_and,
	dmg_processor_instruction_and,
	dmg_processor_instruction_and,
	dmg_processor_instruction_xor, /* 0xa8 */
	dmg_processor_instruction_xor,
	dmg_processor_instruction_xor,
	dmg_processor_instruction_xor,
	dmg_processor_instruction_xor,
	dmg_processor_instruction_xor,
	dmg_processor_instruction_xor,
	dmg_processor_instruction_xor,
	dmg_processor_instruction_or, /* 0xb0 */
	dmg_processor_instruction_or,
	dmg_processor_instruction_or,
	dmg_processor_instruction_or,
	dmg_processor_instruction_or,
	dmg_processor_instruction_or,
	dmg_processor_instruction_or,
	dmg_processor_instruction_or,
	dmg_processor_instruction_cp, /* 0xb8 */
	dmg_processor_instruction_cp,
	dmg_processor_instruction_cp,
	dmg_processor_instruction_cp,
	dmg_processor_instruction_cp,
	dmg_processor_instruction_cp,
	dmg_processor_instruction_cp,
	dmg_processor_instruction_cp,
	dmg_processor_instruction_ret, /* 0xc0 */
	dmg_processor_instruction_pop,
	dmg_processor_instruction_jp,
	dmg_processor_instruction_jp,
	dmg_processor_instruction_call,
	dmg_processor_instruction_push,
	dmg_processor_instruction_add,
	dmg_processor_instruction_rst,
	dmg_processor_instruction_ret, /* 0xc8 */
	dmg_processor_instruction_ret,
	dmg_processor_instruction_jp,
	dmg_processor_instruction_unused,
	dmg_processor_instruction_call,
	dmg_processor_instruction_call,
	dmg_processor_instruction_adc,
	dmg_processor_instruction_rst,
	dmg_processor_instruction_ret, /* 0xd0 */
	dmg_processor_instruction_pop,
	dmg_processor_instruction_jp,
	dmg_processor_instruction_unused,
	dmg_processor_instruction_call,
	dmg_processor_instruction_push,
	dmg_processor_instruction_sub,
	dmg_processor_instruction_rst,
	dmg_processor_instruction_ret, /* 0xd8 */
	dmg_processor_instruction_reti,
	dmg_processor_instruction_jp,
	dmg_processor_instruction_unused,
	dmg_processor_instruction_call,
	dmg_processor_instruction_sbc,
	dmg_processor_instruction_unused,
	dmg_processor_instruction_rst,
	dmg_processor_instruction_ld, /* 0xe0 */
	dmg_processor_instruction_pop,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_unused,
	dmg_processor_instruction_unused,
	dmg_processor_instruction_push,
	dmg_processor_instruction_and,
	dmg_processor_instruction_rst,
	dmg_processor_instruction_add, /* 0xe8 */
	dmg_processor_instruction_jp,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_unused,
	dmg_processor_instruction_unused,
	dmg_processor_instruction_unused,
	dmg_processor_instruction_xor,
	dmg_processor_instruction_rst,
	dmg_processor_instruction_ld, /* 0xf0 */
	dmg_processor_instruction_pop,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_di,
	dmg_processor_instruction_unused,
	dmg_processor_instruction_push,
	dmg_processor_instruction_or,
	dmg_processor_instruction_rst,
	dmg_processor_instruction_ld, /* 0xf8 */
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ld,
	dmg_processor_instruction_ei,
	dmg_processor_instruction_unused,
	dmg_processor_instruction_unused,
	dmg_processor_instruction_cp,
	dmg_processor_instruction_rst,
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
	value.low_lsb = carry.low_lsb;
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
	value.low_lsb = carry.low_lsb;
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
	value.low_msb = carry.low_msb;
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
	value.low_msb = carry.low_msb;
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

#ifndef NDEBUG

static void
dmg_processor_trace(
	__in int level,
	__inout dmg_processor_t *processor
	)
{
	TRACE_FORMAT(level, "AF=%04x (A=%02x, F=%02x [%c%c%c%c]) ", processor->af.word, processor->af.high, processor->af.low,
		processor->af.flag.carry ? 'C' : '-', processor->af.flag.carry_half ? 'H' : '-',
		processor->af.flag.subtract ? 'N' : '-', processor->af.flag.zero ? 'Z' : '-');
	TRACE_FORMAT(level, "BC=%04x (B=%02x, C=%02x)", processor->bc.word, processor->bc.high, processor->bc.low);
	TRACE_FORMAT(level, "DE=%04x (D=%02x, E=%02x)", processor->de.word, processor->de.high, processor->de.low);
	TRACE_FORMAT(level, "HL=%04x (H=%02x, L=%02x)", processor->hl.word, processor->hl.high, processor->hl.low);
	TRACE_FORMAT(level, "PC=%04x", processor->pc.word);
	TRACE_FORMAT(level, "SP=%04x", processor->sp.word);
	TRACE_FORMAT(level, "IME=%x", processor->interrupts_enable);
	TRACE_FORMAT(level, "IE=%02x [%c%c%c%c%c]", processor->interrupt_enable, processor->interrupt_enable.vblank ? 'V' : '-',
		processor->interrupt_enable.lcdc ? 'L' : '-', processor->interrupt_enable.timer ? 'T' : '-',
		processor->interrupt_enable.serial ? 'S' : '-', processor->interrupt_enable.joypad ? 'J' : '-');
	TRACE_FORMAT(level, "IF=%02x [%c%c%c%c%c]", processor->interrupt_flag, processor->interrupt_flag.vblank ? 'V' : '-',
		processor->interrupt_flag.lcdc ? 'L' : '-', processor->interrupt_flag.timer ? 'T' : '-',
		processor->interrupt_flag.serial ? 'S' : '-', processor->interrupt_flag.joypad ? 'J' : '-');
	TRACE_FORMAT(level, "HALT=%x", processor->halt);
	TRACE_FORMAT(level, "STOP=%x", processor->stop);
}

static void
dmg_processor_instruction_trace(
	__in int level,
	__inout dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in bool extended,
	__in const dmg_register_t *operand
	)
{
	const char *format = (extended ? INSTRUCTION_EXTENDED_STR[instruction->opcode] : INSTRUCTION_STR[instruction->opcode]);

	switch(instruction->operand) {
		case OPERAND_BYTE:

			if(extended) {
				TRACE_FORMAT(level, "[%04x] {%02x %02x %02x}", processor->pc.word - instruction->operand - 1, INSTRUCTION_EXTENDED_PREFIX,
					instruction->opcode, operand->low);
			} else {
				TRACE_FORMAT(level, "[%04x] {%02x %02x}", processor->pc.word - instruction->operand - 1, instruction->opcode,
					operand->low);
			}

			switch(instruction->opcode) {
				case INSTRUCTION_ADD_SP_I8:
				case INSTRUCTION_LD_HL_SP_I8:
					TRACE_FORMAT(level, format, operand->low, operand->low);
					break;
				case INSTRUCTION_JR_C_I8:
				case INSTRUCTION_JR_NC_I8:
				case INSTRUCTION_JR_NZ_I8:
				case INSTRUCTION_JR_I8:
				case INSTRUCTION_JR_Z_I8:
					TRACE_FORMAT(level, format, operand->low, operand->low, processor->pc.word + (int8_t)operand->low);
					break;
				default:
					TRACE_FORMAT(level, format, operand->low);
					break;
			}
			break;
		case OPERAND_WORD:

			if(extended) {
				TRACE_FORMAT(level, "[%04x] {%02x %02x %02x %02x}", processor->pc.word - instruction->operand - 1, INSTRUCTION_EXTENDED_PREFIX,
					instruction->opcode, operand->low, operand->high);
			} else {
				TRACE_FORMAT(level, "[%04x] {%02x %02x %02x}", processor->pc.word - instruction->operand - 1, instruction->opcode,
					operand->low, operand->high);
			}

			TRACE_FORMAT(level, format, operand->word);
			break;
		default:

			if(extended) {
				TRACE_FORMAT(level, "[%04x] {%02x %02x}", processor->pc.word - instruction->operand - 1, INSTRUCTION_EXTENDED_PREFIX,
					instruction->opcode);
			} else {
				TRACE_FORMAT(level, "[%04x] {%02x}", processor->pc.word - instruction->operand - 1, instruction->opcode);
			}

			TRACE(level, format);
			break;
	}
}

#endif /* NDEBUG */

static uint32_t
dmg_processor_execute(
	__inout dmg_processor_t *processor
	)
{
	uint32_t result = 0;

	TRACE_PROCESSOR(LEVEL_VERBOSE, processor);

	if(!processor->halt && !processor->stop) {
		bool extended;
		uint8_t opcode;
		dmg_register_t operand = {};
		const dmg_instruction_cb *handler;
		const dmg_instruction_t *instruction;

		if((extended = ((opcode = dmg_processor_fetch(processor)) == INSTRUCTION_EXTENDED_PREFIX))) {
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

		TRACE_INSTRUCTION(LEVEL_VERBOSE, processor, instruction, extended, &operand);

		result += (*handler)(processor, instruction, &operand);

		switch(processor->interrupts_enable_state) {
			case INTERRUPTS_STATE_SET:
				processor->interrupts_enable_state = INTERRUPTS_STATE_NONE;
				processor->interrupts_enable = !processor->interrupts_enable;

				TRACE_FORMAT(LEVEL_VERBOSE, "Interrupts %s [%04x]", processor->interrupts_enable ? "enabled" : "disabled",
						processor->pc.word);
				break;
			case INTERRUPTS_STATE_PENDING:
				processor->interrupts_enable_state = INTERRUPTS_STATE_SET;

				TRACE_FORMAT(LEVEL_VERBOSE, "Interrupts %s pending [%04x]", processor->interrupts_enable ? "disable" : "enable",
						processor->pc.word);
				break;
			default:
				break;
		}
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
