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

#include "./parser_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_assembler_parser_error(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token,
	__in const char *message,
	...
	)
{
	dmg_assembler_string_t string = {};

	if(dmg_assembler_string_allocate(&string) == DMG_STATUS_SUCCESS) {

		for(uint32_t index = 0; index < token->literal.length; ++index) {

			if(dmg_assembler_string_append(&string, token->literal.str[index]) != DMG_STATUS_SUCCESS) {
				break;
			}
		}
	}

	ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s \"%s\" (%s@%u)", message, string.str, parser->lexer.stream.path, token->line);
	dmg_assembler_string_free(&string);

	return DMG_STATUS_FAILURE;
}

static int
dmg_assembler_parser_tree_parse_directive(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(token->type != TOKEN_DIRECTIVE) {
		result = PARSER_ERROR(parser, token, "Expecting directive");
		goto exit;
	}

	// TODO
	if(dmg_assembler_lexer_has_next(&parser->lexer)) {
		result = dmg_assembler_lexer_next(&parser->lexer);
	}
	// ---

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_instruction(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(token->type != TOKEN_OPCODE) {
		result = PARSER_ERROR(parser, token, "Expecting instruction");
		goto exit;
	}

	// TODO
	if(dmg_assembler_lexer_has_next(&parser->lexer)) {
		result = dmg_assembler_lexer_next(&parser->lexer);
	}
	// ---

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_label(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(token->type != TOKEN_LABEL) {
		result = PARSER_ERROR(parser, token, "Expecting label");
		goto exit;
	}

	// TODO
	if(dmg_assembler_lexer_has_next(&parser->lexer)) {
		result = dmg_assembler_lexer_next(&parser->lexer);
	}
	// ---

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_statement(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;
	const dmg_assembler_token_t *token;

	switch((token = dmg_assembler_lexer_token(&parser->lexer))->type) {
		case TOKEN_DIRECTIVE:

			if((result = dmg_assembler_parser_tree_parse_directive(parser, token)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case TOKEN_OPCODE:

			if((result = dmg_assembler_parser_tree_parse_instruction(parser, token)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case TOKEN_LABEL:

			if((result = dmg_assembler_parser_tree_parse_label(parser, token)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		default:
			result = PARSER_ERROR(parser, token, "Expecting statement");
			goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(dmg_assembler_lexer_has_next(&parser->lexer)) {

		if((result = dmg_assembler_parser_tree_parse_statement(parser)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		++parser->count;
	}

exit:
	return result;
}

bool
dmg_assembler_parser_has_next(
	__in const dmg_assembler_parser_t *parser
	)
{
	return ((parser->position < parser->count)
		|| (((parser->position + 1) == parser->count) && dmg_assembler_lexer_has_next(&parser->lexer)));
}

bool
dmg_assembler_parser_has_previous(
	__in const dmg_assembler_parser_t *parser
	)
{
	return (parser->position > 0);
}

int
dmg_assembler_parser_load(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_buffer_t *buffer,
	__in const char *path
	)
{
	int result;

	if((result = dmg_assembler_lexer_load(&parser->lexer, buffer, path)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(dmg_assembler_lexer_has_next(&parser->lexer)) {
		result = dmg_assembler_parser_tree_parse(parser);
	}

exit:
	return result;
}

int
dmg_assembler_parser_next(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((++parser->position == parser->count)
			&& ((result = dmg_assembler_parser_tree_parse(parser)) != DMG_STATUS_SUCCESS)) {
		goto exit;
	}

exit:
	return result;
}

int
dmg_assembler_parser_previous(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!dmg_assembler_parser_has_previous(parser)) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "No previous tree %u", parser->position);
		goto exit;
	}

	--parser->position;

exit:
	return result;
}

const
dmg_assembler_tree_t *dmg_assembler_parser_tree(
	__in const dmg_assembler_parser_t *parser
	)
{
	// TODO
	static const dmg_assembler_tree_t tree = {};
	return &tree;
	// ---
}

void
dmg_assembler_parser_unload(
	__inout dmg_assembler_parser_t *parser
	)
{
	dmg_assembler_lexer_unload(&parser->lexer);
	memset(parser, 0, sizeof(*parser));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
