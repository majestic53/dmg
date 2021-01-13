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
dmg_assembler_parser_tree_parse_expression(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token,
	__in dmg_assembler_tree_t *root
	);

/*static int
dmg_assembler_parser_tree_parse_expression_arithmetic_0(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token,
	__in dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	// TODO

	token = dmg_assembler_lexer_token(&parser->lexer);

	if((token->type == TOKEN_OPERATOR)
			&& ((token->subtype == OPERATOR_ARITHMETIC_ADD)
				|| (token->subtype == OPERATOR_ARITHMETIC_SUBTRACT))) {

		if((result = dmg_assembler_trees_add_child(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
			result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
			goto exit;
		}

		if(dmg_assembler_lexer_has_next(&parser->lexer)) {
			result = dmg_assembler_lexer_next(&parser->lexer);
		}

		root = child;
		token = dmg_assembler_lexer_token(&parser->lexer);

		// TODO
	} else {
		// TODO
	}

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_expression_arithmetic_1(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token,
	__in dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	// TODO

	token = dmg_assembler_lexer_token(&parser->lexer);

	if((token->type == TOKEN_OPERATOR)
			&& ((token->subtype == OPERATOR_ARITHMETIC_DIVIDE)
				|| (token->subtype == OPERATOR_ARITHMETIC_MODULUS)
				|| (token->subtype == OPERATOR_ARITHMETIC_MULTIPLY))) {

		if((result = dmg_assembler_trees_add_child(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
			result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
			goto exit;
		}

		if(dmg_assembler_lexer_has_next(&parser->lexer)) {
			result = dmg_assembler_lexer_next(&parser->lexer);
		}

		root = child;
		token = dmg_assembler_lexer_token(&parser->lexer);

		// TODO
	} else {
		// TODO
	}

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_expression_binary(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token,
	__in dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	// TODO

	token = dmg_assembler_lexer_token(&parser->lexer);

	if((token->type == TOKEN_OPERATOR)
			&& ((token->subtype == OPERATOR_BINARY_AND)
				|| (token->subtype == OPERATOR_BINARY_OR)
				|| (token->subtype == OPERATOR_BINARY_XOR))) {

		if((result = dmg_assembler_trees_add_child(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
			result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
			goto exit;
		}

		if(dmg_assembler_lexer_has_next(&parser->lexer)) {
			result = dmg_assembler_lexer_next(&parser->lexer);
		}

		root = child;
		token = dmg_assembler_lexer_token(&parser->lexer);

		// TODO
	} else {
		// TODO
	}

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_expression_logical(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token,
	__in dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	// TODO

	token = dmg_assembler_lexer_token(&parser->lexer);

	if((token->type == TOKEN_OPERATOR)
			&& ((token->subtype == OPERATOR_LOGICAL_AND)
				|| (token->subtype == OPERATOR_LOGICAL_OR)
				|| (token->subtype == OPERATOR_LOGICAL_SHIFT_LEFT)
				|| (token->subtype == OPERATOR_LOGICAL_SHIFT_RIGHT))) {

		if((result = dmg_assembler_trees_add_child(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
			result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
			goto exit;
		}

		if(dmg_assembler_lexer_has_next(&parser->lexer)) {
			result = dmg_assembler_lexer_next(&parser->lexer);
		}

		root = child;
		token = dmg_assembler_lexer_token(&parser->lexer);

		// TODO
	} else {
		// TODO
	}

exit:
	return result;
}*/

