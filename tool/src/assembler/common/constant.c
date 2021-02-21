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

#include "./constant_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_assembler_constant_error(
	__in const dmg_assembler_token_t *token,
	__in const char *message,
	...
	)
{
	dmg_assembler_string_t string = {};

	if(dmg_assembler_string_allocate(&string) == DMG_STATUS_SUCCESS) {

		if(token->literal.length) {

			for(uint32_t index = 0; index < token->literal.length; ++index) {

				if(dmg_assembler_string_append_character(&string, token->literal.str[index]) != DMG_STATUS_SUCCESS) {
					break;
				}
			}
		} else {
			strcpy(string.str, "EOF");
		}
	}

	ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s \"%s\" (@%u)", message, string.str, token->line);
	dmg_assembler_string_free(&string);

	return DMG_STATUS_FAILURE;
}

static int
dmg_assembler_constant_find(
	__inout dmg_assembler_constants_t *constants,
	__in const dmg_assembler_token_t *token,
	__out dmg_assembler_constant_t **constant
	)
{
	uint32_t index = 0;
	int result = DMG_STATUS_SUCCESS;

	for(; index < constants->count; ++index) {
		*constant = &constants->constant[index];

		if((token->literal.length == (*constant)->token->literal.length)
				&& !strncmp(token->literal.str, (*constant)->token->literal.str, token->literal.length)) {
			break;
		}
	}

	if(index == constants->count) {
		result = CONSTANT_ERROR(token, "Failed to find constant");
		*constant = NULL;
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_constants_reallocate(
	__inout dmg_assembler_constants_t *constants
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!(constants->constant = (dmg_assembler_constant_t *)realloc(constants->constant,
			sizeof(dmg_assembler_constant_t) * constants->capacity * CONSTANT_CAPACITY_SCALE))) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to reallocate constant buffer");
		goto exit;
	}

	memset(&(constants->constant[constants->capacity]), 0,
		sizeof(dmg_assembler_constant_t) * ((constants->capacity * CONSTANT_CAPACITY_SCALE) - constants->capacity));
	constants->capacity *= CONSTANT_CAPACITY_SCALE;

exit:
	return result;
}

static int
dmg_assembler_constants_resize(
	__inout dmg_assembler_constants_t *constants
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((constants->count + 1) == constants->capacity) {
		result = dmg_assembler_constants_reallocate(constants);
	}

	return result;
}

int
dmg_assembler_constant_add(
	__inout dmg_assembler_constants_t *constants,
	__in const dmg_assembler_token_t *token,
	__in const dmg_assembler_scalar_t *value,
	__in bool allow_duplicate
	)
{
	uint32_t index = 0;
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_constant_t *constant = NULL;

	for(; index < constants->count; ++index) {

		if(!(constant = &constants->constant[index])->in_use) {
			break;
		}
	}

	if(!allow_duplicate && (dmg_assembler_constant_find(constants, token, &constant) == DMG_STATUS_SUCCESS)) {
		result = CONSTANT_ERROR(token, "Redefined constant");
		goto exit;
	}

	if(index == constants->count) {
		constant = &constants->constant[constants->count++];

		if((result = dmg_assembler_constants_resize(constants)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	constant->token = token;
	constant->value.word = value->word;
	constant->in_use = true;

exit:
	return result;
}

bool
dmg_assembler_constant_defined(
	__inout dmg_assembler_constants_t *constants,
	__in const dmg_assembler_token_t *token
	)
{
	uint32_t index = 0;

	for(; index < constants->count; ++index) {
		dmg_assembler_constant_t *constant = &constants->constant[index];

		if((token->literal.length == constant->token->literal.length)
				&& !strncmp(token->literal.str, constant->token->literal.str, token->literal.length)) {
			break;
		}
	}

	return (index < constants->count);
}

int
dmg_assembler_constant_get(
	__inout dmg_assembler_constants_t *constants,
	__in const dmg_assembler_token_t *token,
	__inout dmg_assembler_scalar_t *value
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_constant_t *constant = NULL;

	if((result = dmg_assembler_constant_find(constants, token, &constant)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	value->word = constant->value.word;

exit:
	return result;
}

int
dmg_assembler_constant_set(
	__inout dmg_assembler_constants_t *constants,
	__in const dmg_assembler_token_t *token,
	__in const dmg_assembler_scalar_t *value
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_constant_t *constant = NULL;

	if((result = dmg_assembler_constant_find(constants, token, &constant)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	constant->value.word = value->word;

exit:
	return result;
}

int
dmg_assembler_constant_remove(
	__inout dmg_assembler_constants_t *constants,
	__in const dmg_assembler_token_t *token
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_constant_t *constant = NULL;

	if(dmg_assembler_constant_find(constants, token, &constant) != DMG_STATUS_SUCCESS) {
		result = CONSTANT_ERROR(token, "Undefined constant");
		goto exit;
	}

	constant->token = NULL;
	constant->value.word = 0;
	constant->in_use = false;

exit:
	return result;
}

int
dmg_assembler_constants_allocate(
	__inout dmg_assembler_constants_t *constants
	)
{
	int result = DMG_STATUS_SUCCESS;

	dmg_assembler_constants_free(constants);

	if(!(constants->constant = (dmg_assembler_constant_t *)calloc(CONSTANT_CAPACITY_INIT, sizeof(dmg_assembler_constant_t)))) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to allocate constant buffer");
		goto exit;
	}

	constants->capacity = CONSTANT_CAPACITY_INIT;
	constants->count = 0;

exit:
	return result;
}

void
dmg_assembler_constants_free(
	__inout dmg_assembler_constants_t *constants
	)
{

	if(constants->constant) {
		free(constants->constant);
	}

	memset(constants, 0, sizeof(*constants));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
