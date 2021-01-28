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

#ifndef DMG_TOOL_ASSEMBLER_GENERATOR_H_
#define DMG_TOOL_ASSEMBLER_GENERATOR_H_

#include "./parser.h"

typedef struct {
	dmg_assembler_parser_t parser;

	// TODO

} dmg_assembler_generator_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_assembler_generator_load(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_buffer_t *buffer,
	__in const char *path
	);

void dmg_assembler_generator_unload(
	__inout dmg_assembler_generator_t *generator
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TOOL_ASSEMBLER_GENERATOR_H_ */
