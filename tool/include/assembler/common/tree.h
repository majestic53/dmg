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

#include "./token.h"

#define TREE_CHILD_MAX 32

typedef struct {
	const dmg_assembler_token_t *parent;
	const uintptr_t *child[TREE_CHILD_MAX];
	uint32_t capacity;
	uint32_t count;
} dmg_assembler_tree_t;

typedef struct {

	struct {
		dmg_assembler_tree_t *ptr;
		uint32_t capacity;
		uint32_t count;
	} tree;

	const dmg_assembler_tree_t *root;
} dmg_assembler_trees_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_assembler_trees_add(
	__inout dmg_assembler_trees_t *trees,
	__in bool root,
	__in const dmg_assembler_token_t *token,
	__out dmg_assembler_tree_t **tree
	);

int dmg_assembler_trees_append_child_token(
	__inout dmg_assembler_trees_t *trees,
	__inout dmg_assembler_tree_t *parent,
	__in const dmg_assembler_token_t *token,
	__out dmg_assembler_tree_t **tree
	);

int dmg_assembler_trees_append_child_tree(
	__inout dmg_assembler_tree_t *parent,
	__in const dmg_assembler_tree_t *child
	);

int dmg_assembler_trees_allocate(
	__inout dmg_assembler_trees_t *trees
	);

void dmg_assembler_trees_free(
	__inout dmg_assembler_trees_t *trees
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TOOL_ASSEMBLER_COMMON_TREE_H_ */
