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
#include "../../include/system/processor/instruction.h"
#include "../../include/common/define.h"
#include "../../include/dmg.h"

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

#define PATH_DELIMITER '/'
#define PATH_MAX 1024

#ifdef COLOR

static const char *LEVEL_STR[] = {
	"\x1b[0m", /* LEVEL_NONE */
	"\x1b[91m", /* LEVEL_ERROR */
	"\x1b[93m", /* LEVEL_WARNING */
	"\x1b[94m", /* LEVEL_INFORMATION */
	"\x1b[90m", /* LEVEL_VERBOSE */
	"", /* LEVEL_MAX */
	};

#define LEVEL_COLOR(_STREAM_, _LEVEL_) \
	fprintf(_STREAM_, "%s", LEVEL_STR[_LEVEL_])
#else
#define LEVEL_COLOR(_STREAM_, _LEVEL_)
#endif /* COLOR */

#endif /* DMG_TOOL_COMMON_H_ */
