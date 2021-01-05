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
dmg_assembler_parser_tree_parse_directive_bank(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	// TODO

	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_data_byte(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	// TODO

	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_data_word(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	// TODO

	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_define(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	// TODO

	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_if(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	// TODO

	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_if_define(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	// TODO

	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_include(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	// TODO

	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_include_binary(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	// TODO

	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_origin(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	// TODO

	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_reserve(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	// TODO

	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_undefine(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	// TODO

	return result;
}

static int
dmg_assembler_parser_tree_parse_directive(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;
	const dmg_assembler_token_t *token;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type != TOKEN_DIRECTIVE) {
		result = PARSER_ERROR(parser, token, "Expecting directive");
		goto exit;
	}

	parser->trees.tree[parser->position].parent = token;

	if(!dmg_assembler_lexer_has_next(&parser->lexer)) {
		result = PARSER_ERROR(parser, token, "Unterminated directive");
		goto exit;
	}

	if((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	switch(token->subtype) {
		case DIRECTIVE_BANK:

			if((result = dmg_assembler_parser_tree_parse_directive_bank(parser)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_DATA_BYTE:

			if((result = dmg_assembler_parser_tree_parse_directive_data_byte(parser)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_DATA_WORD:

			if((result = dmg_assembler_parser_tree_parse_directive_data_word(parser)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_DEFINE:

			if((result = dmg_assembler_parser_tree_parse_directive_define(parser)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_IF:

			if((result = dmg_assembler_parser_tree_parse_directive_if(parser)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_IF_DEFINED:

			if((result = dmg_assembler_parser_tree_parse_directive_if_define(parser)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_INCLUDE:

			if((result = dmg_assembler_parser_tree_parse_directive_include(parser)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_INCLUDE_BINARY:

			if((result = dmg_assembler_parser_tree_parse_directive_include_binary(parser)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_ORIGIN:

			if((result = dmg_assembler_parser_tree_parse_directive_origin(parser)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_RESERVE:

			if((result = dmg_assembler_parser_tree_parse_directive_reserve(parser)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_UNDEFINE:

			if((result = dmg_assembler_parser_tree_parse_directive_undefine(parser)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		default:
			result = PARSER_ERROR(parser, token, "Invalid directive");
			goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_instruction(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;
	const dmg_assembler_token_t *token;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type != TOKEN_OPCODE) {
		result = PARSER_ERROR(parser, token, "Expecting instruction");
		goto exit;
	}

	parser->trees.tree[parser->position].parent = token;

	if(dmg_assembler_lexer_has_next(&parser->lexer)) {
		result = PARSER_ERROR(parser, token, "Unterminated instruction");
		goto exit;
	}

	if((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	// TODO

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_label(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;
	const dmg_assembler_token_t *token;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type != TOKEN_LABEL) {
		result = PARSER_ERROR(parser, token, "Expecting label");
		goto exit;
	}

	parser->trees.tree[parser->position].parent = token;

	if(dmg_assembler_lexer_has_next(&parser->lexer)) {
		result = dmg_assembler_lexer_next(&parser->lexer);
	}

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

			if((result = dmg_assembler_parser_tree_parse_directive(parser)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case TOKEN_OPCODE:

			if((result = dmg_assembler_parser_tree_parse_instruction(parser)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case TOKEN_LABEL:

			if((result = dmg_assembler_parser_tree_parse_label(parser)) != DMG_STATUS_SUCCESS) {
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

		if((result = dmg_assembler_trees_resize(&parser->trees)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_parser_tree_parse_statement(parser)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		++parser->trees.count;
	}

exit:
	return result;
}

bool
dmg_assembler_parser_has_next(
	__in const dmg_assembler_parser_t *parser
	)
{
	return ((parser->position < parser->trees.count)
		|| (((parser->position + 1) == parser->trees.count) && dmg_assembler_lexer_has_next(&parser->lexer)));
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

	if((result = dmg_assembler_trees_allocate(&parser->trees)) != DMG_STATUS_SUCCESS) {
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

	if((++parser->position == parser->trees.count)
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
	return &parser->trees.tree[parser->position];
}

const
dmg_assembler_tree_t *dmg_assembler_parser_tree_child(
	__in const dmg_assembler_parser_t *parser,
	__in uint32_t position
	)
{
	return dmg_assembler_tree_child(dmg_assembler_parser_tree(parser), position);
}

void
dmg_assembler_parser_unload(
	__inout dmg_assembler_parser_t *parser
	)
{
	dmg_assembler_trees_free(&parser->trees);
	dmg_assembler_lexer_unload(&parser->lexer);
	memset(parser, 0, sizeof(*parser));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
