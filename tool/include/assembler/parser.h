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

#ifndef DMG_TOOL_ASSEMBLER_PARSER_H_
#define DMG_TOOL_ASSEMBLER_PARSER_H_

#include "./lexer.h"

typedef struct {
	dmg_assembler_lexer_t lexer;
	dmg_assembler_trees_t trees;
	uint32_t count;
	uint32_t position;
} dmg_assembler_parser_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool dmg_assembler_parser_has_next(
	__in const dmg_assembler_parser_t *parser
	);

bool dmg_assembler_parser_has_previous(
	__in const dmg_assembler_parser_t *parser
	);

int dmg_assembler_parser_load(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_buffer_t *buffer,
	__in const char *path
	);

int dmg_assembler_parser_next(
	__inout dmg_assembler_parser_t *parser
	);

int dmg_assembler_parser_previous(
	__inout dmg_assembler_parser_t *parser
	);

const dmg_assembler_tree_t *dmg_assembler_parser_tree(
	__in const dmg_assembler_parser_t *parser
	);

void dmg_assembler_parser_unload(
	__inout dmg_assembler_parser_t *parser
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TOOL_ASSEMBLER_PARSER_H_ */
