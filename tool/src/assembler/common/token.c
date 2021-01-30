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

#include "./token_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_assembler_tokens_reallocate(
	__inout dmg_assembler_tokens_t *tokens
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!(tokens->token = (dmg_assembler_token_t *)realloc(tokens->token, sizeof(dmg_assembler_token_t) * tokens->capacity * TOKEN_CAPACITY_SCALE))) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to reallocate token buffer");
		goto exit;
	}

	memset(&(tokens->token[tokens->capacity]), 0, sizeof(dmg_assembler_token_t) * ((tokens->capacity * TOKEN_CAPACITY_SCALE) - tokens->capacity));
	tokens->capacity *= TOKEN_CAPACITY_SCALE;

exit:
	return result;
}

static int
dmg_assembler_tokens_resize(
	__inout dmg_assembler_tokens_t *tokens
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((tokens->count + 1) == tokens->capacity) {
		result = dmg_assembler_tokens_reallocate(tokens);
	}

	return result;
}

int
dmg_assembler_token_add(
	__inout dmg_assembler_tokens_t *tokens,
	__out dmg_assembler_token_t **token
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((result = dmg_assembler_tokens_resize(tokens)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	*token = &(tokens->token[tokens->count++]);

exit:
	return result;
}

int
dmg_assembler_tokens_allocate(
	__inout dmg_assembler_tokens_t *tokens
	)
{
	int result = DMG_STATUS_SUCCESS;

	dmg_assembler_tokens_free(tokens);

	if(!(tokens->token = (dmg_assembler_token_t *)calloc(TOKEN_CAPACITY_INIT, sizeof(dmg_assembler_token_t)))) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to allocate token buffer");
		goto exit;
	}

	tokens->capacity = TOKEN_CAPACITY_INIT;
	tokens->count = 0;

exit:
	return result;
}

void
dmg_assembler_tokens_free(
	__inout dmg_assembler_tokens_t *tokens
	)
{

	if(tokens->token) {
		free(tokens->token);
	}

	memset(tokens, 0, sizeof(*tokens));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
