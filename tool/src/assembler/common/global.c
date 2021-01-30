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

#include "./global_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_assembler_global_error(
	__in const dmg_assembler_token_t *token,
	__in const char *message,
	...
	)
{
	dmg_assembler_string_t string = {};

	if(dmg_assembler_string_allocate(&string) == DMG_STATUS_SUCCESS) {

		if(token->literal.length) {

			for(uint32_t index = 0; index < token->literal.length; ++index) {

				if(dmg_assembler_string_append(&string, token->literal.str[index]) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_global_find(
	__inout dmg_assembler_globals_t *globals,
	__in const dmg_assembler_token_t *token,
	__out dmg_assembler_global_t **global
	)
{
	uint32_t index = 0;
	int result = DMG_STATUS_SUCCESS;

	for(; index < globals->count; ++index) {
		*global = &globals->global[index];

		if((token->type == (*global)->token->type)
				&& (token->subtype == (*global)->token->subtype)
				&& (token->literal.length == (*global)->token->literal.length)
				&& !strncmp(token->literal.str, (*global)->token->literal.str, token->literal.length)) {
			break;
		}
	}

	if(index == globals->count) {
		result = GLOBAL_ERROR(token, "Failed to final global");
		*global = NULL;
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_globals_reallocate(
	__inout dmg_assembler_globals_t *globals
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!(globals->global = (dmg_assembler_global_t *)realloc(globals->global, sizeof(dmg_assembler_global_t) * globals->capacity * GLOBAL_CAPACITY_SCALE))) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to reallocate global buffer");
		goto exit;
	}

	memset(&(globals->global[globals->capacity]), 0, sizeof(dmg_assembler_global_t) * ((globals->capacity * GLOBAL_CAPACITY_SCALE) - globals->capacity));
	globals->capacity *= GLOBAL_CAPACITY_SCALE;

exit:
	return result;
}

static int
dmg_assembler_globals_resize(
	__inout dmg_assembler_globals_t *globals
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((globals->count + 1) == globals->capacity) {
		result = dmg_assembler_globals_reallocate(globals);
	}

	return result;
}

int
dmg_assembler_global_add(
	__inout dmg_assembler_globals_t *globals,
	__in const dmg_assembler_token_t *token,
	__in const dmg_assembler_scalar_t *value
	)
{
	uint32_t index = 0;
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_global_t *global = NULL;

	for(; index < globals->count; ++index) {

		if(!(global = &globals->global[index])->in_use) {
			break;
		}
	}

	if(index == globals->count) {
		global = &globals->global[globals->count++];

		if((result = dmg_assembler_globals_resize(globals)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	global->token = token;
	global->value.word = value->word;
	global->in_use = true;

exit:
	return result;
}

int
dmg_assembler_global_get(
	__inout dmg_assembler_globals_t *globals,
	__in const dmg_assembler_token_t *token,
	__inout dmg_assembler_scalar_t *value
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_global_t *global = NULL;

	if((result = dmg_assembler_global_find(globals, token, &global)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	value->word = global->value.word;

exit:
	return result;
}

int
dmg_assembler_global_set(
	__inout dmg_assembler_globals_t *globals,
	__in const dmg_assembler_token_t *token,
	__in const dmg_assembler_scalar_t *value
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_global_t *global = NULL;

	if((result = dmg_assembler_global_find(globals, token, &global)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	global->value.word = value->word;

exit:
	return result;
}

void
dmg_assembler_global_remove(
	__inout dmg_assembler_globals_t *globals,
	__in const dmg_assembler_token_t *token
	)
{
	dmg_assembler_global_t *global = NULL;

	if(dmg_assembler_global_find(globals, token, &global) == DMG_STATUS_SUCCESS) {
		global->token = NULL;
		global->value.word = 0;
		global->in_use = false;
	}
}

int
dmg_assembler_globals_allocate(
	__inout dmg_assembler_globals_t *globals
	)
{
	int result = DMG_STATUS_SUCCESS;

	dmg_assembler_globals_free(globals);

	if(!(globals->global = (dmg_assembler_global_t *)calloc(GLOBAL_CAPACITY_INIT, sizeof(dmg_assembler_global_t)))) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to allocate global buffer");
		goto exit;
	}

	globals->capacity = GLOBAL_CAPACITY_INIT;
	globals->count = 0;

exit:
	return result;
}

void
dmg_assembler_globals_free(
	__inout dmg_assembler_globals_t *globals
	)
{

	if(globals->global) {
		free(globals->global);
	}

	memset(globals, 0, sizeof(*globals));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
