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

#ifndef DMG_TOOL_ASSEMBLER_COMMON_GLOBAL_H_
#define DMG_TOOL_ASSEMBLER_COMMON_GLOBAL_H_

#include "./token.h"

typedef struct {
	const dmg_assembler_token_t *token;
	dmg_assembler_scalar_t value;
	bool in_use;
} dmg_assembler_global_t;

typedef struct {
	dmg_assembler_global_t *global;
	uint32_t capacity;
	uint32_t count;
} dmg_assembler_globals_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_assembler_global_add(
	__inout dmg_assembler_globals_t *globals,
	__in const dmg_assembler_token_t *token,
	__in const dmg_assembler_scalar_t *value,
	__in bool allow_duplicate
	);

bool dmg_assembler_global_defined(
	__inout dmg_assembler_globals_t *globals,
	__in const dmg_assembler_token_t *token
	);

int dmg_assembler_global_get(
	__inout dmg_assembler_globals_t *globals,
	__in const dmg_assembler_token_t *token,
	__inout dmg_assembler_scalar_t *value
	);

int dmg_assembler_global_set(
	__inout dmg_assembler_globals_t *globals,
	__in const dmg_assembler_token_t *token,
	__in const dmg_assembler_scalar_t *value
	);

int dmg_assembler_global_remove(
	__inout dmg_assembler_globals_t *globals,
	__in const dmg_assembler_token_t *token
	);

int dmg_assembler_globals_allocate(
	__inout dmg_assembler_globals_t *globals
	);

void dmg_assembler_globals_free(
	__inout dmg_assembler_globals_t *globals
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TOOL_ASSEMBLER_COMMON_GLOBAL_H_ */
