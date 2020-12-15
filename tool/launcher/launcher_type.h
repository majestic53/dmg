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

#ifndef DMG_TOOL_LAUNCHER_TYPE_H_
#define DMG_TOOL_LAUNCHER_TYPE_H_

#include "../include/common.h"

#define ARGUMENT_MAX 10
#define ARGUMENT_READ 1
#define ARGUMENT_READ_WIDTH 16
#define ARGUMENT_WRITE 2

#define PATH_ROM_INFO "dmg-rom-info -r "
#define PATH_SAVE_INFO "dmg-save-info -s "

#define DATA_MASK 1
#define DATA_SHIFT 1

#define DEFAULT_PALETTE PALETTE_GREEN
#define DEFAULT_SCALE 2
#define DEFAULT_OUT NULL

#define DMG "DMG"
#define DMG_NOTICE "Copyright (C) 2020 David Jolly"
#define DMG_USAGE "dmg [args]"

#define HISTORY_MAX 10

#define OPTION_BOOTROM 'b'
#define OPTION_CAPTURE 'c'
#define OPTION_DEBUG 'd'
#define OPTION_HELP 'h'
#define OPTION_INPUT 'i'
#define OPTION_OUTPUT 'o'
#define OPTION_PALETTE 'p'
#define OPTION_ROM 'r'
#define OPTION_SCALE 's'
#define OPTION_VERSION 'v'
#define OPTIONS "b:cdhi:o:p:r:s:v"

#define PROMPT_MAX 128
#define PROMPT_PREFIX "\n("
#define PROMPT_POSTFIX ") "

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
	FLAG_BOOTROM = 0,
	FLAG_CAPTURE,
	FLAG_DEBUG,
	FLAG_HELP,
	FLAG_INPUT,
	FLAG_OUTPUT,
	FLAG_PALETTE,
	FLAG_ROM,
	FLAG_SCALE,
	FLAG_VERSION,
	FLAG_MAX,
};

static const char *FLAG_STR[] = {
	"-b", /* FLAG_BOOTROM */
	"-c", /* FLAG_CAPTURE */
	"-d", /* FLAG_DEBUG */
	"-h", /* FLAG_HELP */
	"-i", /* FLAG_INPUT */
	"-o", /* FLAG_OUTPUT */
	"-p", /* FLAG_PALETTE */
	"-r", /* FLAG_ROM */
	"-s", /* FLAG_SCALE */
	"-v", /* FLAG_VERSION */
	"", /* FLAG_MAX */
	};

static const char *FLAG_DESCRIPTION_STR[] = {
	"Specify bootrom binary", /* FLAG_BOOTROM */
	"Enable serial capture", /* FLAG_CAPTURE */
	"Enable debug prompt", /* FLAG_DEBUG */
	"Display help information", /* FLAG_HELP */
	"Specify input save file path", /* FLAG_INPUT */
	"Specify output save file path", /* FLAG_OUTPUT */
	"Specify color palette", /* FLAG_PALETTE */
	"Specify rom binary", /* FLAG_ROM */
	"Specify display scale", /* FLAG_SCALE */
	"Display version information", /* FLAG_VERSION */
	"", /* FLAG_MAX */
	};

#define PROCESSOR_DELIMITER '-'

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

#define REGISTER_DELIMITER "$"

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

typedef struct {
	uint8_t data;
	uint8_t length;
} dmg_launcher_capture_t;

typedef struct {
	dmg_t configuration;
	const char *bootrom;
	const char *rom;
	dmg_launcher_capture_t capture;
	bool debug;
	bool help;
	long palette;
	bool version;
} dmg_launcher_t;

static const uint32_t BUTTON[] = {
	SDL_SCANCODE_X, /* DMG_BUTTON_A */
	SDL_SCANCODE_Z, /* DMG_BUTTON_B */
	SDL_SCANCODE_C, /* DMG_BUTTON_SELECT */
	SDL_SCANCODE_SPACE, /* DMG_BUTTON_START */
	};

static const uint32_t DIRECTION[] = {
	SDL_SCANCODE_RIGHT, /* DMG_DIRECTION_RIGHT */
	SDL_SCANCODE_LEFT, /* DMG_DIRECTION_LEFT */
	SDL_SCANCODE_UP, /* DMG_DIRECTION_UP */
	SDL_SCANCODE_DOWN, /* DMG_DIRECTION_DOWN */
	};

enum {
	PALETTE_GREY = 0,
	PALETTE_GREEN,
	PALETTE_GREEN_OLIVE,
	PALETTE_GREEN_LCD,
	PALETTE_TEAL,
	PALETTE_BLUE,
	PALETTE_PURPLE,
	PALETTE_RED,
	PALETTE_MAX,
};

static const uint32_t PALETTE[][DMG_PALETTE_MAX] = {
		{ /* PALETTE_GREY */
			0xffffff, /* DMG_PALETTE_WHITE */
			0x949494, /* DMG_PALETTE_GREY_LIGHT */
			0x525252, /* DMG_PALETTE_GREY_DARK */
			0x000000, /* DMG_PALETTE_BLACK */
		},
		{ /* PALETTE_GREEN */
			0xe0f8d0, /* DMG_PALETTE_WHITE */
			0x88c070, /* DMG_PALETTE_GREY_LIGHT */
			0x306850, /* DMG_PALETTE_GREY_DARK */
			0x081820, /* DMG_PALETTE_BLACK */
		},
		{ /* PALETTE_GREEN_OLIVE */
			0xb7c166, /* DMG_PALETTE_WHITE */
			0x7b8a32, /* DMG_PALETTE_GREY_LIGHT */
			0x43591d, /* DMG_PALETTE_GREY_DARK */
			0x132c13, /* DMG_PALETTE_BLACK */
		},
		{ /* PALETTE_GREEN_LCD */
			0x9bbb0e, /* DMG_PALETTE_WHITE */
			0x73a067, /* DMG_PALETTE_GREY_LIGHT */
			0x356237, /* DMG_PALETTE_GREY_DARK */
			0x0f380e, /* DMG_PALETTE_BLACK */
		},
		{ /* PALETTE_TEAL */
			0xe2f3e4, /* DMG_PALETTE_WHITE */
			0x94e344, /* DMG_PALETTE_GREY_LIGHT */
			0x46878f, /* DMG_PALETTE_GREY_DARK */
			0x332c50, /* DMG_PALETTE_BLACK */
		},
		{ /* PALETTE_BLUE */
			0x8be5ff, /* DMG_PALETTE_WHITE */
			0x608fcf, /* DMG_PALETTE_GREY_LIGHT */
			0x7550e8, /* DMG_PALETTE_GREY_DARK */
			0x622e4c, /* DMG_PALETTE_BLACK */
		},
		{ /* PALETTE_PURPLE */
			0xf8e3c4, /* DMG_PALETTE_WHITE */
			0xcc3495, /* DMG_PALETTE_GREY_LIGHT */
			0x6b1fb0, /* DMG_PALETTE_GREY_DARK */
			0x0b0630, /* DMG_PALETTE_BLACK */
		},
		{ /* PALETTE_RED */
			0xeff9d6, /* DMG_PALETTE_WHITE */
			0xba5044, /* DMG_PALETTE_GREY_LIGHT */
			0x7a1c4b, /* DMG_PALETTE_GREY_DARK */
			0x1b0326, /* DMG_PALETTE_BLACK */
		},
	};

#endif /* DMG_TOOL_LAUNCHER_TYPE_H_ */
