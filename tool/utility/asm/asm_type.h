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

#ifndef DMG_TOOL_UTILITY_ASM_H_
#define DMG_TOOL_UTILITY_ASM_H_

#include "../../include/assembler/parser.h"

//#define ASM_PARSE_CHARACTERS
//#define ASM_PARSE_TOKENS
#define ASM_PARSE_TREES

#define DMG_USAGE "dmg-asm [args]"

#define OPTION_HELP 'h'
#define OPTION_OUTPUT 'o'
#define OPTION_SOURCE 's'
#define OPTION_VERSION 'v'
#define OPTIONS "ho:s:v"

#define PATH_OUTPUT "output.gb"

enum {
	FLAG_HELP = 0,
	FLAG_OUTPUT,
	FLAG_SOURCE,
	FLAG_VERSION,
	FLAG_MAX,
};

static const char *FLAG_STR[] = {
	"-h", /* FLAG_HELP */
	"-o", /* FLAG_OUTPUT */
	"-s", /* FLAG_SOURCE */
	"-v", /* FLAG_VERSION */
	"", /* FLAG_MAX */
	};

static const char *FLAG_DESCRIPTION_STR[] = {
	"Display help information", /* FLAG_HELP */
	"Specify output file", /* FLAG_OUTPUT */
	"Specify source file", /* FLAG_SOURCE */
	"Display version information", /* FLAG_VERSION */
	"", /* FLAG_MAX */
	};

typedef struct {
	const char *output;
	const char *source;
	dmg_buffer_t buffer;
	FILE *file;
	bool help;
	bool version;
} dmg_asm_t;

#endif /* DMG_TOOL_UTILITY_ASM_H_ */
