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

#ifndef DMG_TOOL_ASSEMBLER_COMMON_TREE_H_
#define DMG_TOOL_ASSEMBLER_COMMON_TREE_H_

#include "../../common.h"

typedef struct {

	// TODO

} dmg_assembler_tree_t;

typedef struct {
	dmg_assembler_tree_t *tree;
	uint32_t capacity;
	uint32_t count;
} dmg_assembler_trees_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// TODO

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TOOL_ASSEMBLER_COMMON_TREE_H_ */
