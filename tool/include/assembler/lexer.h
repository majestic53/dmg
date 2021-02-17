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

#ifndef DMG_TOOL_ASSEMBLER_LEXER_H_
#define DMG_TOOL_ASSEMBLER_LEXER_H_

#include "./stream.h"

typedef struct {
	dmg_assembler_stream_t stream;
	dmg_assembler_tokens_t tokens;
	uint32_t position;
} dmg_assembler_lexer_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool dmg_assembler_lexer_has_next(
	__in const dmg_assembler_lexer_t *lexer
	);

bool dmg_assembler_lexer_has_previous(
	__in const dmg_assembler_lexer_t *lexer
	);

int dmg_assembler_lexer_load(
	__inout dmg_assembler_lexer_t *lexer,
	__in const dmg_buffer_t *buffer,
	__in const char *path
	);

int dmg_assembler_lexer_next(
	__inout dmg_assembler_lexer_t *lexer
	);

int dmg_assembler_lexer_previous(
	__inout dmg_assembler_lexer_t *lexer
	);

dmg_assembler_token_t *dmg_assembler_lexer_token(
	__in const dmg_assembler_lexer_t *lexer
	);

void dmg_assembler_lexer_unload(
	__inout dmg_assembler_lexer_t *lexer
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TOOL_ASSEMBLER_LEXER_H_ */