static int
dmg_assembler_parser_tree_parse_expression_factor(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token,
	__in dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	token = dmg_assembler_lexer_token(&parser->lexer);

	if(token->type == TOKEN_OPERATOR) {

		if((token->subtype != OPERATOR_ARITHMETIC_SUBTRACT)
				&& (token->subtype != OPERATOR_UNARY_NEGATE)
				&& (token->subtype != OPERATOR_UNARY_NOT)) {
			result = PARSER_ERROR(parser, token, "Unsupported unary operator");
			goto exit;
		}

		if((result = dmg_assembler_trees_add_child(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
			result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
			goto exit;
		}

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| (dmg_assembler_lexer_next(&parser->lexer) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated expression");
			goto exit;
		}

		root = child;
		token = dmg_assembler_lexer_token(&parser->lexer);
	}

	if(token->type == TOKEN_MACRO) {

		if((result = dmg_assembler_trees_add_child(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
			result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
			goto exit;
		}

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| (dmg_assembler_lexer_next(&parser->lexer) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated expression");
			goto exit;
		}

		root = child;
		token = dmg_assembler_lexer_token(&parser->lexer);
	}

	if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| (dmg_assembler_lexer_next(&parser->lexer) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated expression");
			goto exit;
		}

		token = dmg_assembler_lexer_token(&parser->lexer);

		if((result = dmg_assembler_parser_tree_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		token = dmg_assembler_lexer_token(&parser->lexer);

		if((token->type != TOKEN_SYMBOL)
				|| (token->subtype != SYMBOL_BRACE_CLOSE)) {

			if(dmg_assembler_lexer_previous(&parser->lexer) == DMG_STATUS_SUCCESS) {
				token = dmg_assembler_lexer_token(&parser->lexer);
			}

			result = PARSER_ERROR(parser, token, "Unterminated expression brace");
			goto exit;
		}

		if(dmg_assembler_lexer_has_next(&parser->lexer)) {
			result = dmg_assembler_lexer_next(&parser->lexer);
		}
	} else if((token->type == TOKEN_IDENTIFIER)
			|| (token->type == TOKEN_LITERAL)
			|| (token->type == TOKEN_SCALAR)) {

		if((result = dmg_assembler_trees_add_child(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
			result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
			goto exit;
		}

		if(dmg_assembler_lexer_has_next(&parser->lexer)) {
			result = dmg_assembler_lexer_next(&parser->lexer);
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting expression factor");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_expression(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token,
	__in dmg_assembler_tree_t *root
	)
{
	// TODO
	return dmg_assembler_parser_tree_parse_expression_factor(parser, token, root);
	// ---
}

static int
dmg_assembler_parser_tree_parse_expression_list(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token,
	__in dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((result = dmg_assembler_parser_tree_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	for(;;) {
		token = dmg_assembler_lexer_token(&parser->lexer);

		if((token->type != TOKEN_SYMBOL)
				|| (token->subtype != SYMBOL_SEPERATOR)) {
			break;
		}

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated expression list");
			goto exit;
		}

		if((result = dmg_assembler_parser_tree_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_bank(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *root = NULL;

	if(token->subtype != DIRECTIVE_BANK) {
		result = PARSER_ERROR(parser, token, "Expecting bank directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_add(&parser->trees, token, &root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated bank directive");
		goto exit;
	}

	if((result = dmg_assembler_parser_tree_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_data(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *root = NULL;

	if((token->subtype != DIRECTIVE_DATA_BYTE)
			&& (token->subtype != DIRECTIVE_DATA_WORD)) {
		result = PARSER_ERROR(parser, token, "Expecting data directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_add(&parser->trees, token, &root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated data directive");
		goto exit;
	}

	if((result = dmg_assembler_parser_tree_parse_expression_list(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_define(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL, *root = NULL;

	if(token->subtype != DIRECTIVE_DEFINE) {
		result = PARSER_ERROR(parser, token, "Expecting define directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_add(&parser->trees, token, &root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated define directive");
		goto exit;
	}

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type != TOKEN_IDENTIFIER) {
		result = PARSER_ERROR(parser, token, "Expecting identifier");
		goto exit;
	}

	if((result = dmg_assembler_trees_add_child(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated define directive");
		goto exit;
	}

	if((result = dmg_assembler_parser_tree_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_if(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *root = NULL;

	if(token->subtype != DIRECTIVE_IF) {
		result = PARSER_ERROR(parser, token, "Expecting if directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_add(&parser->trees, token, &root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated if directive");
		goto exit;
	}

	// TODO

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_if_define(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *root = NULL;

	if(token->subtype != DIRECTIVE_IF_DEFINE) {
		result = PARSER_ERROR(parser, token, "Expecting if-define directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_add(&parser->trees, token, &root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated if-define directive");
		goto exit;
	}

	// TODO

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_include(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL, *root = NULL;

	if((token->subtype != DIRECTIVE_INCLUDE)
			&& (token->subtype != DIRECTIVE_INCLUDE_BINARY)) {
		result = PARSER_ERROR(parser, token, "Expecting include directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_add(&parser->trees, token, &root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated include directive");
		goto exit;
	}

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type != TOKEN_LITERAL) {
		result = PARSER_ERROR(parser, token, "Expecting literal");
		goto exit;
	}

	if((result = dmg_assembler_trees_add_child(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(dmg_assembler_lexer_has_next(&parser->lexer)) {
		result = dmg_assembler_lexer_next(&parser->lexer);
	}

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_origin(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *root = NULL;

	if(token->subtype != DIRECTIVE_ORIGIN) {
		result = PARSER_ERROR(parser, token, "Expecting origin directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_add(&parser->trees, token, &root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated origin directive");
		goto exit;
	}

	if((result = dmg_assembler_parser_tree_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_reserve(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *root = NULL;

	if(token->subtype != DIRECTIVE_RESERVE) {
		result = PARSER_ERROR(parser, token, "Expecting reserve directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_add(&parser->trees, token, &root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated reserve directive");
		goto exit;
	}

	if((result = dmg_assembler_parser_tree_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_parser_tree_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_tree_parse_directive_undefine(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_assembler_token_t *token
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL, *root = NULL;

	if(token->subtype != DIRECTIVE_UNDEFINE) {
		result = PARSER_ERROR(parser, token, "Expecting undefine directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_add(&parser->trees, token, &root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated undefine directive");
		goto exit;
	}

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type != TOKEN_IDENTIFIER) {
		result = PARSER_ERROR(parser, token, "Expecting identifier");
		goto exit;
	}

	if((result = dmg_assembler_trees_add_child(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(dmg_assembler_lexer_has_next(&parser->lexer)) {
		result = dmg_assembler_lexer_next(&parser->lexer);
	}

exit:
	return result;
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

	switch(token->subtype) {
		case DIRECTIVE_BANK:

			if((result = dmg_assembler_parser_tree_parse_directive_bank(parser, token)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_DATA_BYTE:
		case DIRECTIVE_DATA_WORD:

			if((result = dmg_assembler_parser_tree_parse_directive_data(parser, token)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_DEFINE:

			if((result = dmg_assembler_parser_tree_parse_directive_define(parser, token)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_IF:

			if((result = dmg_assembler_parser_tree_parse_directive_if(parser, token)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_IF_DEFINE:

			if((result = dmg_assembler_parser_tree_parse_directive_if_define(parser, token)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_INCLUDE:
		case DIRECTIVE_INCLUDE_BINARY:

			if((result = dmg_assembler_parser_tree_parse_directive_include(parser, token)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_ORIGIN:

			if((result = dmg_assembler_parser_tree_parse_directive_origin(parser, token)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_RESERVE:

			if((result = dmg_assembler_parser_tree_parse_directive_reserve(parser, token)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case DIRECTIVE_UNDEFINE:

			if((result = dmg_assembler_parser_tree_parse_directive_undefine(parser, token)) != DMG_STATUS_SUCCESS) {
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
	dmg_assembler_tree_t *root = NULL;

	if(token->type != TOKEN_LABEL) {
		result = PARSER_ERROR(parser, token, "Expecting label");
		goto exit;
	}

	if((result = dmg_assembler_trees_add(&parser->trees, token, &root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

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
	return (parser->position < parser->count);
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

	while(dmg_assembler_lexer_has_next(&parser->lexer)) {

		if((result = dmg_assembler_parser_tree_parse(parser)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		++parser->position;
	}

	parser->position = 0;

exit:
	return result;
}

int
dmg_assembler_parser_next(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!dmg_assembler_parser_has_next(parser)) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "No next tree %u", parser->position);
		goto exit;
	}

	++parser->position;

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
	return parser->trees.entry[parser->position];
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
