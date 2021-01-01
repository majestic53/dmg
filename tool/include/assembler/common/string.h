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

#ifndef DMG_TOOL_ASSEMBLER_COMMON_STRING_H_
#define DMG_TOOL_ASSEMBLER_COMMON_STRING_H_

#include "../../common.h"

typedef struct {
	char *str;
	uint32_t capacity;
	uint32_t length;
} dmg_assembler_string_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_assembler_string_allocate(
	__inout dmg_assembler_string_t *string
	);

int dmg_assembler_string_append(
	__inout dmg_assembler_string_t *string,
	__in char value
	);

void dmg_assembler_string_free(
	__inout dmg_assembler_string_t *string
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TOOL_ASSEMBLER_COMMON_STRING_H_ */
