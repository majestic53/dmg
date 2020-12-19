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

#ifndef DMG_TOOL_COMMON_H_
#define DMG_TOOL_COMMON_H_

#include <readline/history.h>
#include <readline/readline.h>
#include <SDL2/SDL.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/common/define.h"
#include "../../include/dmg.h"
#include "./syntax.h"
#include "./trace.h"

#ifndef __in
#define __in
#endif /* __in */
#ifndef __inout
#define __inout
#endif /* __inout */
#ifndef __out
#define __out
#endif /* __out */

#define KBYTE 1024

#define CHARACTER_FILL '.'

#define COMMENT_PREFIX ';'

#define DIRECTIVE_DELIMITER "."

#define LABEL_PREFIX ":"

#define PATH_DELIMITER '/'
#define PATH_MAX 1024

#define BANK_WIDTH ADDRESS_WIDTH(ADDRESS_ROM_BEGIN, ADDRESS_ROM_END)

enum {
	DIRECTIVE_BANK = 0,
	DIRECTIVE_DATA,
	DIRECTIVE_ORIGIN,
	DIRECTIVE_MAX,
};

enum {
	VECTOR_RST_00 = 0,
	VECTOR_RST_08,
	VECTOR_RST_10,
	VECTOR_RST_18,
	VECTOR_RST_20,
	VECTOR_RST_28,
	VECTOR_RST_30,
	VECTOR_RST_38,
	VECTOR_VBLANK,
	VECTOR_LCDC,
	VECTOR_TIMER,
	VECTOR_SERIAL,
	VECTOR_JOYPAD,
	VECTOR_MAX,
};

#define TRACE_TOOL(_STREAM_, _LEVEL_, _FORMAT_, ...) \
	dmg_tool_trace(_STREAM_, _LEVEL_, _FORMAT_, __VA_ARGS__)
#define TRACE_TOOL_ERROR(_FORMAT_, ...) \
	TRACE_TOOL(stderr, LEVEL_ERROR, _FORMAT_, __VA_ARGS__)
#define TRACE_TOOL_MESSAGE(_FORMAT_, ...) \
	TRACE_TOOL(stdout, LEVEL_NONE, _FORMAT_, __VA_ARGS__)

#endif /* DMG_TOOL_COMMON_H_ */
