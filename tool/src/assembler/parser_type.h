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

#ifndef DMG_TOOL_ASSEMBLER_PARSER_TYPE_H_
#define DMG_TOOL_ASSEMBLER_PARSER_TYPE_H_

#include "../../include/assembler/parser.h"

#define PARSER_ERROR(_PARSER_, _TOKEN_, _MESSAGE_) \
	dmg_assembler_parser_error(_PARSER_, _TOKEN_, _MESSAGE_)

typedef int (*dmg_assembler_parser_hdlr)(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	);

#endif /* DMG_TOOL_ASSEMBLER_PARSER_TYPE_H_ */
