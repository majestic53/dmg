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

#ifndef DMG_SYSTEM_PROCESSOR_TYPE_H_
#define DMG_SYSTEM_PROCESSOR_TYPE_H_

#include "../../include/system/processor/instruction.h"
#include "../../include/system/processor.h"
#include "../../include/runtime.h"
#include "../../include/service.h"

#define CYCLE_IDLE CYCLE
#define CYCLE_INTERRUPT (5 * CYCLE)
#define CYCLE_INTERRUPT_HALT CYCLE

#define DAA_MAX_HIGH 0x99
#define DAA_MAX_LOW 0x09
#define DAA_OFFSET_HIGH 0x60
#define DAA_OFFSET_LOW 0x06

#define FLAG_MASK 0xf0

#define INTERRUPT_FLAG_MASK 0x1f

enum {
	INTERRUPTS_STATE_NONE = 0,
	INTERRUPTS_STATE_SET,
	INTERRUPTS_STATE_PENDING,
	INTERRUPTS_STATE_MAX,
};

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

typedef uint32_t (*dmg_processor_instruction_hdlr)(
	__in dmg_processor_t *processor,
	__in const dmg_processor_instruction_t *instruction,
	__in const dmg_processor_register_t *operand
	);

#ifndef NDEBUG
#ifndef UNITTEST
#define TRACE_PROCESSOR_INSTRUCTION(_LEVEL_, _PROCESSOR_, _INSTRUCTION_, _EXTENDED_, _OPERAND_) \
	if((_LEVEL_) <= (LEVEL)) { \
		dmg_processor_trace_instruction(_LEVEL_, _PROCESSOR_, _INSTRUCTION_, _EXTENDED_, _OPERAND_); \
	}
#else
#define TRACE_PROCESSOR_INSTRUCTION(_LEVEL_, _PROCESSOR_, _INSTRUCTION_, _EXTENDED_, _OPERAND_)
#endif /* UNITTEST */

#define TRACE_PROCESSOR(_LEVEL_, _PROCESSOR_) \
	if((_LEVEL_) <= (LEVEL)) { \
		dmg_processor_trace(_LEVEL_, _PROCESSOR_); \
	}

#else
#define TRACE_PROCESSOR(_LEVEL_, _PROCESSOR_)
#define TRACE_PROCESSOR_INSTRUCTION(_LEVEL_, _PROCESSOR_, _INSTRUCTION_, _EXTENDED_, _OPERAND_)
#endif /* NDEBUG */

#endif /* DMG_SYSTEM_PROCESSOR_TYPE_H_ */
