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

#ifndef DMG_SYSTEM_PROCESSOR_TYPE_H_
#define DMG_SYSTEM_PROCESSOR_TYPE_H_

#include "../../include/system/processor.h"
#include "../../include/runtime.h"

#define CYCLE_IDLE CYCLE
#define CYCLE_INTERRUPT (5 * CYCLE)
#define CYCLE_INTERRUPT_HALT CYCLE

#define INTERRUPT_FLAG_MASK 0x1f

static uint16_t INTERRUPT_ADDR[] = {
	0x0040, /* INTERRUPT_VBLANK */
	0x0048, /* INTERRUPT_LCDC */
	0x0050, /* INTERRUPT_TIMER */
	0x0058, /* INTERRUPT_SERIAL */
	0x0060, /* INTERRUPT_JOYPAD */
	0x0000, /* INTERRUPT_MAX */
	};

#define POST_AF 0x01b0
#define POST_BC 0x0013
#define POST_DE 0x00d8
#define POST_HL 0x014d
#define POST_IF 0xe0
#define POST_PC 0x0100
#define POST_SP 0xfffe

enum {
	OPERAND_NONE = 0,
	OPERAND_BYTE,
	OPERAND_WORD,
	OPERAND_MAX,
};

typedef struct {
	uint8_t opcode;
	uint8_t operand;
	uint8_t cycle;
	uint8_t cycle_taken;
} dmg_instruction_t;

enum {
	INSTRUCTION_NOP = 0,

	// TODO: ADD ADDITIONAL INSTRUCTIONS

	INSTRUCTION_EXTENDED_PREFIX = 0xcb,

	// TODO: ADD ADDITIONAL INSTRUCTIONS

	INSTRUCTION_MAX,
};

static const dmg_instruction_t INSTRUCTION[] = {
	{ INSTRUCTION_NOP, OPERAND_NONE, CYCLE, 0 }, /* INSTRUCTION_NOP */

	// TODO: ADD ADDITIONAL INSTRUCTIONS

	};

enum {
	INSTRUCTION_EXTENDED_RLC_B = 0,

	// TODO: ADD ADDITIONAL EXTENDED INSTRUCTIONS

	INSTRUCTION_EXTENDED_MAX,
};

static const dmg_instruction_t INSTRUCTION_EXTENDED[] = {
	{ INSTRUCTION_EXTENDED_RLC_B, OPERAND_NONE, CYCLE * 2, 0 }, /* INSTRUCTION_EXTENDED_RLC_B */

	// TODO: ADD ADDITIONAL EXTENDED INSTRUCTIONS

	};

typedef uint32_t (*dmg_instruction_cb)(
	__in dmg_processor_t *processor,
	__in const dmg_instruction_t *instruction,
	__in const dmg_register_t *operand
	);

#endif /* DMG_SYSTEM_PROCESSOR_TYPE_H_ */
