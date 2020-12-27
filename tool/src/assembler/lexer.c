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

static int
dmg_assembler_lexer_token_parse_alpha(
	__inout dmg_assembler_lexer_t *lexer
	)
{
	dmg_assembler_token_t *token;
	dmg_assembler_string_t string = {};
	int result = DMG_STATUS_SUCCESS, type;

	if((result = dmg_assembler_string_allocate(&string)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	token = &(lexer->tokens.token[lexer->position]);
	token->type = TOKEN_IDENTIFIER;
	token->line = lexer->stream.line;
	token->literal.str = dmg_assembler_stream_character_str(&lexer->stream);
	token->literal.length = 0;

	if((result = dmg_assembler_string_append(&string, dmg_assembler_stream_character(&lexer->stream, &type))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(((type & CHARACTER_ALPHA) != CHARACTER_ALPHA)
			&& (((type & CHARACTER_SYMBOL) == CHARACTER_SYMBOL)
				&& (string.str[token->literal.length] != DELIMITER_IDENTIFIER[0]))) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Expecting identifier \"%s\" (%s@%u)", string.str,
						lexer->stream.path, lexer->stream.line);
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
		} else if(((type & CHARACTER_SYMBOL) == CHARACTER_SYMBOL)
				&& (value != DELIMITER_IDENTIFIER[0])) {

			if(value == DELIMITER_LABEL[0]) {

				if(!dmg_assembler_stream_has_next(&lexer->stream)
						|| ((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS)) {
					break;
				}

				token->type = TOKEN_LABEL;
			}

			break;
		}

		if((result = dmg_assembler_string_append(&string, value)) != DMG_STATUS_SUCCESS) {
			break;
		}
	}

	if(token->type == TOKEN_IDENTIFIER) {

		if(dmg_tool_is_macro_string(string.str, &type)) {
			token->type = TOKEN_MACRO;
			token->subtype = type;
		} else if(dmg_tool_is_opcode_string(string.str, &type)) {
			token->type = TOKEN_OPCODE;
			token->subtype = type;
		} else if(dmg_tool_is_register_string(string.str, &type)) {
			token->type = TOKEN_REGISTER;
			token->subtype = type;
		}
	}

exit:
	dmg_assembler_string_free(&string);

	return result;
}

static int
dmg_assembler_lexer_token_parse_directive(
	__inout dmg_assembler_lexer_t *lexer
	)
{
	dmg_assembler_token_t *token;
	dmg_assembler_string_t string = {};
	int result = DMG_STATUS_SUCCESS, type;

	if((result = dmg_assembler_string_allocate(&string)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	token = &(lexer->tokens.token[lexer->position]);
	token->type = TOKEN_DIRECTIVE;
	token->line = lexer->stream.line;
	token->literal.str = dmg_assembler_stream_character_str(&lexer->stream);
	token->literal.length = 0;

	if((result = dmg_assembler_string_append(&string, dmg_assembler_stream_character(&lexer->stream, &type))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(((type & CHARACTER_SYMBOL) != CHARACTER_SYMBOL)
			|| (string.str[token->literal.length] != DELIMITER_DIRECTIVE[0])) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Expecting directive \"%s\" (%s@%u)", string.str,
						lexer->stream.path, lexer->stream.line);
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

		if((result = dmg_assembler_string_append(&string, value)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	if(!dmg_tool_is_directive_string(string.str, &type)) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Unsupported directive \"%s\" (%s@%u)", string.str,
						lexer->stream.path, lexer->stream.line);
		goto exit;
	}

	token->subtype = type;

exit:
	dmg_assembler_string_free(&string);

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

		if((result = dmg_assembler_tokens_resize(&lexer->tokens)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		value = dmg_assembler_stream_character(&lexer->stream, &type);

		if((type & CHARACTER_ALPHA) == CHARACTER_ALPHA) {

			if((result = dmg_assembler_lexer_token_parse_alpha(lexer)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		} else if((type & CHARACTER_SYMBOL) == CHARACTER_SYMBOL) {

			if(value == DELIMITER_DIRECTIVE[0]) {

				if((result = dmg_assembler_lexer_token_parse_directive(lexer)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
			} else if(value == DELIMITER_IDENTIFIER[0]) {

				if((result = dmg_assembler_lexer_token_parse_alpha(lexer)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
			} else {

// TODO: PARSE OTHER TOKEN TYPES
lexer->tokens.token[lexer->position].scalar.low = dmg_assembler_stream_character(&lexer->stream, &lexer->tokens.token[lexer->position].type);
lexer->tokens.token[lexer->position].line = lexer->stream.line;

if((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS) {
	goto exit;
}
// ---
			}
		} else {

// TODO: PARSE OTHER TOKEN TYPES
lexer->tokens.token[lexer->position].scalar.low = dmg_assembler_stream_character(&lexer->stream, &lexer->tokens.token[lexer->position].type);
lexer->tokens.token[lexer->position].line = lexer->stream.line;

if((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS) {
	goto exit;
}
// ---

		}

		++lexer->tokens.count;
	}

exit:
	return result;
}

bool
dmg_assembler_lexer_has_next(
	__in const dmg_assembler_lexer_t *lexer
	)
{
	return ((lexer->position < lexer->tokens.count)
		|| (((lexer->position + 1) == lexer->tokens.count) && dmg_assembler_stream_has_next(&lexer->stream)));
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

	if((result = dmg_assembler_tokens_allocate(&lexer->tokens)) != DMG_STATUS_SUCCESS) {
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

	if((++lexer->position == lexer->tokens.count)
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
	return &lexer->tokens.token[lexer->position];
}

void
dmg_assembler_lexer_unload(
	__inout dmg_assembler_lexer_t *lexer
	)
{
	dmg_assembler_tokens_free(&lexer->tokens);
	dmg_assembler_stream_unload(&lexer->stream);
	memset(lexer, 0, sizeof(*lexer));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
