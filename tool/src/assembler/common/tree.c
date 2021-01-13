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

static int
dmg_assembler_tree_allocate(
	__inout dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;

	memset(tree, 0, sizeof(dmg_assembler_tree_t));
	tree->capacity = TREE_CHILD_MAX;

	return result;
}

static void
dmg_assembler_tree_free(
	__inout dmg_assembler_tree_t *tree
	)
{
	memset(tree, 0, sizeof(*tree));
}

static int
dmg_assembler_tree_resize(
	__inout dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(tree->count >= tree->capacity) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Exceeded maximum child capacity");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_trees_reallocate_entry(
	__inout dmg_assembler_trees_t *trees
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!(trees->entry = (const dmg_assembler_tree_t **)realloc(trees->entry, sizeof(const dmg_assembler_tree_t *) * trees->capacity
			* TREE_ENTRY_CAPACITY_SCALE))) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to reallocate tree entry buffer");
		goto exit;
	}

	memset(&trees->entry[trees->capacity], 0, sizeof(const dmg_assembler_tree_t *) * ((trees->capacity * TREE_ENTRY_CAPACITY_SCALE) - trees->capacity));
	trees->capacity *= TREE_ENTRY_CAPACITY_SCALE;

exit:
	return result;
}

static int
dmg_assembler_trees_reallocate_tree(
	__inout dmg_assembler_trees_t *trees
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!(trees->tree.ptr = (dmg_assembler_tree_t *)realloc(trees->tree.ptr, sizeof(dmg_assembler_tree_t) * trees->tree.capacity * TREE_CAPACITY_SCALE))) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to reallocate tree buffer");
		goto exit;
	}

	memset(&trees->tree.ptr[trees->tree.capacity], 0, sizeof(dmg_assembler_tree_t) * ((trees->tree.capacity * TREE_CAPACITY_SCALE) - trees->tree.capacity));
	trees->tree.capacity *= TREE_CAPACITY_SCALE;

exit:
	return result;
}

static int
dmg_assembler_trees_resize_entry(
	__inout dmg_assembler_trees_t *trees
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((trees->count + 1) == trees->capacity) {
		result = dmg_assembler_trees_reallocate_entry(trees);
	}

	return result;
}

static int
dmg_assembler_trees_resize_tree(
	__inout dmg_assembler_trees_t *trees
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((trees->tree.count + 1) == trees->tree.capacity) {
		result = dmg_assembler_trees_reallocate_tree(trees);
	}

	return result;
}

int
dmg_assembler_trees_add(
	__inout dmg_assembler_trees_t *trees,
	__in const dmg_assembler_token_t *token,
	__out dmg_assembler_tree_t **tree
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((result = dmg_assembler_tree_allocate(&trees->tree.ptr[trees->tree.count])) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_trees_resize_tree(trees)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	*tree = &trees->tree.ptr[trees->tree.count];
	(*tree)->parent = token;
	++trees->tree.count;

	if((result = dmg_assembler_trees_resize_entry(trees)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	trees->entry[trees->count] = *tree;
	++trees->count;

exit:
	return result;
}

int
dmg_assembler_trees_add_child(
	__inout dmg_assembler_trees_t *trees,
	__inout dmg_assembler_tree_t *parent,
	__in const dmg_assembler_token_t *token,
	__out dmg_assembler_tree_t **tree
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((result = dmg_assembler_tree_allocate(&trees->tree.ptr[trees->tree.count])) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_trees_resize_tree(trees)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	*tree = &trees->tree.ptr[trees->tree.count];
	(*tree)->parent = token;
	++trees->tree.count;

	if((result = dmg_assembler_tree_resize(parent)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	parent->child[parent->count] = (const uintptr_t *)*tree;
	++parent->count;

exit:
	return result;
}

int
dmg_assembler_trees_allocate(
	__inout dmg_assembler_trees_t *trees
	)
{
	int result = DMG_STATUS_SUCCESS;

	memset(trees, 0, sizeof(dmg_assembler_trees_t));

	if(!(trees->tree.ptr = (dmg_assembler_tree_t *)calloc(TREE_CAPACITY, sizeof(dmg_assembler_tree_t)))) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to allocate tree buffer");
		goto exit;
	}

	trees->tree.capacity = TREE_CAPACITY;

	if(!(trees->entry = (const dmg_assembler_tree_t **)calloc(TREE_ENTRY_CAPACITY, sizeof(const dmg_assembler_tree_t *)))) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to allocate tree entry buffer");
		goto exit;
	}

	trees->capacity = TREE_ENTRY_CAPACITY;

exit:
	return result;
}

void
dmg_assembler_trees_free(
	__inout dmg_assembler_trees_t *trees
	)
{

	if(trees->entry) {
		free(trees->entry);
	}

	if(trees->tree.ptr) {

		for(uint32_t index = 0; index < trees->tree.count; ++index) {
			dmg_assembler_tree_free(&trees->tree.ptr[index]);
		}

		free(trees->tree.ptr);
	}

	memset(trees, 0, sizeof(*trees));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
