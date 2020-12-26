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

#ifndef DMG_TOOL_ASSEMBLER_STREAM_H_
#define DMG_TOOL_ASSEMBLER_STREAM_H_

#include "../common.h"

#define CHARACTER_END 0
#define CHARACTER_ALPHA 1
#define CHARACTER_DECIMAL 2
#define CHARACTER_HEXIDECIMAL 4
#define CHARACTER_SPACE 8
#define CHARACTER_SYMBOL 16

typedef struct {
	const dmg_buffer_t *buffer;
	const char *path;
	uint32_t line;
	uint32_t position;
} dmg_assembler_stream_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_assembler_stream_load(
	__inout dmg_assembler_stream_t *stream,
	__in const dmg_buffer_t *buffer,
	__in const char *path
	);

char dmg_assembler_stream_character(
	__in const dmg_assembler_stream_t *stream,
	__inout int *type
	);

const char *dmg_assembler_stream_character_str(
	__in const dmg_assembler_stream_t *stream
	);

bool dmg_assembler_stream_has_next(
	__in const dmg_assembler_stream_t *stream
	);

bool dmg_assembler_stream_has_previous(
	__in const dmg_assembler_stream_t *stream
	);

int dmg_assembler_stream_next(
	__inout dmg_assembler_stream_t *stream
	);

int dmg_assembler_stream_previous(
	__inout dmg_assembler_stream_t *stream
	);

void dmg_assembler_stream_unload(
	__inout dmg_assembler_stream_t *stream
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TOOL_ASSEMBLER_STREAM_H_ */
