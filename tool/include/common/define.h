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

#ifndef DMG_TOOL_COMMON_DEFINE_H_
#define DMG_TOOL_COMMON_DEFINE_H_

#include "../../../include/common/define.h"

#ifndef __in
#define __in
#endif /* __in */
#ifndef __inout
#define __inout
#endif /* __inout */
#ifndef __out
#define __out
#endif /* __out */

#define BANK_WIDTH ADDRESS_WIDTH(ADDRESS_ROM_BEGIN, ADDRESS_ROM_END)

#define CGB_SUPPORT 0x80
#define CGB_SUPPORT_ONLY 0xc0

#define CHARACTER_BINARY_MAX '1'
#define CHARACTER_BINARY_MIN '0'
#define CHARACTER_EOF '\0'
#define CHARACTER_FILL '.'
#define CHARACTER_NEWLINE '\n'

#define DELIMITER_BINARY "@"
#define DELIMITER_COMMENT ';'
#define DELIMITER_DIRECTIVE "."
#define DELIMITER_HEXIDECIMAL "$"
#define DELIMITER_IDENTIFIER "_"
#define DELIMITER_LABEL ":"
#define DELIMITER_LITERAL_CHARACTER "\'"
#define DELIMITER_LITERAL_ESCAPE "\\"
#define DELIMITER_LITERAL_STRING "\""

#define KBYTE 1024

#define PATH_DELIMITER '/'
#define PATH_MAX 1024

#define SGB_SUPPORT 0x03

#define TRACE_TOOL(_STREAM_, _LEVEL_, _FORMAT_, ...) \
	dmg_tool_trace(_STREAM_, _LEVEL_, _FORMAT_, __VA_ARGS__)
#define TRACE_TOOL_ERROR(_FORMAT_, ...) \
	TRACE_TOOL(stderr, LEVEL_ERROR, _FORMAT_, __VA_ARGS__)
#define TRACE_TOOL_MESSAGE(_FORMAT_, ...) \
	TRACE_TOOL(stdout, LEVEL_NONE, _FORMAT_, __VA_ARGS__)
#define TRACE_TOOL_WARNING(_FORMAT_, ...) \
	TRACE_TOOL(stderr, LEVEL_WARNING, _FORMAT_, __VA_ARGS__)

#endif /* DMG_TOOL_COMMON_DEFINE_H_ */
