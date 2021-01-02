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

#ifndef DMG_TOOL_ASSEMBLER_LEXER_TYPE_H_
#define DMG_TOOL_ASSEMBLER_LEXER_TYPE_H_


#include "../../include/assembler/common/string.h"
#include "../../include/assembler/lexer.h"

#define ALPHA_LENGTH_MAX 32

#define BASE_BINARY 2
#define BASE_DECIMAL 10
#define BASE_HEXIDECIMAL 16

#define DIRECTIVE_LENGTH_MAX 32

#define COUNT_BINARY_MAX 16
#define COUNT_BINARY_ESCAPE_MAX 8
#define COUNT_DECIMAL_MAX 5
#define COUNT_DECIMAL_ESCAPE_MAX 3
#define COUNT_HEXIDECIMAL_MAX 4
#define COUNT_HEXIDECIMAL_ESCAPE_MAX 2

#endif /* DMG_TOOL_ASSEMBLER_LEXER_TYPE_H_ */
