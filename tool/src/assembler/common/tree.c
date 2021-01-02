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

#include "./tree_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static void
dmg_assembler_tree_child_free(
	__inout dmg_assembler_tree_t *tree
	)
{

	if(tree->child) {
		free(tree->child);
	}

	memset(tree, 0, sizeof(*tree));
}

static int
dmg_assembler_tree_child_allocate(
	__inout dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;

	dmg_assembler_tree_child_free(tree);

	if((tree->child = (const uintptr_t **)calloc(TREE_CHILD_CAPACITY_INIT, sizeof(const uintptr_t **))) == NULL) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to allocate tree child buffer");
		goto exit;
	}

	tree->capacity = TREE_CHILD_CAPACITY_INIT;
	tree->count = 0;

exit:
	return result;
}

static int
dmg_assembler_tree_child_reallocate(
	__inout dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((tree->child = (const uintptr_t **)realloc(tree->child, sizeof(const uintptr_t **) * tree->capacity * TREE_CHILD_CAPACITY_SCALE))
			== NULL) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to reallocate tree child buffer");
		goto exit;
	}

	memset(&(tree->child[tree->capacity]), 0, sizeof(const uintptr_t *) * ((tree->capacity * TREE_CHILD_CAPACITY_SCALE) - tree->capacity));
	tree->capacity *= TREE_CHILD_CAPACITY_SCALE;

exit:
	return result;
}

static int
dmg_assembler_tree_child_resize(
	__inout dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((tree->count + 1) == tree->capacity) {
		result = dmg_assembler_tree_child_reallocate(tree);
	}

	return result;
}

static int
dmg_assembler_trees_reallocate(
	__inout dmg_assembler_trees_t *trees
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((trees->tree = (dmg_assembler_tree_t *)realloc(trees->tree, sizeof(dmg_assembler_tree_t) * trees->capacity * TREE_CAPACITY_SCALE)) == NULL) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to reallocate tree buffer");
		goto exit;
	}

	memset(&(trees->tree[trees->capacity]), 0, sizeof(dmg_assembler_tree_t) * ((trees->capacity * TREE_CAPACITY_SCALE) - trees->capacity));

	for(uint32_t index = trees->capacity; index < (trees->capacity * TREE_CAPACITY_SCALE); ++index) {

		if((result = dmg_assembler_tree_child_allocate(&trees->tree[index])) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	trees->capacity *= TREE_CAPACITY_SCALE;

exit:
	return result;
}

const dmg_assembler_tree_t *
dmg_assembler_tree_child(
	__in const dmg_assembler_tree_t *tree,
	__in uint32_t position
	)
{
	return (const dmg_assembler_tree_t *)tree->child[position];
}

int
dmg_assembler_tree_child_append(
	__inout dmg_assembler_tree_t *tree,
	__in const dmg_assembler_tree_t *child
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(((tree->count + 1) == tree->capacity)
			&& ((result = dmg_assembler_tree_child_resize(tree)) != DMG_STATUS_SUCCESS)) {
		goto exit;
	}

	tree->child[tree->count++] = (const uintptr_t *)child;

exit:
	return result;
}

int
dmg_assembler_trees_allocate(
	__inout dmg_assembler_trees_t *trees
	)
{
	int result = DMG_STATUS_SUCCESS;

	dmg_assembler_trees_free(trees);

	if((trees->tree = (dmg_assembler_tree_t *)calloc(TREE_CAPACITY_INIT, sizeof(dmg_assembler_tree_t))) == NULL) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to allocate tree buffer");
		goto exit;
	}

	trees->capacity = TREE_CAPACITY_INIT;
	trees->count = 0;

	for(uint32_t index = 0; index < trees->capacity; ++index) {

		if((result = dmg_assembler_tree_child_allocate(&trees->tree[index])) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

exit:
	return result;
}

void
dmg_assembler_trees_free(
	__inout dmg_assembler_trees_t *trees
	)
{

	if(trees->tree) {

		for(uint32_t index = 0; index < trees->capacity; ++index) {
			dmg_assembler_tree_child_free(&trees->tree[index]);
		}

		free(trees->tree);
	}

	memset(trees, 0, sizeof(*trees));
}

int
dmg_assembler_trees_resize(
	__inout dmg_assembler_trees_t *trees
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((trees->count + 1) == trees->capacity) {
		result = dmg_assembler_trees_reallocate(trees);
	}

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
