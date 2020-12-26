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

#ifndef DMG_TOOL_ASSEMBLER_LEXER_H_
#define DMG_TOOL_ASSEMBLER_LEXER_H_

#include "./stream.h"

enum {
	TOKEN_END = 0,
	TOKEN_DIRECTIVE,
	TOKEN_IDENTIFIER,
	TOKEN_LABEL,
	TOKEN_LITERAL,
	TOKEN_MACRO,
	TOKEN_OPCODE,
	TOKEN_OPERATOR,
	TOKEN_REGISTER,
	TOKEN_SCALAR,
	TOKEN_SYMBOL,
	TOKEN_MAX,
};

typedef struct {
	int type;
	int subtype;
	uint32_t line;

	union {
		struct {
			const char *str;
			uint32_t length;
		} literal;

		union {

			struct {
				uint8_t low;
				uint8_t high;
				uint16_t unused;
			};

			int8_t offset;
			uint16_t word;
			int32_t raw;
		} scalar;
	};
} dmg_assembler_token_t;

typedef struct {
	dmg_assembler_stream_t stream;
	dmg_assembler_token_t *token;
	uint32_t capacity;
	uint32_t count;
	uint32_t position;
} dmg_assembler_lexer_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_assembler_lexer_load(
	__inout dmg_assembler_lexer_t *lexer,
	__in const dmg_buffer_t *buffer,
	__in const char *path
	);

bool dmg_assembler_lexer_has_next(
	__in const dmg_assembler_lexer_t *lexer
	);

bool dmg_assembler_lexer_has_previous(
	__in const dmg_assembler_lexer_t *lexer
	);

int dmg_assembler_lexer_next(
	__inout dmg_assembler_lexer_t *lexer
	);

int dmg_assembler_lexer_previous(
	__inout dmg_assembler_lexer_t *lexer
	);

const dmg_assembler_token_t *dmg_assembler_lexer_token(
	__in const dmg_assembler_lexer_t *lexer
	);

void dmg_assembler_lexer_unload(
	__inout dmg_assembler_lexer_t *lexer
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TOOL_ASSEMBLER_LEXER_H_ */
