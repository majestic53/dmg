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
dmg_assembler_lexer_token_parse_directive(
	__inout dmg_assembler_lexer_t *lexer
	)
{
	dmg_assembler_token_t *token;
	char str[TOKEN_LITERAL_MAX] = {};
	int result = DMG_STATUS_SUCCESS, type;

	token = &(lexer->token[lexer->position]);
	token->type = TOKEN_DIRECTIVE;
	token->line = lexer->stream.line;
	token->literal.str = dmg_assembler_stream_character_str(&lexer->stream);
	token->literal.length = 0;

	str[token->literal.length] = dmg_assembler_stream_character(&lexer->stream, &type);

	if(((type & CHARACTER_SYMBOL) != CHARACTER_SYMBOL)
			|| (str[token->literal.length] != DELIMITER_DIRECTIVE[0])) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Missing directive delimiter \"%s\" (%s@%u)", str, lexer->stream.path, lexer->stream.line);
		goto exit;
	}

	for(;;) {
		char value;

		if(!dmg_assembler_stream_has_next(&lexer->stream)
				|| ((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS)) {
			break;
		}

		value = dmg_assembler_stream_character(&lexer->stream, &type);
		++token->literal.length;

		if((type & CHARACTER_SPACE) == CHARACTER_SPACE) {
			break;
		}

		str[token->literal.length] = value;
	}

	for(type = 0; type < DIRECTIVE_MAX; ++type) {

		if(!strcmp(dmg_tool_directive_string(type), str)) {
			break;
		}
	}

	if(type == DIRECTIVE_MAX) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Unsupported directive \"%s\" (%s@%u)", str, lexer->stream.path, lexer->stream.line);
		goto exit;
	}

	token->subtype = type;

exit:
	return result;
}

static int
dmg_assembler_lexer_token_parse(
	__inout dmg_assembler_lexer_t *lexer
	)
{
	int result = DMG_STATUS_SUCCESS;

	for(;;) {
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

		if(!dmg_assembler_stream_has_next(&lexer->stream)
				|| ((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS)) {
			break;
		}

	}

	if(dmg_assembler_stream_has_next(&lexer->stream)) {
		int type;
		char value;

		if(((lexer->count + 1) == lexer->capacity)
				&& ((result = dmg_assembler_lexer_token_reallocate(&lexer->token, &lexer->capacity, TOKEN_CAPACITY_SCALE)) != DMG_STATUS_SUCCESS)) {
			goto exit;
		}

		value = dmg_assembler_stream_character(&lexer->stream, &type);

		if((type & CHARACTER_SYMBOL) == CHARACTER_SYMBOL) {

			if(value == DELIMITER_DIRECTIVE[0]) {

				if((result = dmg_assembler_lexer_token_parse_directive(lexer)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
			} else {

// TODO: PARSE OTHER SYMBOLS
lexer->token[lexer->position].scalar.low = dmg_assembler_stream_character(&lexer->stream, &lexer->token[lexer->position].type);
lexer->token[lexer->position].line = lexer->stream.line;

if((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS) {
	goto exit;
}
// ---

			}
		} else {

// TODO: PARSE OTHER TOKEN TYPES
lexer->token[lexer->position].scalar.low = dmg_assembler_stream_character(&lexer->stream, &lexer->token[lexer->position].type);
lexer->token[lexer->position].line = lexer->stream.line;

if((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS) {
	goto exit;
}
// ---

		}

		++lexer->count;
	}

exit:
	return result;
}

bool
dmg_assembler_lexer_has_next(
	__in const dmg_assembler_lexer_t *lexer
	)
{
	return ((lexer->position < lexer->count)
		|| (((lexer->position + 1) == lexer->count) && dmg_assembler_stream_has_next(&lexer->stream)));
}

bool
dmg_assembler_lexer_has_previous(
	__in const dmg_assembler_lexer_t *lexer
	)
{
	return (lexer->position > 0);
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

	if(dmg_assembler_stream_has_next(&lexer->stream)) {
		result = dmg_assembler_lexer_token_parse(lexer);
	}

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

	if(!dmg_assembler_lexer_has_previous(lexer)) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "No previous token %u", lexer->position);
		goto exit;
	}

	--lexer->position;

exit:
	return result;
}

const dmg_assembler_token_t *
dmg_assembler_lexer_token(
	__in const dmg_assembler_lexer_t *lexer
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
