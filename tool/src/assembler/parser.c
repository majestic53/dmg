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

	ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s \"%s\" (%s@%u)", message, string.str, parser->lexer.stream.path, token->line);
	dmg_assembler_string_free(&string);

	return DMG_STATUS_FAILURE;
}

static int
dmg_assembler_parser_parse_expression(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	);

static int
dmg_assembler_parser_parse_expression_factor(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	token = dmg_assembler_lexer_token(&parser->lexer);

	if(token->type == TOKEN_OPERATOR) {

		if((token->subtype != OPERATOR_ARITHMETIC_SUBTRACT)
				&& (token->subtype != OPERATOR_UNARY_NEGATE)
				&& (token->subtype != OPERATOR_UNARY_NOT)) {
			result = PARSER_ERROR(parser, token, "Unsupported operator");
			goto exit;
		}

		if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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

		if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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

		if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		token = dmg_assembler_lexer_token(&parser->lexer);

		if((token->type != TOKEN_SYMBOL)
				|| (token->subtype != SYMBOL_BRACE_CLOSE)) {

			if((dmg_assembler_lexer_next(&parser->lexer) == DMG_STATUS_SUCCESS)
					&& (dmg_assembler_lexer_previous(&parser->lexer) == DMG_STATUS_SUCCESS)) {
				token = dmg_assembler_lexer_token(&parser->lexer);
			}

			result = PARSER_ERROR(parser, token, "Unterminated expression");
			goto exit;
		}

		if(dmg_assembler_lexer_has_next(&parser->lexer)) {
			result = dmg_assembler_lexer_next(&parser->lexer);
		}
	} else if((token->type == TOKEN_IDENTIFIER)
			|| (token->type == TOKEN_LABEL)
			|| (token->type == TOKEN_LITERAL)
			|| (token->type == TOKEN_REGISTER)
			|| (token->type == TOKEN_SCALAR)) {

		if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
			result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
			goto exit;
		}

		if(dmg_assembler_lexer_has_next(&parser->lexer)) {
			result = dmg_assembler_lexer_next(&parser->lexer);
		}

		if((token->type != TOKEN_REGISTER)
				&& (token->type != TOKEN_SCALAR)) {
			token = dmg_assembler_lexer_token(&parser->lexer);

			if((token->type == TOKEN_SYMBOL)
					&& (token->subtype == SYMBOL_BRACKET_OPEN)) {

				if(!dmg_assembler_lexer_has_next(&parser->lexer)
						|| (dmg_assembler_lexer_next(&parser->lexer) != DMG_STATUS_SUCCESS)) {
					result = PARSER_ERROR(parser, token, "Unterminated expression");
					goto exit;
				}

				token = dmg_assembler_lexer_token(&parser->lexer);

				if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}

				token = dmg_assembler_lexer_token(&parser->lexer);

				if((token->type != TOKEN_SYMBOL)
						|| (token->subtype != SYMBOL_BRACKET_CLOSE)) {

					if((dmg_assembler_lexer_next(&parser->lexer) == DMG_STATUS_SUCCESS)
							&& (dmg_assembler_lexer_previous(&parser->lexer) == DMG_STATUS_SUCCESS)) {
						token = dmg_assembler_lexer_token(&parser->lexer);
					}

					result = PARSER_ERROR(parser, token, "Unterminated expression");
					goto exit;
				}

				if(dmg_assembler_lexer_has_next(&parser->lexer)) {
					result = dmg_assembler_lexer_next(&parser->lexer);
				}
			}
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting expression");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_expression(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result;
	dmg_assembler_tree_t *child = NULL, *child_left = NULL;

	if((result = dmg_assembler_trees_add(&parser->trees, false, NULL, &child_left)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_parser_parse_expression_factor(parser, token, child_left)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	token = dmg_assembler_lexer_token(&parser->lexer);

	if(token->type == TOKEN_OPERATOR) {

		if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
			result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
			goto exit;
		}

		root = child;

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| (dmg_assembler_lexer_next(&parser->lexer) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated expression");
			goto exit;
		}

		token = dmg_assembler_lexer_token(&parser->lexer);

		if((result = dmg_assembler_trees_append_child_tree(root, child_left)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((result = dmg_assembler_trees_append_child_tree(root, child_left)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_expression_list(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
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

		if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_conditional(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result;
	dmg_assembler_tree_t *child = NULL, *child_left = NULL;

	if((result = dmg_assembler_trees_add(&parser->trees, false, NULL, &child_left)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_parser_parse_expression(parser, token, child_left)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	token = dmg_assembler_lexer_token(&parser->lexer);

	if(token->type == TOKEN_INEQUALITY) {

		if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
			result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
			goto exit;
		}

		root = child;

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| (dmg_assembler_lexer_next(&parser->lexer) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated condition");
			goto exit;
		}

		token = dmg_assembler_lexer_token(&parser->lexer);

		if((result = dmg_assembler_trees_append_child_tree(root, child_left)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((result = dmg_assembler_trees_append_child_tree(root, child_left)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_statement(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_tree_t *root
	);

static int
dmg_assembler_parser_parse_statement_list(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;

	for(;;) {
		token = dmg_assembler_lexer_token(&parser->lexer);

		if((token->type == TOKEN_DIRECTIVE)
				&& ((token->subtype == DIRECTIVE_ELSE)
					|| (token->subtype == DIRECTIVE_ELSE_IF)
					|| (token->subtype == DIRECTIVE_END))) {
			break;
		}

		if((result = dmg_assembler_parser_parse_statement(parser, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(!dmg_assembler_lexer_has_next(&parser->lexer)) {
			result = PARSER_ERROR(parser, token, "Unterminated statement list");
			goto exit;
		}
	}

exit:
	return result;

}

static int
dmg_assembler_parser_parse_directive_bank(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if(token->subtype != DIRECTIVE_BANK) {
		result = PARSER_ERROR(parser, token, "Expecting directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	root = child;

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated directive");
		goto exit;
	}

	if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_directive_data(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->subtype != DIRECTIVE_DATA_BYTE)
			&& (token->subtype != DIRECTIVE_DATA_WORD)) {
		result = PARSER_ERROR(parser, token, "Expecting directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	root = child;

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated directive");
		goto exit;
	}

	if((result = dmg_assembler_parser_parse_expression_list(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_directive_define(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if(token->subtype != DIRECTIVE_DEFINE) {
		result = PARSER_ERROR(parser, token, "Expecting directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	root = child;

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated directive");
		goto exit;
	}

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type != TOKEN_IDENTIFIER) {
		result = PARSER_ERROR(parser, token, "Expecting identifier");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated directive");
		goto exit;
	}

	if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_directive_else(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if(token->subtype != DIRECTIVE_ELSE) {
		result = PARSER_ERROR(parser, token, "Expecting directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	root = child;

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated directive");
		goto exit;
	}

	if((result = dmg_assembler_parser_parse_statement_list(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_directive_else_if(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if(token->subtype != DIRECTIVE_ELSE_IF) {
		result = PARSER_ERROR(parser, token, "Expecting directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	root = child;

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated directive");
		goto exit;
	}

	if((result = dmg_assembler_parser_parse_conditional(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_parser_parse_statement_list(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;

}

static int
dmg_assembler_parser_parse_directive_end(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((token->type != TOKEN_DIRECTIVE)
			|| (token->subtype != DIRECTIVE_END)) {
		result = PARSER_ERROR(parser, token, "Unterminated directive");
		goto exit;
	}

	if(dmg_assembler_lexer_has_next(&parser->lexer)) {
		result = dmg_assembler_lexer_next(&parser->lexer);
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_directive_if(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if(token->subtype != DIRECTIVE_IF) {
		result = PARSER_ERROR(parser, token, "Expecting directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	root = child;

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated directive");
		goto exit;
	}

	if((result = dmg_assembler_parser_parse_conditional(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_parser_parse_statement_list(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	token = dmg_assembler_lexer_token(&parser->lexer);

	while((token->type == TOKEN_DIRECTIVE)
			&& (token->subtype == DIRECTIVE_ELSE_IF)) {

		if((result = dmg_assembler_parser_parse_directive_else_if(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		token = dmg_assembler_lexer_token(&parser->lexer);
	}

	if((token->type == TOKEN_DIRECTIVE)
			&& (token->subtype == DIRECTIVE_ELSE)) {

		if((result = dmg_assembler_parser_parse_directive_else(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		token = dmg_assembler_lexer_token(&parser->lexer);
	}

	if((result = dmg_assembler_parser_parse_directive_end(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_directive_if_define(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->subtype != DIRECTIVE_IF_DEFINE)
			&& (token->subtype != DIRECTIVE_IF_NOT_DEFINE)) {
		result = PARSER_ERROR(parser, token, "Expecting directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	root = child;

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated directive");
		goto exit;
	}

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type != TOKEN_IDENTIFIER) {
		result = PARSER_ERROR(parser, token, "Expecting identifier");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated directive");
		goto exit;
	}

	if((result = dmg_assembler_parser_parse_statement_list(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	token = dmg_assembler_lexer_token(&parser->lexer);

	if((token->type == TOKEN_DIRECTIVE)
			&& (token->subtype == DIRECTIVE_ELSE)) {

		if((result = dmg_assembler_parser_parse_directive_else(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		token = dmg_assembler_lexer_token(&parser->lexer);
	}

	if((result = dmg_assembler_parser_parse_directive_end(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_directive_include(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->subtype != DIRECTIVE_INCLUDE)
			&& (token->subtype != DIRECTIVE_INCLUDE_BINARY)) {
		result = PARSER_ERROR(parser, token, "Expecting directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	root = child;

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated directive");
		goto exit;
	}

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type != TOKEN_LITERAL) {
		result = PARSER_ERROR(parser, token, "Expecting literal");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_directive_origin(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if(token->subtype != DIRECTIVE_ORIGIN) {
		result = PARSER_ERROR(parser, token, "Expecting directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	root = child;

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated directive");
		goto exit;
	}

	if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_directive_reserve(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if(token->subtype != DIRECTIVE_RESERVE) {
		result = PARSER_ERROR(parser, token, "Expecting directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	root = child;

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated directive");
		goto exit;
	}

	if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_directive_undefine(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if(token->subtype != DIRECTIVE_UNDEFINE) {
		result = PARSER_ERROR(parser, token, "Expecting directive");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	root = child;

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated directive");
		goto exit;
	}

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type != TOKEN_IDENTIFIER) {
		result = PARSER_ERROR(parser, token, "Expecting identifier");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(dmg_assembler_lexer_has_next(&parser->lexer)) {
		result = dmg_assembler_lexer_next(&parser->lexer);
	}

exit:
	return result;
}

static dmg_assembler_parser_hdlr DIRECTIVE_HANDLER[] = {
	dmg_assembler_parser_parse_directive_bank, /* DIRECTIVE_BANK */
	dmg_assembler_parser_parse_directive_data, /* DIRECTIVE_DATA_BYTE */
	dmg_assembler_parser_parse_directive_data, /* DIRECTIVE_DATA_WORD */
	dmg_assembler_parser_parse_directive_define, /* DIRECTIVE_DEFINE */
	NULL, /* DIRECTIVE_ELSE_IF */
	NULL, /* DIRECTIVE_ELSE */
	NULL, /* DIRECTIVE_END */
	dmg_assembler_parser_parse_directive_if, /* DIRECTIVE_IF */
	dmg_assembler_parser_parse_directive_if_define, /* DIRECTIVE_IF_DEFINE */
	dmg_assembler_parser_parse_directive_if_define, /* DIRECTIVE_IF_NOT_DEFINE */
	dmg_assembler_parser_parse_directive_include, /* DIRECTIVE_INCLUDE */
	dmg_assembler_parser_parse_directive_include, /* DIRECTIVE_INCLUDE_BINARY */
	dmg_assembler_parser_parse_directive_origin, /* DIRECTIVE_ORIGIN */
	dmg_assembler_parser_parse_directive_reserve, /* DIRECTIVE_RESERVE */
	dmg_assembler_parser_parse_directive_undefine, /* DIRECTIVE_UNDEFINE */
	};

static int
dmg_assembler_parser_parse_directive(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_parser_hdlr handler;

	if(token->type != TOKEN_DIRECTIVE) {
		result = PARSER_ERROR(parser, token, "Expecting directive");
		goto exit;
	}

	if((token->subtype >= DIRECTIVE_MAX)
			|| !(handler = DIRECTIVE_HANDLER[token->subtype])) {
		result = PARSER_ERROR(parser, token, "Invalid directive");
		goto exit;
	}

	if((result = handler(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_operand_condition(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if(token->type != TOKEN_CONDITION) {
		result = PARSER_ERROR(parser, token, "Expecting condition");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_operand_expression_indirect(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((result = dmg_assembler_parser_parse_expression(parser, dmg_assembler_lexer_token(&parser->lexer), root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	token = dmg_assembler_lexer_token(&parser->lexer);

	if((token->type != TOKEN_SYMBOL)
			|| (token->subtype != SYMBOL_BRACE_CLOSE)) {
		result = PARSER_ERROR(parser, token, "Expecting brace");
		goto exit;
	}

	if(dmg_assembler_lexer_has_next(&parser->lexer)) {
		result = dmg_assembler_lexer_next(&parser->lexer);
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_operand_register(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if(token->type != TOKEN_REGISTER) {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_operand_register_indirect(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if(token->type != TOKEN_REGISTER) {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Expecting brace");
		goto exit;
	}

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_OPERATOR) {
		root = child;

		if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
			result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
			goto exit;
		}

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Expecting brace");
			goto exit;
		}
	}

	token = dmg_assembler_lexer_token(&parser->lexer);

	if((token->type != TOKEN_SYMBOL)
			|| (token->subtype != SYMBOL_BRACE_CLOSE)) {
		result = PARSER_ERROR(parser, token, "Expecting brace");
		goto exit;
	}

	if(dmg_assembler_lexer_has_next(&parser->lexer)) {
		result = dmg_assembler_lexer_next(&parser->lexer);
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_operand_seperator(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((token->type != TOKEN_SYMBOL)
			|| (token->subtype != SYMBOL_SEPERATOR)) {
		result = PARSER_ERROR(parser, token, "Expecting seperator");
		goto exit;
	}

	if(dmg_assembler_lexer_has_next(&parser->lexer)) {
		result = dmg_assembler_lexer_next(&parser->lexer);
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_adc(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_ADC)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_add(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_ADD)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	token = dmg_assembler_lexer_token(&parser->lexer);
	if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_SEPERATOR)) {

		if((result = dmg_assembler_parser_parse_operand_seperator(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

			if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		} else if((token->type == TOKEN_SYMBOL)
				&& (token->subtype == SYMBOL_BRACE_OPEN)) {

			if(!dmg_assembler_lexer_has_next(&parser->lexer)
					|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
				result = PARSER_ERROR(parser, token, "Unterminated opcode");
				goto exit;
			}

			if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
					!= DMG_STATUS_SUCCESS) {
				goto exit;
			}
		} else if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_and(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_AND)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_bit(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype < OPCODE_BIT0)
			|| (token->subtype > OPCODE_BIT7)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_call(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_CALL)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_CONDITION) {

		if((result = dmg_assembler_parser_parse_operand_condition(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_seperator(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}

		token = dmg_assembler_lexer_token(&parser->lexer);
	}

	if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_ccf(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_CCF)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_instruction_cp(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_CP)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_cpl(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_CPL)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_instruction_daa(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_DAA)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_instruction_dec(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;
	dmg_assembler_token_t *parent = token;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_DEC)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(token->subtype == REGISTER_HL) {
			parent->subtype = OPCODE_DEC_HL;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_di(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_DI)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_instruction_ei(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_EI)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_instruction_halt(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_HALT)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_instruction_inc(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;
	dmg_assembler_token_t *parent = token;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_INC)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(token->subtype == REGISTER_HL) {
			parent->subtype = OPCODE_INC_HL;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_jp(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_JP)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_CONDITION) {

		if((result = dmg_assembler_parser_parse_operand_condition(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_seperator(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_expression(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_jr(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_JR)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_CONDITION) {

		if((result = dmg_assembler_parser_parse_operand_condition(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_seperator(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_expression(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_ld(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;
	dmg_assembler_token_t *parent = token;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_LD)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
			result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
			goto exit;
		}

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {
			dmg_assembler_tree_t *child_register = NULL;

			if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, token, root))
					!= DMG_STATUS_SUCCESS) {
				goto exit;
			}

			if((result = dmg_assembler_tree_child(&parser->trees, root, 0, &child_register)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			switch(child_register->token->subtype) {
				case REGISTER_C:
					parent->subtype = OPCODE_LD_IND_C;
					break;
				case REGISTER_HL:

					if(child_register->count) {
						dmg_assembler_tree_t *child_operator = NULL;

						if((result = dmg_assembler_tree_child(&parser->trees, child_register, 0, &child_operator))
								!= DMG_STATUS_SUCCESS) {
							goto exit;
						}

						if(child_operator->token->type == TOKEN_OPERATOR) {

							switch(child_operator->token->subtype) {
								case OPERATOR_ARITHMETIC_ADD:
									parent->subtype = OPCODE_LD_IND_HL_INC;
									break;
								case OPERATOR_ARITHMETIC_SUBTRACT:
									parent->subtype = OPCODE_LD_IND_HL_DEC;
									break;
								default:
									result = PARSER_ERROR(parser, child_operator->token,
												"Unsupported operator");
									goto exit;
							}
						}
					}
					break;
				default:
					break;
			}
		} else {

			if((result = dmg_assembler_parser_parse_operand_expression_indirect(parser, token, root))
					!= DMG_STATUS_SUCCESS) {
				goto exit;
			}

			parent->subtype = OPCODE_LD_IND_U16;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_parser_parse_operand_seperator(parser, dmg_assembler_lexer_token(&parser->lexer), root))
			!= DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if(token->subtype == REGISTER_SP) {
			dmg_assembler_tree_t *child_register = NULL;

			if((result = dmg_assembler_trees_append_child_token(&parser->trees, child, token, &child_register)) != DMG_STATUS_SUCCESS) {
				result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
				goto exit;
			}

			if(dmg_assembler_lexer_has_next(&parser->lexer)
					&& ((result = dmg_assembler_lexer_next(&parser->lexer)) == DMG_STATUS_SUCCESS)) {
				token = dmg_assembler_lexer_token(&parser->lexer);

				if((token->type == TOKEN_OPERATOR)
						&& (token->subtype == OPERATOR_ARITHMETIC_ADD)) {

					if(!dmg_assembler_lexer_has_next(&parser->lexer)
							|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
						result = PARSER_ERROR(parser, token, "Unterminated opcode");
						goto exit;
					}

					if((result = dmg_assembler_parser_parse_expression_factor(parser, dmg_assembler_lexer_token(&parser->lexer),
							child_register)) != DMG_STATUS_SUCCESS) {
						goto exit;
					}

					parent->subtype = OPCODE_LD_HL_SP_I8;
				}
			}
		} else if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {
			dmg_assembler_tree_t *child_register = NULL;

			if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, token, root))
					!= DMG_STATUS_SUCCESS) {
				goto exit;
			}

			if((result = dmg_assembler_tree_child(&parser->trees, root, 1, &child_register)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			switch(child_register->token->subtype) {
				case REGISTER_C:
					parent->subtype = OPCODE_LD_IND_C;
					break;
				case REGISTER_HL:

					if(child_register->count) {
						dmg_assembler_tree_t *child_operator = NULL;

						if((result = dmg_assembler_tree_child(&parser->trees, child_register, 0, &child_operator))
								!= DMG_STATUS_SUCCESS) {
							goto exit;
						}

						if(child_operator->token->type == TOKEN_OPERATOR) {

							switch(child_operator->token->subtype) {
								case OPERATOR_ARITHMETIC_ADD:
									parent->subtype = OPCODE_LD_IND_HL_INC;
									break;
								case OPERATOR_ARITHMETIC_SUBTRACT:
									parent->subtype = OPCODE_LD_IND_HL_DEC;
									break;
								default:
									result = PARSER_ERROR(parser, child_operator->token,
												"Unsupported operator");
									goto exit;
							}
						}
					}
					break;
				default:
					break;
			}
		} else {

			if((result = dmg_assembler_parser_parse_operand_expression_indirect(parser, token, root))
					!= DMG_STATUS_SUCCESS) {
				goto exit;
			}

			parent->subtype = OPCODE_LD_IND_U16;
		}
	} else if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_nop(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_NOP)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_instruction_or(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_OR)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_pop(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_POP)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_push(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_PUSH)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_res(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype < OPCODE_RES0)
			|| (token->subtype > OPCODE_RES7)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_ret(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_RET)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_CONDITION) {

		if((result = dmg_assembler_parser_parse_operand_condition(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_reti(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_RETI)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_instruction_rl(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_RL)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_rla(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_RLA)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_instruction_rlc(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_RLC)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_rlca(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_RLCA)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_instruction_rr(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_RR)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_rra(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_RRA)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_instruction_rrc(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_RRC)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_rrca(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_RRCA)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_instruction_rst(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_RST)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((result = dmg_assembler_parser_parse_expression(parser, dmg_assembler_lexer_token(&parser->lexer), root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_sbc(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_SBC)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_scf(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_SCF)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_instruction_set(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype < OPCODE_SET0)
			|| (token->subtype > OPCODE_SET7)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_sla(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_SLA)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_sra(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_SRA)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_srl(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_SRL)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_stop(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_STOP)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_instruction_sub(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_SUB)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_swap(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_SWAP)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		result = PARSER_ERROR(parser, token, "Expecting register");
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_instruction_unused(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| ((token->subtype != OPCODE_UNUSED_D3)
				&& (token->subtype != OPCODE_UNUSED_DB)
				&& (token->subtype != OPCODE_UNUSED_DD)
				&& (token->subtype != OPCODE_UNUSED_E3)
				&& (token->subtype != OPCODE_UNUSED_E4)
				&& (token->subtype != OPCODE_UNUSED_EB)
				&& (token->subtype != OPCODE_UNUSED_EC)
				&& (token->subtype != OPCODE_UNUSED_ED)
				&& (token->subtype != OPCODE_UNUSED_F4)
				&& (token->subtype != OPCODE_UNUSED_FC)
				&& (token->subtype != OPCODE_UNUSED_FD))) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_instruction_xor(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if((token->type != TOKEN_OPCODE)
			|| (token->subtype != OPCODE_XOR)) {
		result = PARSER_ERROR(parser, token, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
		result = PARSER_ERROR(parser, token, "Exceeded maximum list length");
		goto exit;
	}

	if(!dmg_assembler_lexer_has_next(&parser->lexer)
			|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
		result = PARSER_ERROR(parser, token, "Unterminated opcode");
		goto exit;
	}

	root = child;

	if((token = dmg_assembler_lexer_token(&parser->lexer))->type == TOKEN_REGISTER) {

		if((result = dmg_assembler_parser_parse_operand_register(parser, token, root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((token->type == TOKEN_SYMBOL)
			&& (token->subtype == SYMBOL_BRACE_OPEN)) {

		if(!dmg_assembler_lexer_has_next(&parser->lexer)
				|| ((result = dmg_assembler_lexer_next(&parser->lexer)) != DMG_STATUS_SUCCESS)) {
			result = PARSER_ERROR(parser, token, "Unterminated opcode");
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_operand_register_indirect(parser, dmg_assembler_lexer_token(&parser->lexer), root))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((result = dmg_assembler_parser_parse_expression(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static dmg_assembler_parser_hdlr INSTRUCTION_HANDLER[] = {
	dmg_assembler_parser_parse_instruction_adc, /* OPCODE_ADC */
	dmg_assembler_parser_parse_instruction_add, /* OPCODE_ADD */
	dmg_assembler_parser_parse_instruction_and, /* OPCODE_AND */
	dmg_assembler_parser_parse_instruction_call, /* OPCODE_CALL */
	dmg_assembler_parser_parse_instruction_ccf, /* OPCODE_CCF */
	dmg_assembler_parser_parse_instruction_cp, /* OPCODE_CP */
	dmg_assembler_parser_parse_instruction_cpl, /* OPCODE_CPL */
	dmg_assembler_parser_parse_instruction_daa, /* OPCODE_DAA */
	dmg_assembler_parser_parse_instruction_dec, /* OPCODE_DEC */
	NULL, /* OPCODE_DEC_HL */
	dmg_assembler_parser_parse_instruction_di, /* OPCODE_DI */
	dmg_assembler_parser_parse_instruction_ei, /* OPCODE_EI */
	dmg_assembler_parser_parse_instruction_halt, /* OPCODE_HALT */
	dmg_assembler_parser_parse_instruction_inc, /* OPCODE_INC */
	NULL, /* OPCODE_INC_HL */
	dmg_assembler_parser_parse_instruction_jp, /* OPCODE_JP */
	dmg_assembler_parser_parse_instruction_jr, /* OPCODE_JR */
	dmg_assembler_parser_parse_instruction_ld, /* OPCODE_LD */
	NULL, /* OPCODE_LD_HL_SP_I8 */
	NULL, /* OPCODE_LD_IND_C */
	NULL, /* OPCODE_LD_IND_HL_DEC */
	NULL, /* OPCODE_LD_IND_HL_INC */
	NULL, /* OPCODE_LD_IND_U16 */
	dmg_assembler_parser_parse_instruction_nop, /* OPCODE_NOP */
	dmg_assembler_parser_parse_instruction_or, /* OPCODE_OR */
	dmg_assembler_parser_parse_instruction_pop, /* OPCODE_POP */
	dmg_assembler_parser_parse_instruction_push, /* OPCODE_PUSH */
	dmg_assembler_parser_parse_instruction_ret, /* OPCODE_RET */
	dmg_assembler_parser_parse_instruction_reti, /* OPCODE_RETI */
	dmg_assembler_parser_parse_instruction_rla, /* OPCODE_RLA */
	dmg_assembler_parser_parse_instruction_rlca, /* OPCODE_RLCA */
	dmg_assembler_parser_parse_instruction_rra, /* OPCODE_RRA */
	dmg_assembler_parser_parse_instruction_rrca, /* OPCODE_RRCA */
	dmg_assembler_parser_parse_instruction_rst, /* OPCODE_RST */
	dmg_assembler_parser_parse_instruction_scf, /* OPCODE_SCF */
	dmg_assembler_parser_parse_instruction_sbc, /* OPCODE_SBC */
	dmg_assembler_parser_parse_instruction_stop, /* OPCODE_STOP */
	dmg_assembler_parser_parse_instruction_sub, /* OPCODE_SUB */
	dmg_assembler_parser_parse_instruction_xor, /* OPCODE_XOR */
	dmg_assembler_parser_parse_instruction_unused, /* OPCODE_UNUSED_D3 */
	dmg_assembler_parser_parse_instruction_unused, /* OPCODE_UNUSED_DB */
	dmg_assembler_parser_parse_instruction_unused, /* OPCODE_UNUSED_DD */
	dmg_assembler_parser_parse_instruction_unused, /* OPCODE_UNUSED_E3 */
	dmg_assembler_parser_parse_instruction_unused, /* OPCODE_UNUSED_E4 */
	dmg_assembler_parser_parse_instruction_unused, /* OPCODE_UNUSED_EB */
	dmg_assembler_parser_parse_instruction_unused, /* OPCODE_UNUSED_EC */
	dmg_assembler_parser_parse_instruction_unused, /* OPCODE_UNUSED_ED */
	dmg_assembler_parser_parse_instruction_unused, /* OPCODE_UNUSED_F4 */
	dmg_assembler_parser_parse_instruction_unused, /* OPCODE_UNUSED_FC */
	dmg_assembler_parser_parse_instruction_unused, /* OPCODE_UNUSED_FD */
	dmg_assembler_parser_parse_instruction_bit, /* OPCODE_BIT0 */
	dmg_assembler_parser_parse_instruction_bit, /* OPCODE_BIT1 */
	dmg_assembler_parser_parse_instruction_bit, /* OPCODE_BIT2 */
	dmg_assembler_parser_parse_instruction_bit, /* OPCODE_BIT3 */
	dmg_assembler_parser_parse_instruction_bit, /* OPCODE_BIT4 */
	dmg_assembler_parser_parse_instruction_bit, /* OPCODE_BIT5 */
	dmg_assembler_parser_parse_instruction_bit, /* OPCODE_BIT6 */
	dmg_assembler_parser_parse_instruction_bit, /* OPCODE_BIT7 */
	dmg_assembler_parser_parse_instruction_res, /* OPCODE_RES0 */
	dmg_assembler_parser_parse_instruction_res, /* OPCODE_RES1 */
	dmg_assembler_parser_parse_instruction_res, /* OPCODE_RES2 */
	dmg_assembler_parser_parse_instruction_res, /* OPCODE_RES3 */
	dmg_assembler_parser_parse_instruction_res, /* OPCODE_RES4 */
	dmg_assembler_parser_parse_instruction_res, /* OPCODE_RES5 */
	dmg_assembler_parser_parse_instruction_res, /* OPCODE_RES6 */
	dmg_assembler_parser_parse_instruction_res, /* OPCODE_RES7 */
	dmg_assembler_parser_parse_instruction_rl, /* OPCODE_RL */
	dmg_assembler_parser_parse_instruction_rlc, /* OPCODE_RLC */
	dmg_assembler_parser_parse_instruction_rr, /* OPCODE_RR */
	dmg_assembler_parser_parse_instruction_rrc, /* OPCODE_RRC */
	dmg_assembler_parser_parse_instruction_set, /* OPCODE_SET0 */
	dmg_assembler_parser_parse_instruction_set, /* OPCODE_SET1 */
	dmg_assembler_parser_parse_instruction_set, /* OPCODE_SET2 */
	dmg_assembler_parser_parse_instruction_set, /* OPCODE_SET3 */
	dmg_assembler_parser_parse_instruction_set, /* OPCODE_SET4 */
	dmg_assembler_parser_parse_instruction_set, /* OPCODE_SET5 */
	dmg_assembler_parser_parse_instruction_set, /* OPCODE_SET6 */
	dmg_assembler_parser_parse_instruction_set, /* OPCODE_SET7 */
	dmg_assembler_parser_parse_instruction_sla, /* OPCODE_SLA */
	dmg_assembler_parser_parse_instruction_sra, /* OPCODE_SRA */
	dmg_assembler_parser_parse_instruction_srl, /* OPCODE_SRL */
	dmg_assembler_parser_parse_instruction_swap, /* OPCODE_SWAP */
	};

static int
dmg_assembler_parser_parse_instruction(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_parser_hdlr handler;

	if((token->subtype >= OPCODE_MAX)
			|| !(handler = INSTRUCTION_HANDLER[token->subtype])) {
		result = PARSER_ERROR(parser, token, "Invalid instruction");
		goto exit;
	}

	if((result = handler(parser, token, root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_parser_parse_label(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_token_t *token,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if(token->type != TOKEN_LABEL) {
		result = PARSER_ERROR(parser, token, "Expecting label");
		goto exit;
	}

	if((result = dmg_assembler_trees_append_child_token(&parser->trees, root, token, &child)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse_statement(
	__inout dmg_assembler_parser_t *parser,
	__inout dmg_assembler_tree_t *root
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_token_t *token;

	switch((token = dmg_assembler_lexer_token(&parser->lexer))->type) {
		case TOKEN_DIRECTIVE:

			if((result = dmg_assembler_parser_parse_directive(parser, token, root)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case TOKEN_OPCODE:

			if((result = dmg_assembler_parser_parse_instruction(parser, token, root)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case TOKEN_LABEL:

			if((result = dmg_assembler_parser_parse_label(parser, token, root)) != DMG_STATUS_SUCCESS) {
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
dmg_assembler_parser_parse(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(dmg_assembler_lexer_has_next(&parser->lexer)) {
		dmg_assembler_tree_t *root = NULL;

		if((result = dmg_assembler_trees_add(&parser->trees, true, NULL, &root)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_parser_parse_statement(parser, root)) != DMG_STATUS_SUCCESS) {
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
	return dmg_assembler_lexer_has_next(&parser->lexer);
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

	parser->count = 0;

	if(dmg_assembler_parser_has_next(parser)) {
		result = dmg_assembler_parser_next(parser);
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

	if(!dmg_assembler_parser_has_next(parser)) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "No next tree %u", parser->count);
		goto exit;
	}

	dmg_assembler_trees_free(&parser->trees);

	if((result = dmg_assembler_trees_allocate(&parser->trees)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_parser_parse(parser)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

const
dmg_assembler_tree_t *dmg_assembler_parser_tree(
	__in const dmg_assembler_parser_t *parser
	)
{
	return &parser->trees.tree.ptr[parser->trees.root];
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
