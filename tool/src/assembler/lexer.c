/**
 * DMG
 * Copyright (C) 2020 David Jolly
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

#include "./lexer_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static void
dmg_assembler_lexer_token_free(
	__inout dmg_assembler_token_t **token
	)
{

	if(token && *token) {
		free(*token);
		*token = NULL;
	}
}

static int
dmg_assembler_lexer_token_allocate(
	__inout dmg_assembler_token_t **token,
	__in uint32_t capacity
	)
{
	int result = DMG_STATUS_SUCCESS;

	dmg_assembler_lexer_token_free(token);

	if((*token = (dmg_assembler_token_t *)calloc(capacity, sizeof(dmg_assembler_token_t))) == NULL) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to allocate token buffer");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_lexer_token_reallocate(
	__inout dmg_assembler_token_t **token,
	__inout uint32_t *capacity,
	__in uint32_t scale
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((*token = (dmg_assembler_token_t *)realloc(*token, sizeof(dmg_assembler_token_t) * *capacity * scale)) == NULL) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to allocate token buffer");
		goto exit;
	}

	memset(&((*token)[*capacity]), 0, sizeof(dmg_assembler_token_t) * *capacity);
	*capacity *= scale;

exit:
	return result;
}

static int
dmg_assembler_lexer_token_parse(
	__inout dmg_assembler_lexer_t *lexer
	)
{
	int result = DMG_STATUS_SUCCESS;

	do {
		int type;
		char value;

		if((value = dmg_assembler_stream_character(&lexer->stream, &type)) == DELIMITER_COMMENT) {

			do {

				if((value = dmg_assembler_stream_character(&lexer->stream, &type)) == CHARACTER_NEWLINE) {
					break;
				}
			} while(dmg_assembler_stream_next(&lexer->stream) == DMG_STATUS_SUCCESS);
		} else if(!(type & CHARACTER_SPACE)) {
			break;
		}
	} while(dmg_assembler_stream_next(&lexer->stream) == DMG_STATUS_SUCCESS);

	if(!dmg_assembler_stream_has_next(&lexer->stream)) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "No next token %u", lexer->position);
		goto exit;
	}

	if(((lexer->count + 1) == lexer->capacity)
			&& ((result = dmg_assembler_lexer_token_reallocate(&lexer->token, &lexer->capacity, TOKEN_CAPACITY_SCALE)) != DMG_STATUS_SUCCESS)) {
		goto exit;
	}

// TODO
lexer->token[lexer->position].scalar.low = dmg_assembler_stream_character(&lexer->stream, &lexer->token[lexer->position].type);
lexer->token[lexer->position].line = lexer->stream.line;

if((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS) {
	goto exit;
}
// ---

	++lexer->count;

exit:
	return result;
}

int
dmg_assembler_lexer_load(
	__inout dmg_assembler_lexer_t *lexer,
	__in const dmg_buffer_t *buffer,
	__in const char *path
	)
{
	int result;

	if((result = dmg_assembler_stream_load(&lexer->stream, buffer, path)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	lexer->capacity = TOKEN_CAPACITY_INIT;

	if((result = dmg_assembler_lexer_token_allocate(&lexer->token, lexer->capacity)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	dmg_assembler_lexer_token_parse(lexer);

exit:
	return result;
}

int
dmg_assembler_lexer_next(
	__inout dmg_assembler_lexer_t *lexer
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((++lexer->position == lexer->count)
			&& ((result = dmg_assembler_lexer_token_parse(lexer)) != DMG_STATUS_SUCCESS)) {
		goto exit;
	}

exit:
	return result;
}

int
dmg_assembler_lexer_previous(
	__inout dmg_assembler_lexer_t *lexer
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(lexer->position == 0) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "No previous token %u", lexer->position);
		goto exit;
	}

	--lexer->position;

exit:
	return result;
}

const dmg_assembler_token_t *
dmg_assembler_lexer_token(
	__inout dmg_assembler_lexer_t *lexer
	)
{
	return &lexer->token[lexer->position];
}

void
dmg_assembler_lexer_unload(
	__inout dmg_assembler_lexer_t *lexer
	)
{
	dmg_assembler_lexer_token_free(&lexer->token);
	dmg_assembler_stream_unload(&lexer->stream);
	memset(lexer, 0, sizeof(*lexer));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
