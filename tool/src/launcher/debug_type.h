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

#ifndef DMG_TOOL_LAUNCHER_DEBUG_TYPE_H_
#define DMG_TOOL_LAUNCHER_DEBUG_TYPE_H_

#include "../../include/launcher/debug.h"

#define ARGUMENT_MAX 10
#define ARGUMENT_READ 1
#define ARGUMENT_READ_WIDTH 16
#define ARGUMENT_WRITE 2

#define PATH_ROM_INFO "dmg-rom-info -r "
#define PATH_SAVE_INFO "dmg-save-info -s "

#define PROCESSOR_DELIMITER '-'

#define PROMPT_MAX 128
#define PROMPT_PREFIX "\n("
#define PROMPT_POSTFIX ") "

#define REGISTER_DELIMITER "$"

enum {
	DEBUG_EXIT = 0,
	DEBUG_DISASSEMBLE,
	DEBUG_HELP,
	DEBUG_PROCESSOR,
	DEBUG_READ,
	DEBUG_RUN,
	DEBUG_STEP,
	DEBUG_VERSION,
	DEBUG_WRITE,
	DEBUG_MAX,
};

enum {
	DEBUG_READ_ADDRESS = 0,
	DEBUG_READ_OFFSET,
};

enum {
	DEBUG_WRITE_ADDRESS = 0,
	DEBUG_WRITE_VALUE,
	DEBUG_WRITE_OFFSET,
};

static const char DEBUG_CHAR[] = {
	'q', /* DEBUG_EXIT */
	'z', /* DEBUG_DISASSEMBLE */
	'h', /* DEBUG_HELP */
	'p', /* DEBUG_PROCESSOR */
	'r', /* DEBUG_READ */
	'c', /* DEBUG_RUN */
	's', /* DEBUG_STEP */
	'v', /* DEBUG_VERSION */
	'w', /* DEBUG_WRITE */
	'\0', /* DEBUG_MAX */
	};

static const char *DEBUG_DESCRIPTION_STR[] = {
	"Exit debug prompt", /* DEBUG_EXIT */
	"Disassemble instruction(s) at address", /* DEBUG_DISASSEMBLE */
	"Display help information", /* DEBUG_HELP */
	"Display processor information", /* DEBUG_PROCESSOR */
	"Read byte(s) from address/register", /* DEBUG_READ */
	"Run emulator until breakpoint", /* DEBUG_RUN */
	"Step emulator through instructions", /* DEBUG_STEP */
	"Display version information", /* DEBUG_VERSION */
	"Write byte(s) to address/register", /* DEBUG_WRITE */
	"", /* DEBUG_MAX */
	};

enum {
	PROCESSOR_FLAG_UNUSED_0 = 0,
	PROCESSOR_FLAG_UNUSED_1,
	PROCESSOR_FLAG_UNUSED_2,
	PROCESSOR_FLAG_UNUSED_3,
	PROCESSOR_FLAG_CARRY,
	PROCESSOR_FLAG_CARRY_HALF,
	PROCESSOR_FLAG_SUBTRACT,
	PROCESSOR_FLAG_ZERO,
	PROCESSOR_FLAG_MAX,
};

static const char PROCESSOR_FLAG_CHAR[] = {
	'\0', /* PROCESSOR_FLAG_UNUSED_0 */
	'\0', /* PROCESSOR_FLAG_UNUSED_1 */
	'\0', /* PROCESSOR_FLAG_UNUSED_2 */
	'\0', /* PROCESSOR_FLAG_UNUSED_3 */
	'C', /* PROCESSOR_FLAG_CARRY */
	'H', /* PROCESSOR_FLAG_CARRY_HALF */
	'N', /* PROCESSOR_FLAG_SUBTRACT */
	'Z', /* PROCESSOR_FLAG_ZERO */
	'\0', /* PROCESSOR_FLAG_MAX */
	};

enum {
	PROCESSOR_INTERRUPT_VBLANK = 0,
	PROCESSOR_INTERRUPT_LCDC,
	PROCESSOR_INTERRUPT_TIMER,
	PROCESSOR_INTERRUPT_SERIAL,
	PROCESSOR_INTERRUPT_JOYPAD,
	PROCESSOR_INTERRUPT_MAX,
};

static const char PROCESSOR_INTERRUPT_CHAR[] = {
	'V', /* PROCESSOR_INTERRUPT_VBLANK */
	'L', /* PROCESSOR_INTERRUPT_LCDC */
	'T', /* PROCESSOR_INTERRUPT_TIMER */
	'S', /* PROCESSOR_INTERRUPT_SERIAL */
	'J', /* PROCESSOR_INTERRUPT_JOYPAD */
	'\0', /* PROCESSOR_INTERRUPT_MAX */
	};

static const char *REGISTER_STR[] = {
	REGISTER_DELIMITER "a", /* DMG_REGISTER_PROCESSOR_A */
	REGISTER_DELIMITER "af", /* DMG_REGISTER_PROCESSOR_AF */
	REGISTER_DELIMITER "b", /* DMG_REGISTER_PROCESSOR_B */
	REGISTER_DELIMITER "bc", /* DMG_REGISTER_PROCESSOR_BC */
	REGISTER_DELIMITER "c", /* DMG_REGISTER_PROCESSOR_C */
	REGISTER_DELIMITER "d", /* DMG_REGISTER_PROCESSOR_D */
	REGISTER_DELIMITER "de", /* DMG_REGISTER_PROCESSOR_DE */
	REGISTER_DELIMITER "e", /* DMG_REGISTER_PROCESSOR_E */
	REGISTER_DELIMITER "f", /* DMG_REGISTER_PROCESSOR_F */
	REGISTER_DELIMITER "h", /* DMG_REGISTER_PROCESSOR_H */
	REGISTER_DELIMITER "halt", /* DMG_REGISTER_PROCESSOR_HALT */
	REGISTER_DELIMITER "hl", /* DMG_REGISTER_PROCESSOR_HL */
	REGISTER_DELIMITER "ie", /* DMG_REGISTER_PROCESSOR_IE */
	REGISTER_DELIMITER "if", /* DMG_REGISTER_PROCESSOR_IF */
	REGISTER_DELIMITER "ime", /* DMG_REGISTER_PROCESSOR_IME */
	REGISTER_DELIMITER "l", /* DMG_REGISTER_PROCESSOR_L */
	REGISTER_DELIMITER "pc", /* DMG_REGISTER_PROCESSOR_PC */
	REGISTER_DELIMITER "sp", /* DMG_REGISTER_PROCESSOR_SP */
	REGISTER_DELIMITER "stop", /* DMG_REGISTER_PROCESSOR_STOP */
	REGISTER_DELIMITER "", /* DMG_REGISTER_MAX */
	};

typedef int (*dmg_launcher_debug_hdlr)(
	__in const char *argument[],
	__in uint32_t count
	);

#endif /* DMG_TOOL_LAUNCHER_DEBUG_TYPE_H_ */
