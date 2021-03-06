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

#include "./generator_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NDEBUG

static int
dmg_assembler_generator_error_tree(
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__inout dmg_assembler_string_t *string,
	__in uint32_t depth
	)
{
	int result = DMG_STATUS_SUCCESS;
	char str[GENERATOR_ERROR_STR_MAX] = {};

	if((result = dmg_assembler_string_append_character(string, '\n')) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	for(uint32_t index = 0; index < depth; ++index) {

		if((result = dmg_assembler_string_append_character(string, '\t')) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	if(tree && tree->token) {
		uint32_t index;
		const dmg_assembler_token_t *token = tree->token;

		snprintf(str, GENERATOR_ERROR_STR_MAX, "<%i", token->type);

		if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((token->type != TOKEN_END)
				&& (token->subtype != TOKEN_SUBTYPE_UNDEFINED)) {
			snprintf(str, GENERATOR_ERROR_STR_MAX, ":%i", token->subtype);

			if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}

		snprintf(str, GENERATOR_ERROR_STR_MAX, "> %u", tree->index);

		if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(tree->count) {
			snprintf(str, GENERATOR_ERROR_STR_MAX, "[%u]={", tree->count);

			if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			for(index = 0; index < tree->count; ++index) {

				if(index) {

					if((result = dmg_assembler_string_append_character(string, ',')) != DMG_STATUS_SUCCESS) {
						goto exit;
					}
				}

				snprintf(str, GENERATOR_ERROR_STR_MAX, "%u", tree->child[index]);

				if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
			}

			if((result = dmg_assembler_string_append_character(string, '}')) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}

		if((result = dmg_assembler_string_append_character(string, ' ')) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((token->type > TOKEN_END) && (token->type < TOKEN_MAX)) {

			if((result = dmg_assembler_string_append_character(string, '\"')) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			if(token->indirect) {

				if((result = dmg_assembler_string_append_character(string, '(')) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
			}

			for(index = 0; index < token->literal.length; ++index) {

				if((result = dmg_assembler_string_append_character(string, token->literal.str[index]))
						!= DMG_STATUS_SUCCESS) {
					goto exit;
				}
			}

			if(token->indirect) {

				if((result = dmg_assembler_string_append_character(string, ')')) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
			}

			if((result = dmg_assembler_string_append_character(string, '\"')) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			if(token->type == TOKEN_SCALAR) {
				snprintf(str, GENERATOR_ERROR_STR_MAX, " %04x (%u)", token->scalar.word, token->scalar.word);

				if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
			}
		} else {
			snprintf(str, GENERATOR_ERROR_STR_MAX, " \'%c\' (%02x)", (isprint(token->scalar.low) && !isspace(token->scalar.low))
				? token->scalar.low : CHARACTER_FILL, token->scalar.low);

			if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}

		snprintf(str, GENERATOR_ERROR_STR_MAX, " (%s@%u)", parser->lexer.stream.path, token->line);

		if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		for(index = 0; index < tree->count; ++index) {
			dmg_assembler_tree_t *child = NULL;

			if((result = dmg_assembler_tree_child(&parser->trees, tree, index, &child)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			if((result = dmg_assembler_generator_error_tree(parser, child, string, depth + 1)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}
	}

exit:
	return result;
}

#endif /* NDEBUG */

static int
dmg_assembler_generator_error(
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in const char *message,
	...
	)
{
	dmg_assembler_string_t string = {};

	if(dmg_assembler_string_allocate(&string) == DMG_STATUS_SUCCESS) {

		if(tree->token->literal.length) {

			for(uint32_t index = 0; index < tree->token->literal.length; ++index) {

				if(dmg_assembler_string_append_character(&string, tree->token->literal.str[index]) != DMG_STATUS_SUCCESS) {
					break;
				}
			}
		} else {
			strcpy(string.str, "EOF");
		}
	}

#ifndef NDEBUG
	dmg_assembler_string_t debug_string = {};

	if(dmg_assembler_string_allocate(&debug_string) == DMG_STATUS_SUCCESS) {
		dmg_assembler_generator_error_tree(parser, tree, &debug_string, 1);
	}

	ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s \"%s\" (%s@%u)%s", message, string.str, parser->lexer.stream.path, tree->token->line, debug_string.str);
	dmg_assembler_string_free(&debug_string);
#else
	ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s \"%s\" (%s@%u)", message, string.str, parser->lexer.stream.path, tree->token->line);
#endif /* NDEBUG */
	dmg_assembler_string_free(&string);

	return DMG_STATUS_FAILURE;
}

static int
dmg_assembler_generator_bank_set(
	__inout dmg_assembler_generator_t *generator,
	__in uint32_t bank,
	__in const dmg_assembler_scalar_t *origin,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!first_pass) {

		for(; (generator->banks.count - 1) < bank;) {

			if((result = dmg_assembler_bank_add(&generator->banks, origin)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}
	}

	generator->bank = bank;
	generator->offset.word = (origin->word % BANK_WIDTH);

exit:
	return result;
}

static int
dmg_assembler_generator_bank_set_byte(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_scalar_t *value,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!first_pass && ((result = dmg_assembler_bank_set_byte(&generator->banks, generator->bank, &generator->offset, value))
			!= DMG_STATUS_SUCCESS)) {
		goto exit;
	}

	generator->offset.word += sizeof(value->low);

exit:
	return result;
}

static int
dmg_assembler_generator_bank_set_word(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_scalar_t *value,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!first_pass && ((result = dmg_assembler_bank_set_word(&generator->banks, generator->bank, &generator->offset, value))
			!= DMG_STATUS_SUCCESS)) {
		goto exit;
	}

	generator->offset.word += sizeof(value->word);

exit:
	return result;
}

static int
dmg_assembler_generator_evaluate_expression(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__inout dmg_assembler_scalar_t *value,
	__in bool first_pass
	);

static int
dmg_assembler_generator_generate_statement(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	);

static int
dmg_assembler_generator_evaluate_conditional(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__inout bool *value,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t child_value = {};

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting conditional");
		goto exit;
	}

	if(tree->token->type == TOKEN_INEQUALITY) {
		int subtype = tree->token->subtype;
		dmg_assembler_tree_t *child_left = NULL, *child_right = NULL;
		dmg_assembler_scalar_t child_left_value = {}, child_right_value = {};

		if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child_left)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child_left, &child_left_value, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_tree_child(&parser->trees, tree, 1, &child_right)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child_right, &child_right_value, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		switch(subtype) {
			case INEQUALITY_EQUALS:
				*value = (child_left_value.word == child_right_value.word);
				break;
			case INEQUALITY_GREATER_THAN:
				*value = (child_left_value.word > child_right_value.word);
				break;
			case INEQUALITY_GREATER_THAN_EQUALS:
				*value = (child_left_value.word >= child_right_value.word);
				break;
			case INEQUALITY_LESS_THAN:
				*value = (child_left_value.word < child_right_value.word);
				break;
			case INEQUALITY_LESS_THAN_EQUALS:
				*value = (child_left_value.word <= child_right_value.word);
				break;
			case INEQUALITY_NOT_EQUALS:
				*value = (child_left_value.word != child_right_value.word);
				break;
			default:
				result = GENERATOR_ERROR(parser, tree, "Unsupported inequality");
				goto exit;
		}
	} else {

		if((result = dmg_assembler_generator_evaluate_expression(generator, parser, tree, &child_value, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		*value = (child_value.word != 0);
	}

exit:
	return result;
}

static int
dmg_assembler_generator_evaluate_expression_constant(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__inout dmg_assembler_scalar_t *value,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;

	switch(tree->token->type) {
		case TOKEN_IDENTIFIER:
		case TOKEN_LABEL:

			if((result = dmg_assembler_constant_get(&generator->defines, tree->token, value)) != DMG_STATUS_SUCCESS) {

				if((result = dmg_assembler_constant_get(&generator->labels, tree->token, value)) != DMG_STATUS_SUCCESS) {

					if(first_pass) {
						result = DMG_STATUS_SUCCESS;
					}

					goto exit;
				}
			}
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Expecting constant");
			goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generator_evaluate_expression_literal(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__inout dmg_assembler_scalar_t *value,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(tree->token->type != TOKEN_LITERAL) {
		result = GENERATOR_ERROR(parser, tree, "Expecting literal");
		goto exit;
	}

	if(tree->count) {
		dmg_assembler_tree_t *child = NULL;
		dmg_assembler_scalar_t child_value = {};

		if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &child_value, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(child_value.word >= tree->token->literal.length) {
			result = GENERATOR_ERROR(parser, tree, "Literal index out-of-range");
			goto exit;
		}

		value->word = tree->token->literal.str[child_value.word];
	} else if(tree->token->literal.length) {
		value->word = tree->token->literal.str[0];
	} else {
		value->word = CHARACTER_EOF;
	}

exit:
	return result;
}

static int
dmg_assembler_generator_evaluate_expression_macro(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__inout dmg_assembler_scalar_t *value,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;
	dmg_assembler_scalar_t child_value = {};

	if(tree->token->type != TOKEN_MACRO) {
		result = GENERATOR_ERROR(parser, tree, "Expecting macro");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &child_value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	switch(tree->token->subtype) {
		case MACRO_HIGH:
			value->word = child_value.high;
			break;
		case MACRO_LOW:
			value->word = child_value.low;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported macro");
			goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generator_evaluate_expression_operator_binary(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__inout dmg_assembler_scalar_t *value,
	__in bool first_pass
	)
{
	dmg_assembler_tree_t *child_left = NULL, *child_right = NULL;
	int operator = tree->token->subtype, result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t child_left_value = {}, child_right_value = {};

	if(tree->count < 2) {
		result = GENERATOR_ERROR(parser, tree, "Expecting binary expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child_left)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child_left, &child_left_value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 1, &child_right)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child_right, &child_right_value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	switch(operator) {
		case OPERATOR_ARITHMETIC_ADD:
			value->word = (child_left_value.word + child_right_value.word);
			break;
		case OPERATOR_ARITHMETIC_DIVIDE:
			value->word = (child_left_value.word / child_right_value.word);
			break;
		case OPERATOR_ARITHMETIC_MODULUS:
			value->word = (child_left_value.word % child_right_value.word);
			break;
		case OPERATOR_ARITHMETIC_MULTIPLY:
			value->word = (child_left_value.word * child_right_value.word);
			break;
		case OPERATOR_ARITHMETIC_SUBTRACT:
			value->word = (child_left_value.word - child_right_value.word);
			break;
		case OPERATOR_BINARY_AND:
			value->word = (child_left_value.word & child_right_value.word);
			break;
		case OPERATOR_BINARY_OR:
			value->word = (child_left_value.word | child_right_value.word);
			break;
		case OPERATOR_BINARY_XOR:
			value->word = (child_left_value.word ^ child_right_value.word);
			break;
		case OPERATOR_LOGICAL_AND:
			value->word = (child_left_value.word && child_right_value.word);
			break;
		case OPERATOR_LOGICAL_OR:
			value->word = (child_left_value.word || child_right_value.word);
			break;
		case OPERATOR_LOGICAL_SHIFT_LEFT:
			value->word = (child_left_value.word << child_right_value.word);
			break;
		case OPERATOR_LOGICAL_SHIFT_RIGHT:
			value->word = (child_left_value.word >> child_right_value.word);
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Expecting binary operator");
			goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generator_evaluate_expression_operator_unary(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__inout dmg_assembler_scalar_t *value,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;
	dmg_assembler_scalar_t child_value = {};

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &child_value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	switch(tree->token->subtype) {
		case OPERATOR_UNARY_NEGATE:
			value->word = ~child_value.word;
			break;
		case OPERATOR_UNARY_NOT:
			value->word = !child_value.word;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Expecting unary operator");
			goto exit;
	}

exit:
	return result;
}

static dmg_assembler_evaluator_hdlr OPERATOR_HANDLER[] = {
	dmg_assembler_generator_evaluate_expression_operator_binary, /* OPERATOR_ARITHMETIC_ADD */
	dmg_assembler_generator_evaluate_expression_operator_binary, /* OPERATOR_ARITHMETIC_DIVIDE */
	dmg_assembler_generator_evaluate_expression_operator_binary, /* OPERATOR_ARITHMETIC_MODULUS */
	dmg_assembler_generator_evaluate_expression_operator_binary, /* OPERATOR_ARITHMETIC_MULTIPLY */
	dmg_assembler_generator_evaluate_expression_operator_binary, /* OPERATOR_ARITHMETIC_SUBTRACT */
	dmg_assembler_generator_evaluate_expression_operator_binary, /* OPERATOR_BINARY_AND */
	dmg_assembler_generator_evaluate_expression_operator_binary, /* OPERATOR_BINARY_OR */
	dmg_assembler_generator_evaluate_expression_operator_binary, /* OPERATOR_BINARY_XOR */
	dmg_assembler_generator_evaluate_expression_operator_binary, /* OPERATOR_LOGICAL_AND */
	dmg_assembler_generator_evaluate_expression_operator_binary, /* OPERATOR_LOGICAL_OR */
	dmg_assembler_generator_evaluate_expression_operator_binary, /* OPERATOR_LOGICAL_SHIFT_LEFT */
	dmg_assembler_generator_evaluate_expression_operator_binary, /* OPERATOR_LOGICAL_SHIFT_RIGHT */
	dmg_assembler_generator_evaluate_expression_operator_unary, /* OPERATOR_UNARY_NEGATE */
	dmg_assembler_generator_evaluate_expression_operator_unary, /* OPERATOR_UNARY_NOT */
	};

static int
dmg_assembler_generator_evaluate_expression_operator(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__inout dmg_assembler_scalar_t *value,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_evaluator_hdlr handler;

	if(tree->token->type != TOKEN_OPERATOR) {
		result = GENERATOR_ERROR(parser, tree, "Expecting operator");
		goto exit;
	}

	if((tree->token->subtype >= OPERATOR_MAX)
			|| !(handler = OPERATOR_HANDLER[tree->token->subtype])) {
		result = GENERATOR_ERROR(parser, tree, "Unsupported operator");
		goto exit;
	}

	if((result = handler(generator, parser, tree, value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generator_evaluate_expression_scalar(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__inout dmg_assembler_scalar_t *value,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(tree->token->type != TOKEN_SCALAR) {
		result = GENERATOR_ERROR(parser, tree, "Expecting scalar");
		goto exit;
	}

	value->word = tree->token->scalar.word;

exit:
	return result;
}

static dmg_assembler_evaluator_hdlr EXPRESSION_HANDLER[] = {
	NULL, /* TOKEN_END */
	NULL, /* TOKEN_CONDITION */
	NULL, /* TOKEN_DIRECTIVE */
	dmg_assembler_generator_evaluate_expression_constant, /* TOKEN_IDENTIFIER */
	NULL, /* TOKEN_INEQUALITY */
	dmg_assembler_generator_evaluate_expression_constant, /* TOKEN_LABEL */
	dmg_assembler_generator_evaluate_expression_literal, /* TOKEN_LITERAL */
	dmg_assembler_generator_evaluate_expression_macro, /* TOKEN_MACRO */
	NULL, /* TOKEN_OPCODE */
	dmg_assembler_generator_evaluate_expression_operator, /* TOKEN_OPERATOR */
	NULL, /* TOKEN_REGISTER */
	dmg_assembler_generator_evaluate_expression_scalar, /* TOKEN_SCALAR */
	NULL, /* TOKEN_SYMBOL */
	};

static int
dmg_assembler_generator_evaluate_expression(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__inout dmg_assembler_scalar_t *value,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_evaluator_hdlr handler;

	if((tree->token->type >= TOKEN_MAX)
			|| !(handler = EXPRESSION_HANDLER[tree->token->type])) {
		result = GENERATOR_ERROR(parser, tree, "Unsupported expression");
		goto exit;
	}

	if((result = handler(generator, parser, tree, value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generator_generate_directive_bank(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;
	dmg_assembler_scalar_t origin = {}, value = {};

	if(tree->token->subtype != DIRECTIVE_BANK) {
		result = GENERATOR_ERROR(parser, tree, "Expecting directive");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generator_bank_set(generator, value.word, &origin, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generator_generate_directive_data_byte(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != DIRECTIVE_DATA_BYTE) {
		result = GENERATOR_ERROR(parser, tree, "Expecting directive");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	for(uint32_t index = 0; index < tree->count; ++index) {
		dmg_assembler_scalar_t value = {};
		dmg_assembler_tree_t *child = NULL;

		if((result = dmg_assembler_tree_child(&parser->trees, tree, index, &child)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_generator_bank_set_byte(generator, &value, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

exit:
	return result;
}

static int
dmg_assembler_generator_generate_directive_data_word(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != DIRECTIVE_DATA_WORD) {
		result = GENERATOR_ERROR(parser, tree, "Expecting directive");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	for(uint32_t index = 0; index < tree->count; ++index) {
		dmg_assembler_scalar_t value = {};
		dmg_assembler_tree_t *child = NULL;

		if((result = dmg_assembler_tree_child(&parser->trees, tree, index, &child)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_generator_bank_set_word(generator, &value, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

exit:
	return result;
}

static int
dmg_assembler_generator_generate_directive_define(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	const dmg_assembler_token_t *token = NULL;
	int index = 0, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != DIRECTIVE_DEFINE) {
		result = GENERATOR_ERROR(parser, tree, "Expecting directive");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, index++, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	token = child->token;

	if((result = dmg_assembler_tree_child(&parser->trees, tree, index, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_constant_add(&generator->defines, token, &value, false)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generator_generate_directive_else(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int index, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != DIRECTIVE_ELSE) {
		result = GENERATOR_ERROR(parser, tree, "Expecting directive");
		goto exit;
	}

	for(index = 0; index < tree->count; ++index) {
		dmg_assembler_tree_t *child = NULL;

		if((result = dmg_assembler_tree_child(&parser->trees, tree, index, &child)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_generator_generate_statement(generator, parser, child, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

exit:
	return result;
}

static int
dmg_assembler_generator_generate_directive_else_if(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int index, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != DIRECTIVE_ELSE_IF) {
		result = GENERATOR_ERROR(parser, tree, "Expecting directive");
		goto exit;
	}

	for(index = 1; index < tree->count; ++index) {
		dmg_assembler_tree_t *child = NULL;

		if((result = dmg_assembler_tree_child(&parser->trees, tree, index, &child)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_generator_generate_statement(generator, parser, child, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

exit:
	return result;
}

static int
dmg_assembler_generator_generate_directive_if(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	bool condition = false;
	dmg_assembler_tree_t *child = NULL;
	int index, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != DIRECTIVE_IF) {
		result = GENERATOR_ERROR(parser, tree, "Expecting directive");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting conditional");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type == TOKEN_INEQUALITY) {

		if((result = dmg_assembler_generator_evaluate_conditional(generator, parser, child, &condition, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		dmg_assembler_scalar_t value = {};

		if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		condition = (value.word != 0);
	}

	if(condition) {

		for(index = 1; index < tree->count; ++index) {

			if((result = dmg_assembler_tree_child(&parser->trees, tree, index, &child)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			if((child->token->type == TOKEN_DIRECTIVE)
					&& ((child->token->subtype == DIRECTIVE_ELSE_IF)
						|| (child->token->subtype == DIRECTIVE_ELSE))) {
				break;
			}

			if((result = dmg_assembler_generator_generate_statement(generator, parser, child, first_pass)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}
	} else {

		for(index = 1; index < tree->count; ++index) {

			if((result = dmg_assembler_tree_child(&parser->trees, tree, index, &child)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			if((child->token->type == TOKEN_DIRECTIVE)
					&& ((child->token->subtype == DIRECTIVE_ELSE_IF)
						|| (child->token->subtype == DIRECTIVE_ELSE))) {
				break;
			}
		}

		if(index < tree->count) {

			for(; index < tree->count; ++index) {

				if((result = dmg_assembler_tree_child(&parser->trees, tree, index, &child)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}

				if(child->token->subtype == DIRECTIVE_ELSE_IF) {
					dmg_assembler_tree_t *else_child = NULL;

					if(!child->count) {
						result = GENERATOR_ERROR(parser, tree, "Expecting conditional");
						goto exit;
					}

					if((result = dmg_assembler_tree_child(&parser->trees, child, 0, &else_child))
							!= DMG_STATUS_SUCCESS) {
						goto exit;
					}

					if(else_child->token->type == TOKEN_INEQUALITY) {

						if((result = dmg_assembler_generator_evaluate_conditional(generator, parser, else_child, &condition, first_pass))
								!= DMG_STATUS_SUCCESS) {
							goto exit;
						}
					} else {
						dmg_assembler_scalar_t value = {};

						if((result = dmg_assembler_generator_evaluate_expression(generator, parser, else_child, &value, first_pass))
								!= DMG_STATUS_SUCCESS) {
							goto exit;
						}

						condition = (value.word != 0);
					}

					if(condition) {

						if((result = dmg_assembler_generator_generate_directive_else_if(generator, parser, child, first_pass))
								!= DMG_STATUS_SUCCESS) {
							goto exit;
						}

						break;
					}
				} else if(child->token->subtype == DIRECTIVE_ELSE) {

					if((result = dmg_assembler_generator_generate_directive_else(generator, parser, child, first_pass))
							!= DMG_STATUS_SUCCESS) {
						goto exit;
					}

					break;
				}
			}
		}
	}

exit:
	return result;
}

static int
dmg_assembler_generator_generate_directive_if_define(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	bool condition = false;
	dmg_assembler_tree_t *child = NULL;
	int index, result = DMG_STATUS_SUCCESS;

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting conditional");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	switch(tree->token->subtype) {
		case DIRECTIVE_IF_DEFINE:
			condition = dmg_assembler_constant_defined(&generator->defines, child->token);
			break;
		case DIRECTIVE_IF_NOT_DEFINE:
			condition = !dmg_assembler_constant_defined(&generator->defines, child->token);
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Expecting directive");
			goto exit;
	}

	if(condition) {

		for(index = 1; index < tree->count; ++index) {

			if((result = dmg_assembler_tree_child(&parser->trees, tree, index, &child)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			if((child->token->type == TOKEN_DIRECTIVE)
					&& (child->token->subtype == DIRECTIVE_ELSE)) {
				break;
			}

			if((result = dmg_assembler_generator_generate_statement(generator, parser, child, first_pass)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}
	} else {

		for(index = 1; index < tree->count; ++index) {

			if((result = dmg_assembler_tree_child(&parser->trees, tree, index, &child)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			if((child->token->type == TOKEN_DIRECTIVE)
					&& (child->token->subtype == DIRECTIVE_ELSE)) {
				break;
			}
		}

		if(index < tree->count) {

			if((result = dmg_assembler_generator_generate_directive_else(generator, parser, child, first_pass)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}
	}

exit:
	return result;
}

static int
dmg_assembler_generator_generate_directive_include(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	FILE *file = NULL;
	dmg_buffer_t buffer = {};
	dmg_assembler_string_t path = {};
	dmg_assembler_tree_t *child = NULL;
	const dmg_assembler_tree_t *child_tree;
	dmg_assembler_parser_t child_parser = {};
	int index, length = 0, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != DIRECTIVE_INCLUDE) {
		result = GENERATOR_ERROR(parser, tree, "Expecting directive");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting literal");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(!child->token->literal.length) {
		result = GENERATOR_ERROR(parser, tree, "Expecting file path");
		goto exit;
	}

	if((result = dmg_assembler_string_allocate(&path)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_string_append_string(&path, generator->root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_string_append_character(&path, PATH_DELIMITER)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	for(index = 0; index < child->token->literal.length; ++index) {

		if((result = dmg_assembler_string_append_character(&path, child->token->literal.str[index]))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	if((result = dmg_assembler_string_append_character(&path, CHARACTER_EOF)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_tool_file_open(path.str, true, true, &file, &length)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_buffer_allocate(&buffer, length, 0)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(fread(buffer.data, sizeof(uint8_t), length, file) != length) {
		result = GENERATOR_ERROR(parser, tree, "Failed to read file");
		goto exit;
	}

	if((result = dmg_assembler_parser_load(&child_parser, &buffer, path.str)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	for(;;) {

		if(!(child_tree = dmg_assembler_parser_tree(&child_parser))) {
			break;
		}

		if((result = dmg_assembler_generator_generate_statement(generator, &child_parser, child_tree, true)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(!dmg_assembler_parser_has_next(&child_parser)
				|| ((result = dmg_assembler_parser_next(&child_parser)) != DMG_STATUS_SUCCESS)) {
			break;
		}
	}

	dmg_assembler_parser_unload(&child_parser);

	if((result = dmg_assembler_parser_load(&child_parser, &buffer, path.str)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	dmg_assembler_constants_free(&generator->defines);

	if((result = dmg_assembler_constants_allocate(&generator->defines)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	generator->bank = 0;
	generator->offset.word = 0;

	for(;;) {

		if(!(child_tree = dmg_assembler_parser_tree(&child_parser))) {
			break;
		}

		if((result = dmg_assembler_generator_generate_statement(generator, &child_parser, child_tree, false)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(!dmg_assembler_parser_has_next(&child_parser)
				|| ((result = dmg_assembler_parser_next(&child_parser)) != DMG_STATUS_SUCCESS)) {
			break;
		}
	}

exit:
	dmg_assembler_parser_unload(&child_parser);
	dmg_buffer_free(&buffer);
	dmg_tool_file_close(&file);
	dmg_assembler_string_free(&path);

	return result;
}

static int
dmg_assembler_generator_generate_directive_include_binary(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	FILE *file = NULL;
	dmg_assembler_string_t path = {};
	dmg_assembler_tree_t *child = NULL;
	int index, length = 0, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != DIRECTIVE_INCLUDE_BINARY) {
		result = GENERATOR_ERROR(parser, tree, "Expecting directive");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting literal");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(!child->token->literal.length) {
		result = GENERATOR_ERROR(parser, tree, "Expecting file path");
		goto exit;
	}

	if((result = dmg_assembler_string_allocate(&path)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_string_append_string(&path, generator->root)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_string_append_character(&path, PATH_DELIMITER)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	for(index = 0; index < child->token->literal.length; ++index) {

		if((result = dmg_assembler_string_append_character(&path, child->token->literal.str[index]))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	if((result = dmg_assembler_string_append_character(&path, CHARACTER_EOF)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_tool_file_open(path.str, true, true, &file, &length)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(length > (BANK_WIDTH - generator->offset.word)) {
		result = GENERATOR_ERROR(parser, tree, "Binary file exceeds remaining bank size");
		goto exit;
	}

	for(index = 0; index < length; ++index) {
		dmg_assembler_scalar_t value = {};

		if(fread(&value.low, sizeof(uint8_t), sizeof(uint8_t), file) != sizeof(uint8_t)) {
			result = GENERATOR_ERROR(parser, tree, "Failed to read file");
			goto exit;
		}

		if((result = dmg_assembler_generator_bank_set_byte(generator, &value, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

exit:
	dmg_tool_file_close(&file);
	dmg_assembler_string_free(&path);

	return result;
}

static int
dmg_assembler_generator_generate_directive_origin(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;

	if(tree->token->subtype != DIRECTIVE_ORIGIN) {
		result = GENERATOR_ERROR(parser, tree, "Expecting directive");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(value.word > ORIGIN_MAX) {
		result = GENERATOR_ERROR(parser, tree, "Origin out-of-range");
		goto exit;
	}

	generator->banks.bank[generator->bank].origin.word = (BANK_WIDTH * (value.word / BANK_WIDTH));
	generator->offset.word = (value.word % BANK_WIDTH);

exit:
	return result;
}

static int
dmg_assembler_generator_generate_directive_reserve(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;
	dmg_assembler_scalar_t length = {}, value = {};

	if(tree->token->subtype != DIRECTIVE_RESERVE) {
		result = GENERATOR_ERROR(parser, tree, "Expecting directive");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &length, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(length.word > (BANK_WIDTH - generator->offset.word)) {
		result = GENERATOR_ERROR(parser, tree, "Reserve out-of-range");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 1, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	for(uint32_t index = 0; index < length.word; ++index) {

		if((result = dmg_assembler_generator_bank_set_byte(generator, &value, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

exit:
	return result;
}

static int
dmg_assembler_generator_generate_directive_undefine(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child = NULL;

	if(tree->token->subtype != DIRECTIVE_UNDEFINE) {
		result = GENERATOR_ERROR(parser, tree, "Expecting directive");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_constant_remove(&generator->defines, child->token)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static dmg_assembler_generator_hdlr DIRECTIVE_HANDLER[] = {
	dmg_assembler_generator_generate_directive_bank, /* DIRECTIVE_BANK */
	dmg_assembler_generator_generate_directive_data_byte, /* DIRECTIVE_DATA_BYTE */
	dmg_assembler_generator_generate_directive_data_word, /* DIRECTIVE_DATA_WORD */
	dmg_assembler_generator_generate_directive_define, /* DIRECTIVE_DEFINE */
	NULL, /* DIRECTIVE_ELSE_IF */
	NULL, /* DIRECTIVE_ELSE */
	NULL, /* DIRECTIVE_END */
	dmg_assembler_generator_generate_directive_if, /* DIRECTIVE_IF */
	dmg_assembler_generator_generate_directive_if_define, /* DIRECTIVE_IF_DEFINE */
	dmg_assembler_generator_generate_directive_if_define, /* DIRECTIVE_IF_NOT_DEFINE */
	dmg_assembler_generator_generate_directive_include, /* DIRECTIVE_INCLUDE */
	dmg_assembler_generator_generate_directive_include_binary, /* DIRECTIVE_INCLUDE_BINARY */
	dmg_assembler_generator_generate_directive_origin, /* DIRECTIVE_ORIGIN */
	dmg_assembler_generator_generate_directive_reserve, /* DIRECTIVE_RESERVE */
	dmg_assembler_generator_generate_directive_undefine, /* DIRECTIVE_UNDEFINE */
	};

static int
dmg_assembler_generator_generate_directive(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_generator_hdlr handler;

	if((tree->token->subtype >= DIRECTIVE_MAX)
			|| !(handler = DIRECTIVE_HANDLER[tree->token->subtype])) {
		result = GENERATOR_ERROR(parser, tree, "Unsupported directive");
		goto exit;
	}

	if((result = handler(generator, parser, tree, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generator_generate_label(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->token->type != TOKEN_LABEL) {
		result = GENERATOR_ERROR(parser, tree, "Expecting label");
		goto exit;
	}

	if(first_pass) {

		if(dmg_assembler_constant_get(&generator->labels, tree->token, &value) == DMG_STATUS_SUCCESS) {
			result = GENERATOR_ERROR(parser, tree, "Duplicate label");
			goto exit;
		}

		value.word = (generator->banks.bank[generator->bank].origin.word + generator->offset.word);

		if((result = dmg_assembler_constant_add(&generator->labels, tree->token, &value, false)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

exit:
	return result;
}

static int
dmg_assembler_generate_instruction(
	__inout dmg_assembler_generator_t *generator,
	__in int instruction,
	__in bool extended,
	__in const dmg_assembler_scalar_t *operand,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(extended) {
		value.low = INSTRUCTION_EXTENDED_PREFIX;
		value.high = instruction;

		if((result = dmg_assembler_generator_bank_set_word(generator, &value, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		value.low = instruction;

		if((result = dmg_assembler_generator_bank_set_byte(generator, &value, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	switch(dmg_processor_instruction(instruction, extended)->operand) {
		case OPERAND_WORD:

			if((result = dmg_assembler_generator_bank_set_word(generator, operand, first_pass)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		case OPERAND_BYTE:

			if((result = dmg_assembler_generator_bank_set_byte(generator, operand, first_pass)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
			break;
		default:
			break;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_adc(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = INSTRUCTION_ADC_U8, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_ADC) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type == TOKEN_REGISTER) {

		switch(child->token->subtype) {
			case REGISTER_A:
				instruction = INSTRUCTION_ADC_A;
				break;
			case REGISTER_B:
				instruction = INSTRUCTION_ADC_B;
				break;
			case REGISTER_C:
				instruction = INSTRUCTION_ADC_C;
				break;
			case REGISTER_D:
				instruction = INSTRUCTION_ADC_D;
				break;
			case REGISTER_E:
				instruction = INSTRUCTION_ADC_E;
				break;
			case REGISTER_H:
				instruction = INSTRUCTION_ADC_H;
				break;
			case REGISTER_HL:
				instruction = INSTRUCTION_ADC_HL_IND;
				break;
			case REGISTER_L:
				instruction = INSTRUCTION_ADC_L;
				break;
			default:
				result = GENERATOR_ERROR(parser, tree, "Unsupported register");
				goto exit;
		}
	} else if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_add(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = INSTRUCTION_ADD_U8, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_ADD) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type == TOKEN_REGISTER) {

		switch(child->token->subtype) {
			case REGISTER_A:
				instruction = INSTRUCTION_ADD_A;
				break;
			case REGISTER_B:
				instruction = INSTRUCTION_ADD_B;
				break;
			case REGISTER_C:
				instruction = INSTRUCTION_ADD_C;
				break;
			case REGISTER_D:
				instruction = INSTRUCTION_ADD_D;
				break;
			case REGISTER_E:
				instruction = INSTRUCTION_ADD_E;
				break;
			case REGISTER_H:
				instruction = INSTRUCTION_ADD_H;
				break;
			case REGISTER_HL:

				if(tree->count > 1) {

					if((result = dmg_assembler_tree_child(&parser->trees, tree, 1, &child)) != DMG_STATUS_SUCCESS) {
						goto exit;
					}

					if(child->token->type != TOKEN_REGISTER) {
						result = GENERATOR_ERROR(parser, tree, "Expecting register");
						goto exit;
					}

					switch(child->token->subtype) {
						case REGISTER_BC:
							instruction = INSTRUCTION_ADD_HL_BC;
							break;
						case REGISTER_DE:
							instruction = INSTRUCTION_ADD_HL_DE;
							break;
						case REGISTER_HL:
							instruction = INSTRUCTION_ADD_HL_HL;
							break;
						case REGISTER_SP:
							instruction = INSTRUCTION_ADD_HL_SP;
							break;
						default:
							result = GENERATOR_ERROR(parser, tree, "Unsupported register");
							goto exit;
					}
				} else {
					instruction = INSTRUCTION_ADD_HL_IND;
				}
				break;
			case REGISTER_L:
				instruction = INSTRUCTION_ADD_L;
				break;
			case REGISTER_SP:
				instruction = INSTRUCTION_ADD_SP_I8;

				if(tree->count < 1) {
					result = GENERATOR_ERROR(parser, tree, "Expecting expression");
					goto exit;
				}

				if((result = dmg_assembler_tree_child(&parser->trees, tree, 1, &child)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}

				if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
				break;
			default:
				result = GENERATOR_ERROR(parser, tree, "Unsupported register");
				goto exit;
		}
	} else if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_and(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = INSTRUCTION_AND_U8, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_AND) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type == TOKEN_REGISTER) {

		switch(child->token->subtype) {
			case REGISTER_A:
				instruction = INSTRUCTION_AND_A;
				break;
			case REGISTER_B:
				instruction = INSTRUCTION_AND_B;
				break;
			case REGISTER_C:
				instruction = INSTRUCTION_AND_C;
				break;
			case REGISTER_D:
				instruction = INSTRUCTION_AND_D;
				break;
			case REGISTER_E:
				instruction = INSTRUCTION_AND_E;
				break;
			case REGISTER_H:
				instruction = INSTRUCTION_AND_H;
				break;
			case REGISTER_HL:
				instruction = INSTRUCTION_AND_HL_IND;
				break;
			case REGISTER_L:
				instruction = INSTRUCTION_AND_L;
				break;
			default:
				result = GENERATOR_ERROR(parser, tree, "Unsupported register");
				goto exit;
		}
	} else if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_bit(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS;

	if((tree->token->subtype < OPCODE_BIT0)
			|| (tree->token->subtype > OPCODE_BIT7)) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type != TOKEN_REGISTER) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	switch(child->token->subtype) {
		case REGISTER_A:
			instruction = INSTRUCTION_EXTENDED_BIT_0_A;
			break;
		case REGISTER_B:
			instruction = INSTRUCTION_EXTENDED_BIT_0_B;
			break;
		case REGISTER_C:
			instruction = INSTRUCTION_EXTENDED_BIT_0_C;
			break;
		case REGISTER_D:
			instruction = INSTRUCTION_EXTENDED_BIT_0_D;
			break;
		case REGISTER_E:
			instruction = INSTRUCTION_EXTENDED_BIT_0_E;
			break;
		case REGISTER_H:
			instruction = INSTRUCTION_EXTENDED_BIT_0_H;
			break;
		case REGISTER_HL:
			instruction = INSTRUCTION_EXTENDED_BIT_0_HL_IND;
			break;
		case REGISTER_L:
			instruction = INSTRUCTION_EXTENDED_BIT_0_H;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported register");
			goto exit;
	}

	instruction += ((tree->token->subtype - OPCODE_BIT0) * CHAR_BIT);

	if((result = dmg_assembler_generate_instruction(generator, instruction, true, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_call(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int condition = CONDITION_MAX, instruction = INSTRUCTION_CALL_U16, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_CALL) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting condition");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type == TOKEN_CONDITION) {
		condition = child->token->subtype;

		if(tree->count < 2) {
			result = GENERATOR_ERROR(parser, tree, "Expecting expression");
			goto exit;
		}

		if((result = dmg_assembler_tree_child(&parser->trees, tree, 1, &child)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(condition < CONDITION_MAX) {

		switch(condition) {
			case CONDITION_CARRY:
				instruction = INSTRUCTION_CALL_C_U16;
				break;
			case CONDITION_CARRY_NOT:
				instruction = INSTRUCTION_CALL_NC_U16;
				break;
			case CONDITION_ZERO:
				instruction = INSTRUCTION_CALL_Z_U16;
				break;
			case CONDITION_ZERO_NOT:
				instruction = INSTRUCTION_CALL_NZ_U16;
				break;
			default:
				result = GENERATOR_ERROR(parser, tree, "Unsupported condition");
				goto exit;
		}
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_ccf(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->token->subtype != OPCODE_CCF) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_CCF, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_cp(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = INSTRUCTION_CP_U8, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_CP) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type == TOKEN_REGISTER) {

		switch(child->token->subtype) {
			case REGISTER_A:
				instruction = INSTRUCTION_CP_A;
				break;
			case REGISTER_B:
				instruction = INSTRUCTION_CP_B;
				break;
			case REGISTER_C:
				instruction = INSTRUCTION_CP_C;
				break;
			case REGISTER_D:
				instruction = INSTRUCTION_CP_D;
				break;
			case REGISTER_E:
				instruction = INSTRUCTION_CP_E;
				break;
			case REGISTER_H:
				instruction = INSTRUCTION_CP_H;
				break;
			case REGISTER_HL:
				instruction = INSTRUCTION_CP_HL_IND;
				break;
			case REGISTER_L:
				instruction = INSTRUCTION_CP_L;
				break;
			default:
				result = GENERATOR_ERROR(parser, tree, "Unsupported register");
				goto exit;
		}
	} else if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_cpl(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->token->subtype != OPCODE_CPL) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_CPL, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_daa(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->token->subtype != OPCODE_DAA) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_DAA, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_di(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->token->subtype != OPCODE_DI) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_DI, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_dec(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS, subtype = tree->token->subtype;

	if(subtype != OPCODE_DEC) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type != TOKEN_REGISTER) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	switch(child->token->subtype) {
		case REGISTER_A:
			instruction = INSTRUCTION_DEC_A;
			break;
		case REGISTER_B:
			instruction = INSTRUCTION_DEC_B;
			break;
		case REGISTER_BC:
			instruction = INSTRUCTION_DEC_BC;
			break;
		case REGISTER_C:
			instruction = INSTRUCTION_DEC_C;
			break;
		case REGISTER_D:
			instruction = INSTRUCTION_DEC_D;
			break;
		case REGISTER_DE:
			instruction = INSTRUCTION_DEC_DE;
			break;
		case REGISTER_E:
			instruction = INSTRUCTION_DEC_E;
			break;
		case REGISTER_H:
			instruction = INSTRUCTION_DEC_H;
			break;
		case REGISTER_HL:
			instruction = (child->token->indirect ? INSTRUCTION_DEC_HL_IND : INSTRUCTION_DEC_HL);
			break;
		case REGISTER_L:
			instruction = INSTRUCTION_DEC_L;
			break;
		case REGISTER_SP:
			instruction = INSTRUCTION_DEC_SP;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported register");
			goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_ei(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->token->subtype != OPCODE_EI) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_EI, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_halt(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->token->subtype != OPCODE_HALT) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_HALT, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_inc(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS, subtype = tree->token->subtype;

	if(subtype != OPCODE_INC) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type != TOKEN_REGISTER) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	switch(child->token->subtype) {
		case REGISTER_A:
			instruction = INSTRUCTION_INC_A;
			break;
		case REGISTER_B:
			instruction = INSTRUCTION_INC_B;
			break;
		case REGISTER_BC:
			instruction = INSTRUCTION_INC_BC;
			break;
		case REGISTER_C:
			instruction = INSTRUCTION_INC_C;
			break;
		case REGISTER_D:
			instruction = INSTRUCTION_INC_D;
			break;
		case REGISTER_DE:
			instruction = INSTRUCTION_INC_DE;
			break;
		case REGISTER_E:
			instruction = INSTRUCTION_INC_E;
			break;
		case REGISTER_H:
			instruction = INSTRUCTION_INC_H;
			break;
		case REGISTER_HL:
			instruction = (child->token->indirect ? INSTRUCTION_INC_HL_IND : INSTRUCTION_INC_HL);
			break;
		case REGISTER_L:
			instruction = INSTRUCTION_INC_L;
			break;
		case REGISTER_SP:
			instruction = INSTRUCTION_INC_SP;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported register");
			goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_jp(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int condition = CONDITION_MAX, instruction = INSTRUCTION_JP_U16, reg = REGISTER_MAX, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_JP) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting condition");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type == TOKEN_CONDITION) {
		condition = child->token->subtype;

		if(tree->count < 2) {
			result = GENERATOR_ERROR(parser, tree, "Expecting expression");
			goto exit;
		}

		if((result = dmg_assembler_tree_child(&parser->trees, tree, 1, &child)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if(child->token->type == TOKEN_REGISTER) {
		reg = child->token->subtype;
	} else if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(condition < CONDITION_MAX) {

		switch(condition) {
			case CONDITION_CARRY:
				instruction = INSTRUCTION_JP_C_U16;
				break;
			case CONDITION_CARRY_NOT:
				instruction = INSTRUCTION_JP_NC_U16;
				break;
			case CONDITION_ZERO:
				instruction = INSTRUCTION_JP_Z_U16;
				break;
			case CONDITION_ZERO_NOT:
				instruction = INSTRUCTION_JP_NZ_U16;
				break;
			default:
				result = GENERATOR_ERROR(parser, tree, "Unsupported condition");
				goto exit;
		}
	} else if(reg < REGISTER_MAX) {

		switch(reg) {
			case REGISTER_HL:
				instruction = INSTRUCTION_JP_HL;
				break;
			default:
				result = GENERATOR_ERROR(parser, tree, "Unsupported register");
				goto exit;
		}
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_jr(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int condition = CONDITION_MAX, instruction = INSTRUCTION_JR_I8, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_JR) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting condition");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type == TOKEN_CONDITION) {
		condition = child->token->subtype;

		if(tree->count < 2) {
			result = GENERATOR_ERROR(parser, tree, "Expecting expression");
			goto exit;
		}

		if((result = dmg_assembler_tree_child(&parser->trees, tree, 1, &child)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	value.low -= (generator->offset.low + OPERAND_WORD);

	if(condition < CONDITION_MAX) {

		switch(condition) {
			case CONDITION_CARRY:
				instruction = INSTRUCTION_JR_C_I8;
				break;
			case CONDITION_CARRY_NOT:
				instruction = INSTRUCTION_JR_NC_I8;
				break;
			case CONDITION_ZERO:
				instruction = INSTRUCTION_JR_Z_I8;
				break;
			case CONDITION_ZERO_NOT:
				instruction = INSTRUCTION_JR_NZ_I8;
				break;
			default:
				result = GENERATOR_ERROR(parser, tree, "Unsupported condition");
				goto exit;
		}
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_ld(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int instruction = 0, result = DMG_STATUS_SUCCESS;
	dmg_assembler_tree_t *child_left = NULL, *child_right = NULL;
	dmg_assembler_scalar_t value = {}, value_left = {}, value_right = {};

	if(tree->token->subtype != OPCODE_LD) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count < 2) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child_left)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((child_left->token->type != TOKEN_REGISTER)
			&& ((result = dmg_assembler_generator_evaluate_expression(generator, parser, child_left, &value_left, first_pass)) != DMG_STATUS_SUCCESS)) {
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 1, &child_right)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((child_right->token->type != TOKEN_REGISTER)
			&& ((result = dmg_assembler_generator_evaluate_expression(generator, parser, child_right, &value_right, first_pass)) != DMG_STATUS_SUCCESS)) {
		goto exit;
	}

	if(!tree->token->indirect) {

		if(child_left->token->type != TOKEN_REGISTER) {
			result = GENERATOR_ERROR(parser, tree, "Expecting register");
			goto exit;
		}

		switch(child_left->token->subtype) {
			case REGISTER_A:

				if(child_right->token->type == TOKEN_REGISTER) {

					switch(child_right->token->subtype) {
						case REGISTER_A:
							instruction = INSTRUCTION_LD_A_A;
							break;
						case REGISTER_B:
							instruction = INSTRUCTION_LD_A_B;
							break;
						case REGISTER_BC:
							instruction = INSTRUCTION_LD_A_BC_IND;
							break;
						case REGISTER_C:
							instruction = (child_right->token->indirect ? INSTRUCTION_LD_A_FF00_C_IND : INSTRUCTION_LD_A_C);
							break;
						case REGISTER_D:
							instruction = INSTRUCTION_LD_A_D;
							break;
						case REGISTER_DE:
							instruction = INSTRUCTION_LD_A_DE_IND;
							break;
						case REGISTER_E:
							instruction = INSTRUCTION_LD_A_E;
							break;
						case REGISTER_H:
							instruction = INSTRUCTION_LD_A_H;
							break;
						case REGISTER_HL:

							if(child_right->count) {

								if((result = dmg_assembler_tree_child(&parser->trees, child_right, 0, &child_left))
										!= DMG_STATUS_SUCCESS) {
									goto exit;
								}

								if(child_left->token->type != TOKEN_OPERATOR) {
									result = GENERATOR_ERROR(parser, tree, "Expecting operator");
									goto exit;
								}

								switch(child_left->token->subtype) {
									case OPERATOR_ARITHMETIC_ADD:
										instruction = INSTRUCTION_LD_A_HL_IND_INC;
										break;
									case OPERATOR_ARITHMETIC_SUBTRACT:
										instruction = INSTRUCTION_LD_A_HL_IND_DEC;
										break;
									default:
										result = GENERATOR_ERROR(parser, tree, "Unsupported operator");
										goto exit;
										break;
								}
							} else {
								instruction = INSTRUCTION_LD_A_HL_IND;
							}
							break;
						case REGISTER_L:
							instruction = INSTRUCTION_LD_A_L;
							break;
						default:
							result = GENERATOR_ERROR(parser, tree, "Unsupported register");
							goto exit;
					}
				} else {
					instruction = INSTRUCTION_LD_A_U8;
					value.word = value_right.low;
				}
				break;
			case REGISTER_B:

				if(child_right->token->type == TOKEN_REGISTER) {

					switch(child_right->token->subtype) {
						case REGISTER_A:
							instruction = INSTRUCTION_LD_B_A;
							break;
						case REGISTER_B:
							instruction = INSTRUCTION_LD_B_B;
							break;
						case REGISTER_C:
							instruction = INSTRUCTION_LD_B_C;
							break;
						case REGISTER_D:
							instruction = INSTRUCTION_LD_B_D;
							break;
						case REGISTER_E:
							instruction = INSTRUCTION_LD_B_E;
							break;
						case REGISTER_H:
							instruction = INSTRUCTION_LD_B_H;
							break;
						case REGISTER_HL:
							instruction = INSTRUCTION_LD_B_HL_IND;
							break;
						case REGISTER_L:
							instruction = INSTRUCTION_LD_B_L;
							break;
						default:
							result = GENERATOR_ERROR(parser, tree, "Unsupported register");
							goto exit;
					}
				} else {
					instruction = INSTRUCTION_LD_B_U8;
					value.word = value_right.low;
				}
				break;
			case REGISTER_BC:

				if(child_left->token->indirect) {

					if((child_right->token->type != TOKEN_REGISTER)
							&& (child_right->token->subtype != REGISTER_A)) {
						result = GENERATOR_ERROR(parser, tree, "Unsupported register");
						goto exit;
					}

					instruction = INSTRUCTION_LD_BC_IND_A;
				} else {
					instruction = INSTRUCTION_LD_BC_U16;
					value.word = value_right.word;
				}
				break;
			case REGISTER_C:

				if(child_right->token->type == TOKEN_REGISTER) {

					switch(child_right->token->subtype) {
						case REGISTER_A:
							instruction = (child_left->token->indirect ? INSTRUCTION_LD_FF00_C_IND_A : INSTRUCTION_LD_C_A);
							break;
						case REGISTER_B:
							instruction = INSTRUCTION_LD_C_B;
							break;
						case REGISTER_C:
							instruction = INSTRUCTION_LD_C_C;
							break;
						case REGISTER_D:
							instruction = INSTRUCTION_LD_C_D;
							break;
						case REGISTER_E:
							instruction = INSTRUCTION_LD_C_E;
							break;
						case REGISTER_H:
							instruction = INSTRUCTION_LD_C_H;
							break;
						case REGISTER_HL:
							instruction = INSTRUCTION_LD_C_HL_IND;
							break;
						case REGISTER_L:
							instruction = INSTRUCTION_LD_C_L;
							break;
						default:
							result = GENERATOR_ERROR(parser, tree, "Unsupported register");
							goto exit;
					}
				} else {
					instruction = INSTRUCTION_LD_C_U8;
					value.word = value_right.low;
				}
				break;
			case REGISTER_D:

				if(child_right->token->type == TOKEN_REGISTER) {

					switch(child_right->token->subtype) {
						case REGISTER_A:
							instruction = INSTRUCTION_LD_D_A;
							break;
						case REGISTER_B:
							instruction = INSTRUCTION_LD_D_B;
							break;
						case REGISTER_C:
							instruction = INSTRUCTION_LD_D_C;
							break;
						case REGISTER_D:
							instruction = INSTRUCTION_LD_D_D;
							break;
						case REGISTER_E:
							instruction = INSTRUCTION_LD_D_E;
							break;
						case REGISTER_H:
							instruction = INSTRUCTION_LD_D_H;
							break;
						case REGISTER_HL:
							instruction = INSTRUCTION_LD_D_HL_IND;
							break;
						case REGISTER_L:
							instruction = INSTRUCTION_LD_D_L;
							break;
						default:
							result = GENERATOR_ERROR(parser, tree, "Unsupported register");
							goto exit;
					}
				} else {
					instruction = INSTRUCTION_LD_D_U8;
					value.word = value_right.low;
				}
				break;
			case REGISTER_DE:

				if(child_left->token->indirect) {

					if((child_right->token->type != TOKEN_REGISTER)
							&& (child_right->token->subtype != REGISTER_A)) {
						result = GENERATOR_ERROR(parser, tree, "Unsupported register");
						goto exit;
					}

					instruction = INSTRUCTION_LD_DE_IND_A;
				} else {
					instruction = INSTRUCTION_LD_DE_U16;
					value.word = value_right.word;
				}
				break;
			case REGISTER_E:

				if(child_right->token->type == TOKEN_REGISTER) {

					switch(child_right->token->subtype) {
						case REGISTER_A:
							instruction = INSTRUCTION_LD_E_A;
							break;
						case REGISTER_B:
							instruction = INSTRUCTION_LD_E_B;
							break;
						case REGISTER_C:
							instruction = INSTRUCTION_LD_E_C;
							break;
						case REGISTER_D:
							instruction = INSTRUCTION_LD_E_D;
							break;
						case REGISTER_E:
							instruction = INSTRUCTION_LD_E_E;
							break;
						case REGISTER_H:
							instruction = INSTRUCTION_LD_E_H;
							break;
						case REGISTER_HL:
							instruction = INSTRUCTION_LD_E_HL_IND;
							break;
						case REGISTER_L:
							instruction = INSTRUCTION_LD_E_L;
							break;
						default:
							result = GENERATOR_ERROR(parser, tree, "Unsupported register");
							goto exit;
					}
				} else {
					instruction = INSTRUCTION_LD_E_U8;
					value.word = value_right.low;
				}
				break;
			case REGISTER_H:

				if(child_right->token->type == TOKEN_REGISTER) {

					switch(child_right->token->subtype) {
						case REGISTER_A:
							instruction = INSTRUCTION_LD_H_A;
							break;
						case REGISTER_B:
							instruction = INSTRUCTION_LD_H_B;
							break;
						case REGISTER_C:
							instruction = INSTRUCTION_LD_H_C;
							break;
						case REGISTER_D:
							instruction = INSTRUCTION_LD_H_D;
							break;
						case REGISTER_E:
							instruction = INSTRUCTION_LD_H_E;
							break;
						case REGISTER_H:
							instruction = INSTRUCTION_LD_H_H;
							break;
						case REGISTER_HL:
							instruction = INSTRUCTION_LD_H_HL_IND;
							break;
						case REGISTER_L:
							instruction = INSTRUCTION_LD_H_L;
							break;
						default:
							result = GENERATOR_ERROR(parser, tree, "Unsupported register");
							goto exit;
					}
				} else {
					instruction = INSTRUCTION_LD_H_U8;
					value.word = value_right.low;
				}
				break;
			case REGISTER_HL:

				if(child_left->token->indirect) {

					if(child_right->token->type == TOKEN_REGISTER) {

						switch(child_right->token->subtype) {
							case REGISTER_A:

								if(child_left->count) {

									if((result = dmg_assembler_tree_child(&parser->trees, child_left, 0, &child_right))
											!= DMG_STATUS_SUCCESS) {
										goto exit;
									}

									if(child_right->token->type != TOKEN_OPERATOR) {
										result = GENERATOR_ERROR(parser, tree, "Expecting operator");
										goto exit;
									}

									switch(child_right->token->subtype) {
										case OPERATOR_ARITHMETIC_ADD:
											instruction = INSTRUCTION_LD_HL_IND_INC_A;
											break;
										case OPERATOR_ARITHMETIC_SUBTRACT:
											instruction = INSTRUCTION_LD_HL_IND_DEC_A;
											break;
										default:
											result = GENERATOR_ERROR(parser, tree, "Unsupported operator");
											goto exit;
											break;
									}
								} else {
									instruction = INSTRUCTION_LD_HL_IND_A;
								}
								break;
							case REGISTER_B:
								instruction = INSTRUCTION_LD_HL_IND_B;
								break;
							case REGISTER_C:
								instruction = INSTRUCTION_LD_HL_IND_C;
								break;
							case REGISTER_D:
								instruction = INSTRUCTION_LD_HL_IND_D;
								break;
							case REGISTER_E:
								instruction = INSTRUCTION_LD_HL_IND_E;
								break;
							case REGISTER_H:
								instruction = INSTRUCTION_LD_HL_IND_H;
								break;
							case REGISTER_L:
								instruction = INSTRUCTION_LD_HL_IND_L;
								break;
							default:
								result = GENERATOR_ERROR(parser, tree, "Unsupported register");
								goto exit;
						}
					} else {
						instruction = INSTRUCTION_LD_HL_IND_U8;
						value.word = value_right.low;
					}
				} else {

					if(child_right->token->type == TOKEN_REGISTER) {

						switch(child_right->token->subtype) {
							case REGISTER_SP:

								if((result = dmg_assembler_tree_child(&parser->trees, child_right, 0, &child_left))
										!= DMG_STATUS_SUCCESS) {
									goto exit;
								}

								if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child_left,
										&value_left, first_pass)) != DMG_STATUS_SUCCESS) {
									goto exit;
								}

								instruction = INSTRUCTION_LD_HL_SP_I8;
								value.word = value_left.low;
								break;
							default:
								result = GENERATOR_ERROR(parser, tree, "Unsupported register");
								goto exit;
						}
					} else {
						instruction = INSTRUCTION_LD_HL_U16;
						value.word = value_right.word;
					}
				}
				break;
			case REGISTER_L:

				if(child_right->token->type == TOKEN_REGISTER) {

					switch(child_right->token->subtype) {
						case REGISTER_A:
							instruction = INSTRUCTION_LD_L_A;
							break;
						case REGISTER_B:
							instruction = INSTRUCTION_LD_L_B;
							break;
						case REGISTER_C:
							instruction = INSTRUCTION_LD_L_C;
							break;
						case REGISTER_D:
							instruction = INSTRUCTION_LD_L_D;
							break;
						case REGISTER_E:
							instruction = INSTRUCTION_LD_L_E;
							break;
						case REGISTER_H:
							instruction = INSTRUCTION_LD_L_H;
							break;
						case REGISTER_HL:
							instruction = INSTRUCTION_LD_L_HL_IND;
							break;
						case REGISTER_L:
							instruction = INSTRUCTION_LD_L_L;
							break;
						default:
							result = GENERATOR_ERROR(parser, tree, "Unsupported register");
							goto exit;
					}
				} else {
					instruction = INSTRUCTION_LD_L_U8;
					value.word = value_right.low;
				}
				break;
			case REGISTER_SP:

				if(child_right->token->type == TOKEN_REGISTER) {

					switch(child_right->token->subtype) {
						case REGISTER_HL:
							instruction = INSTRUCTION_LD_SP_HL;
							break;
						default:
							result = GENERATOR_ERROR(parser, tree, "Unsupported register");
							goto exit;
					}
				} else {
					instruction = INSTRUCTION_LD_SP_U16;
					value.word = value_right.word;
				}
				break;
			default:
				result = GENERATOR_ERROR(parser, tree, "Unsupported register");
				goto exit;
		}
	} else {

		if(child_left->token->type == TOKEN_REGISTER) {

			switch(child_left->token->subtype) {
				case REGISTER_A:

					if(value_right.word >= ADDRESS_IO_BASE) {
						instruction = INSTRUCTION_LD_A_FF00_U8_IND;
						value.word = (value_right.word - ADDRESS_IO_BASE);
					} else {
						instruction = INSTRUCTION_LD_A_U16_IND;
						value.word = value_right.word;
					}
					break;
				default:
					result = GENERATOR_ERROR(parser, tree, "Unsupported register");
					goto exit;
			}
		} else if(child_right->token->type == TOKEN_REGISTER) {

			switch(child_right->token->subtype) {
				case REGISTER_A:

					if(value_left.word >= ADDRESS_IO_BASE) {
						instruction = INSTRUCTION_LD_FF00_U8_IND_A;
						value.word = (value_left.word - ADDRESS_IO_BASE);
					} else {
						instruction = INSTRUCTION_LD_U16_IND_A;
						value.word = value_left.word;
					}
					break;
				case REGISTER_SP:
					instruction = INSTRUCTION_LD_U16_IND_SP;
					value.word = value_left.word;
					break;
				default:
					result = GENERATOR_ERROR(parser, tree, "Unsupported register");
					goto exit;
			}
		} else {
			result = GENERATOR_ERROR(parser, tree, "Expecting register");
			goto exit;
		}
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_nop(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->token->subtype != OPCODE_NOP) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_NOP, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_or(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = INSTRUCTION_OR_U8, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_OR) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type == TOKEN_REGISTER) {

		switch(child->token->subtype) {
			case REGISTER_A:
				instruction = INSTRUCTION_OR_A;
				break;
			case REGISTER_B:
				instruction = INSTRUCTION_OR_B;
				break;
			case REGISTER_C:
				instruction = INSTRUCTION_OR_C;
				break;
			case REGISTER_D:
				instruction = INSTRUCTION_OR_D;
				break;
			case REGISTER_E:
				instruction = INSTRUCTION_OR_E;
				break;
			case REGISTER_H:
				instruction = INSTRUCTION_OR_H;
				break;
			case REGISTER_HL:
				instruction = INSTRUCTION_OR_HL_IND;
				break;
			case REGISTER_L:
				instruction = INSTRUCTION_OR_L;
				break;
			default:
				result = GENERATOR_ERROR(parser, tree, "Unsupported register");
				goto exit;
		}
	} else if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_pop(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_POP) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type != TOKEN_REGISTER) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	switch(child->token->subtype) {
		case REGISTER_AF:
			instruction = INSTRUCTION_POP_AF;
			break;
		case REGISTER_BC:
			instruction = INSTRUCTION_POP_BC;
			break;
		case REGISTER_DE:
			instruction = INSTRUCTION_POP_DE;
			break;
		case REGISTER_HL:
			instruction = INSTRUCTION_POP_HL;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported register");
			goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_push(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_PUSH) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type != TOKEN_REGISTER) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	switch(child->token->subtype) {
		case REGISTER_AF:
			instruction = INSTRUCTION_PUSH_AF;
			break;
		case REGISTER_BC:
			instruction = INSTRUCTION_PUSH_BC;
			break;
		case REGISTER_DE:
			instruction = INSTRUCTION_PUSH_DE;
			break;
		case REGISTER_HL:
			instruction = INSTRUCTION_PUSH_HL;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported register");
			goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_res(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS;

	if((tree->token->subtype < OPCODE_RES0)
			|| (tree->token->subtype > OPCODE_RES7)) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type != TOKEN_REGISTER) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	switch(child->token->subtype) {
		case REGISTER_A:
			instruction = INSTRUCTION_EXTENDED_RES_0_A;
			break;
		case REGISTER_B:
			instruction = INSTRUCTION_EXTENDED_RES_0_B;
			break;
		case REGISTER_C:
			instruction = INSTRUCTION_EXTENDED_RES_0_C;
			break;
		case REGISTER_D:
			instruction = INSTRUCTION_EXTENDED_RES_0_D;
			break;
		case REGISTER_E:
			instruction = INSTRUCTION_EXTENDED_RES_0_E;
			break;
		case REGISTER_H:
			instruction = INSTRUCTION_EXTENDED_RES_0_H;
			break;
		case REGISTER_HL:
			instruction = INSTRUCTION_EXTENDED_RES_0_HL_IND;
			break;
		case REGISTER_L:
			instruction = INSTRUCTION_EXTENDED_RES_0_H;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported register");
			goto exit;
	}

	instruction += ((tree->token->subtype - OPCODE_RES0) * CHAR_BIT);

	if((result = dmg_assembler_generate_instruction(generator, instruction, true, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_ret(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = INSTRUCTION_RET, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_RET) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count) {

		if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(child->token->type != TOKEN_CONDITION) {
			result = GENERATOR_ERROR(parser, tree, "Expecting condition");
			goto exit;
		}

		switch(child->token->subtype) {
			case CONDITION_CARRY:
				instruction = INSTRUCTION_RET_C;
				break;
			case CONDITION_CARRY_NOT:
				instruction = INSTRUCTION_RET_NC;
				break;
			case CONDITION_ZERO:
				instruction = INSTRUCTION_RET_Z;
				break;
			case CONDITION_ZERO_NOT:
				instruction = INSTRUCTION_RET_NZ;
				break;
			default:
				result = GENERATOR_ERROR(parser, tree, "Unsupported condition");
				goto exit;
		}
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_reti(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->token->subtype != OPCODE_RETI) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_RETI, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_rl(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_RL) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type != TOKEN_REGISTER) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	switch(child->token->subtype) {
		case REGISTER_A:
			instruction = INSTRUCTION_EXTENDED_RL_A;
			break;
		case REGISTER_B:
			instruction = INSTRUCTION_EXTENDED_RL_B;
			break;
		case REGISTER_C:
			instruction = INSTRUCTION_EXTENDED_RL_C;
			break;
		case REGISTER_D:
			instruction = INSTRUCTION_EXTENDED_RL_D;
			break;
		case REGISTER_E:
			instruction = INSTRUCTION_EXTENDED_RL_E;
			break;
		case REGISTER_H:
			instruction = INSTRUCTION_EXTENDED_RL_H;
			break;
		case REGISTER_HL:
			instruction = INSTRUCTION_EXTENDED_RL_HL_IND;
			break;
		case REGISTER_L:
			instruction = INSTRUCTION_EXTENDED_RL_L;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported register");
			goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, true, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_rla(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->token->subtype != OPCODE_RLA) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_RLA, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_rlc(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_RLC) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type != TOKEN_REGISTER) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	switch(child->token->subtype) {
		case REGISTER_A:
			instruction = INSTRUCTION_EXTENDED_RLC_A;
			break;
		case REGISTER_B:
			instruction = INSTRUCTION_EXTENDED_RLC_B;
			break;
		case REGISTER_C:
			instruction = INSTRUCTION_EXTENDED_RLC_C;
			break;
		case REGISTER_D:
			instruction = INSTRUCTION_EXTENDED_RLC_D;
			break;
		case REGISTER_E:
			instruction = INSTRUCTION_EXTENDED_RLC_E;
			break;
		case REGISTER_H:
			instruction = INSTRUCTION_EXTENDED_RLC_H;
			break;
		case REGISTER_HL:
			instruction = INSTRUCTION_EXTENDED_RLC_HL_IND;
			break;
		case REGISTER_L:
			instruction = INSTRUCTION_EXTENDED_RLC_L;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported register");
			goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, true, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_rlca(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->token->subtype != OPCODE_RLCA) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_RLCA, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_rr(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_RR) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type != TOKEN_REGISTER) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	switch(child->token->subtype) {
		case REGISTER_A:
			instruction = INSTRUCTION_EXTENDED_RR_A;
			break;
		case REGISTER_B:
			instruction = INSTRUCTION_EXTENDED_RR_B;
			break;
		case REGISTER_C:
			instruction = INSTRUCTION_EXTENDED_RR_C;
			break;
		case REGISTER_D:
			instruction = INSTRUCTION_EXTENDED_RR_D;
			break;
		case REGISTER_E:
			instruction = INSTRUCTION_EXTENDED_RR_E;
			break;
		case REGISTER_H:
			instruction = INSTRUCTION_EXTENDED_RR_H;
			break;
		case REGISTER_HL:
			instruction = INSTRUCTION_EXTENDED_RR_HL_IND;
			break;
		case REGISTER_L:
			instruction = INSTRUCTION_EXTENDED_RR_L;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported register");
			goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, true, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_rra(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->token->subtype != OPCODE_RRA) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_RRA, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_rrc(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_RRC) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type != TOKEN_REGISTER) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	switch(child->token->subtype) {
		case REGISTER_A:
			instruction = INSTRUCTION_EXTENDED_RRC_A;
			break;
		case REGISTER_B:
			instruction = INSTRUCTION_EXTENDED_RRC_B;
			break;
		case REGISTER_C:
			instruction = INSTRUCTION_EXTENDED_RRC_C;
			break;
		case REGISTER_D:
			instruction = INSTRUCTION_EXTENDED_RRC_D;
			break;
		case REGISTER_E:
			instruction = INSTRUCTION_EXTENDED_RRC_E;
			break;
		case REGISTER_H:
			instruction = INSTRUCTION_EXTENDED_RRC_H;
			break;
		case REGISTER_HL:
			instruction = INSTRUCTION_EXTENDED_RRC_HL_IND;
			break;
		case REGISTER_L:
			instruction = INSTRUCTION_EXTENDED_RRC_L;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported register");
			goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, true, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_rrca(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->token->subtype != OPCODE_RRCA) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_RRCA, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_rst(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_RST) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	switch(value.word) {
		case ADDRESS_RST_00:
			instruction = INSTRUCTION_RST_00;
			break;
		case ADDRESS_RST_08:
			instruction = INSTRUCTION_RST_08;
			break;
		case ADDRESS_RST_10:
			instruction = INSTRUCTION_RST_10;
			break;
		case ADDRESS_RST_18:
			instruction = INSTRUCTION_RST_18;
			break;
		case ADDRESS_RST_20:
			instruction = INSTRUCTION_RST_20;
			break;
		case ADDRESS_RST_28:
			instruction = INSTRUCTION_RST_28;
			break;
		case ADDRESS_RST_30:
			instruction = INSTRUCTION_RST_30;
			break;
		case ADDRESS_RST_38:
			instruction = INSTRUCTION_RST_38;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported address");
			goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_sbc(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = INSTRUCTION_SBC_U8, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_SBC) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type == TOKEN_REGISTER) {

		switch(child->token->subtype) {
			case REGISTER_A:
				instruction = INSTRUCTION_SBC_A;
				break;
			case REGISTER_B:
				instruction = INSTRUCTION_SBC_B;
				break;
			case REGISTER_C:
				instruction = INSTRUCTION_SBC_C;
				break;
			case REGISTER_D:
				instruction = INSTRUCTION_SBC_D;
				break;
			case REGISTER_E:
				instruction = INSTRUCTION_SBC_E;
				break;
			case REGISTER_H:
				instruction = INSTRUCTION_SBC_H;
				break;
			case REGISTER_HL:
				instruction = INSTRUCTION_SBC_HL_IND;
				break;
			case REGISTER_L:
				instruction = INSTRUCTION_SBC_L;
				break;
			default:
				result = GENERATOR_ERROR(parser, tree, "Unsupported register");
				goto exit;
		}
	} else if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_scf(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->token->subtype != OPCODE_SCF) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_SCF, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_set(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS;

	if((tree->token->subtype < OPCODE_SET0)
			|| (tree->token->subtype > OPCODE_SET7)) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type != TOKEN_REGISTER) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	switch(child->token->subtype) {
		case REGISTER_A:
			instruction = INSTRUCTION_EXTENDED_SET_0_A;
			break;
		case REGISTER_B:
			instruction = INSTRUCTION_EXTENDED_SET_0_B;
			break;
		case REGISTER_C:
			instruction = INSTRUCTION_EXTENDED_SET_0_C;
			break;
		case REGISTER_D:
			instruction = INSTRUCTION_EXTENDED_SET_0_D;
			break;
		case REGISTER_E:
			instruction = INSTRUCTION_EXTENDED_SET_0_E;
			break;
		case REGISTER_H:
			instruction = INSTRUCTION_EXTENDED_SET_0_H;
			break;
		case REGISTER_HL:
			instruction = INSTRUCTION_EXTENDED_SET_0_HL_IND;
			break;
		case REGISTER_L:
			instruction = INSTRUCTION_EXTENDED_SET_0_H;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported register");
			goto exit;
	}

	instruction += ((tree->token->subtype - OPCODE_SET0) * CHAR_BIT);

	if((result = dmg_assembler_generate_instruction(generator, instruction, true, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_sla(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_SLA) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type != TOKEN_REGISTER) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	switch(child->token->subtype) {
		case REGISTER_A:
			instruction = INSTRUCTION_EXTENDED_SLA_A;
			break;
		case REGISTER_B:
			instruction = INSTRUCTION_EXTENDED_SLA_B;
			break;
		case REGISTER_C:
			instruction = INSTRUCTION_EXTENDED_SLA_C;
			break;
		case REGISTER_D:
			instruction = INSTRUCTION_EXTENDED_SLA_D;
			break;
		case REGISTER_E:
			instruction = INSTRUCTION_EXTENDED_SLA_E;
			break;
		case REGISTER_H:
			instruction = INSTRUCTION_EXTENDED_SLA_H;
			break;
		case REGISTER_HL:
			instruction = INSTRUCTION_EXTENDED_SLA_HL_IND;
			break;
		case REGISTER_L:
			instruction = INSTRUCTION_EXTENDED_SLA_L;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported register");
			goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, true, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_sra(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_SRA) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type != TOKEN_REGISTER) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	switch(child->token->subtype) {
		case REGISTER_A:
			instruction = INSTRUCTION_EXTENDED_SRA_A;
			break;
		case REGISTER_B:
			instruction = INSTRUCTION_EXTENDED_SRA_B;
			break;
		case REGISTER_C:
			instruction = INSTRUCTION_EXTENDED_SRA_C;
			break;
		case REGISTER_D:
			instruction = INSTRUCTION_EXTENDED_SRA_D;
			break;
		case REGISTER_E:
			instruction = INSTRUCTION_EXTENDED_SRA_E;
			break;
		case REGISTER_H:
			instruction = INSTRUCTION_EXTENDED_SRA_H;
			break;
		case REGISTER_HL:
			instruction = INSTRUCTION_EXTENDED_SRA_HL_IND;
			break;
		case REGISTER_L:
			instruction = INSTRUCTION_EXTENDED_SRA_L;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported register");
			goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, true, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_srl(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_SRL) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type != TOKEN_REGISTER) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	switch(child->token->subtype) {
		case REGISTER_A:
			instruction = INSTRUCTION_EXTENDED_SRL_A;
			break;
		case REGISTER_B:
			instruction = INSTRUCTION_EXTENDED_SRL_B;
			break;
		case REGISTER_C:
			instruction = INSTRUCTION_EXTENDED_SRL_C;
			break;
		case REGISTER_D:
			instruction = INSTRUCTION_EXTENDED_SRL_D;
			break;
		case REGISTER_E:
			instruction = INSTRUCTION_EXTENDED_SRL_E;
			break;
		case REGISTER_H:
			instruction = INSTRUCTION_EXTENDED_SRL_H;
			break;
		case REGISTER_HL:
			instruction = INSTRUCTION_EXTENDED_SRL_HL_IND;
			break;
		case REGISTER_L:
			instruction = INSTRUCTION_EXTENDED_SRL_L;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported register");
			goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, true, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_stop(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->token->subtype != OPCODE_STOP) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_STOP, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_sub(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = INSTRUCTION_SUB_U8, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_SUB) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type == TOKEN_REGISTER) {

		switch(child->token->subtype) {
			case REGISTER_A:
				instruction = INSTRUCTION_SUB_A;
				break;
			case REGISTER_B:
				instruction = INSTRUCTION_SUB_B;
				break;
			case REGISTER_C:
				instruction = INSTRUCTION_SUB_C;
				break;
			case REGISTER_D:
				instruction = INSTRUCTION_SUB_D;
				break;
			case REGISTER_E:
				instruction = INSTRUCTION_SUB_E;
				break;
			case REGISTER_H:
				instruction = INSTRUCTION_SUB_H;
				break;
			case REGISTER_HL:
				instruction = INSTRUCTION_SUB_HL_IND;
				break;
			case REGISTER_L:
				instruction = INSTRUCTION_SUB_L;
				break;
			default:
				result = GENERATOR_ERROR(parser, tree, "Unsupported register");
				goto exit;
		}
	} else if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_swap(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = 0, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_SWAP) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type != TOKEN_REGISTER) {
		result = GENERATOR_ERROR(parser, tree, "Expecting register");
		goto exit;
	}

	switch(child->token->subtype) {
		case REGISTER_A:
			instruction = INSTRUCTION_EXTENDED_SWAP_A;
			break;
		case REGISTER_B:
			instruction = INSTRUCTION_EXTENDED_SWAP_B;
			break;
		case REGISTER_C:
			instruction = INSTRUCTION_EXTENDED_SWAP_C;
			break;
		case REGISTER_D:
			instruction = INSTRUCTION_EXTENDED_SWAP_D;
			break;
		case REGISTER_E:
			instruction = INSTRUCTION_EXTENDED_SWAP_E;
			break;
		case REGISTER_H:
			instruction = INSTRUCTION_EXTENDED_SWAP_H;
			break;
		case REGISTER_HL:
			instruction = INSTRUCTION_EXTENDED_SWAP_HL_IND;
			break;
		case REGISTER_L:
			instruction = INSTRUCTION_EXTENDED_SWAP_L;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Unsupported register");
			goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, true, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_unused(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	int instruction, result = DMG_STATUS_SUCCESS;

	switch(tree->token->subtype) {
		case OPCODE_UNUSED_D3:
			instruction = INSTRUCTION_UNUSED_D3;
			break;
		case OPCODE_UNUSED_DB:
			instruction = INSTRUCTION_UNUSED_DB;
			break;
		case OPCODE_UNUSED_DD:
			instruction = INSTRUCTION_UNUSED_DD;
			break;
		case OPCODE_UNUSED_E3:
			instruction = INSTRUCTION_UNUSED_E3;
			break;
		case OPCODE_UNUSED_E4:
			instruction = INSTRUCTION_UNUSED_E4;
			break;
		case OPCODE_UNUSED_EB:
			instruction = INSTRUCTION_UNUSED_EB;
			break;
		case OPCODE_UNUSED_EC:
			instruction = INSTRUCTION_UNUSED_EC;
			break;
		case OPCODE_UNUSED_ED:
			instruction = INSTRUCTION_UNUSED_ED;
			break;
		case OPCODE_UNUSED_F4:
			instruction = INSTRUCTION_UNUSED_F4;
			break;
		case OPCODE_UNUSED_FC:
			instruction = INSTRUCTION_UNUSED_FC;
			break;
		case OPCODE_UNUSED_FD:
			instruction = INSTRUCTION_UNUSED_FD;
			break;
		default:
			result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
			goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_xor(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	dmg_assembler_scalar_t value = {};
	dmg_assembler_tree_t *child = NULL;
	int instruction = INSTRUCTION_XOR_U8, result = DMG_STATUS_SUCCESS;

	if(tree->token->subtype != OPCODE_XOR) {
		result = GENERATOR_ERROR(parser, tree, "Expecting opcode");
		goto exit;
	}

	if(!tree->count) {
		result = GENERATOR_ERROR(parser, tree, "Expecting expression");
		goto exit;
	}

	if((result = dmg_assembler_tree_child(&parser->trees, tree, 0, &child)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(child->token->type == TOKEN_REGISTER) {

		switch(child->token->subtype) {
			case REGISTER_A:
				instruction = INSTRUCTION_XOR_A;
				break;
			case REGISTER_B:
				instruction = INSTRUCTION_XOR_B;
				break;
			case REGISTER_C:
				instruction = INSTRUCTION_XOR_C;
				break;
			case REGISTER_D:
				instruction = INSTRUCTION_XOR_D;
				break;
			case REGISTER_E:
				instruction = INSTRUCTION_XOR_E;
				break;
			case REGISTER_H:
				instruction = INSTRUCTION_XOR_H;
				break;
			case REGISTER_HL:
				instruction = INSTRUCTION_XOR_HL_IND;
				break;
			case REGISTER_L:
				instruction = INSTRUCTION_XOR_L;
				break;
			default:
				result = GENERATOR_ERROR(parser, tree, "Unsupported register");
				goto exit;
		}
	} else if((result = dmg_assembler_generator_evaluate_expression(generator, parser, child, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static dmg_assembler_generator_hdlr OPCODE_HANDLER[] = {
	dmg_assembler_generate_opcode_adc, /* OPCODE_ADC */
	dmg_assembler_generate_opcode_add, /* OPCODE_ADD */
	dmg_assembler_generate_opcode_and, /* OPCODE_AND */
	dmg_assembler_generate_opcode_call, /* OPCODE_CALL */
	dmg_assembler_generate_opcode_ccf, /* OPCODE_CCF */
	dmg_assembler_generate_opcode_cp, /* OPCODE_CP */
	dmg_assembler_generate_opcode_cpl, /* OPCODE_CPL */
	dmg_assembler_generate_opcode_daa, /* OPCODE_DAA */
	dmg_assembler_generate_opcode_dec, /* OPCODE_DEC */
	dmg_assembler_generate_opcode_di, /* OPCODE_DI */
	dmg_assembler_generate_opcode_ei, /* OPCODE_EI */
	dmg_assembler_generate_opcode_halt, /* OPCODE_HALT */
	dmg_assembler_generate_opcode_inc, /* OPCODE_INC */
	dmg_assembler_generate_opcode_jp, /* OPCODE_JP */
	dmg_assembler_generate_opcode_jr, /* OPCODE_JR */
	dmg_assembler_generate_opcode_ld, /* OPCODE_LD */
	dmg_assembler_generate_opcode_nop, /* OPCODE_NOP */
	dmg_assembler_generate_opcode_or, /* OPCODE_OR */
	dmg_assembler_generate_opcode_pop, /* OPCODE_POP */
	dmg_assembler_generate_opcode_push, /* OPCODE_PUSH */
	dmg_assembler_generate_opcode_ret, /* OPCODE_RET */
	dmg_assembler_generate_opcode_reti, /* OPCODE_RETI */
	dmg_assembler_generate_opcode_rla, /* OPCODE_RLA */
	dmg_assembler_generate_opcode_rlca, /* OPCODE_RLCA */
	dmg_assembler_generate_opcode_rra, /* OPCODE_RRA */
	dmg_assembler_generate_opcode_rrca, /* OPCODE_RRCA */
	dmg_assembler_generate_opcode_rst, /* OPCODE_RST */
	dmg_assembler_generate_opcode_scf, /* OPCODE_SCF */
	dmg_assembler_generate_opcode_sbc, /* OPCODE_SBC */
	dmg_assembler_generate_opcode_stop, /* OPCODE_STOP */
	dmg_assembler_generate_opcode_sub, /* OPCODE_SUB */
	dmg_assembler_generate_opcode_xor, /* OPCODE_XOR */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_D3 */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_DB */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_DD */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_E3 */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_E4 */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_EB */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_EC */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_ED */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_F4 */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_FC */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_FD */
	dmg_assembler_generate_opcode_bit, /* OPCODE_BIT0 */
	dmg_assembler_generate_opcode_bit, /* OPCODE_BIT1 */
	dmg_assembler_generate_opcode_bit, /* OPCODE_BIT2 */
	dmg_assembler_generate_opcode_bit, /* OPCODE_BIT3 */
	dmg_assembler_generate_opcode_bit, /* OPCODE_BIT4 */
	dmg_assembler_generate_opcode_bit, /* OPCODE_BIT5 */
	dmg_assembler_generate_opcode_bit, /* OPCODE_BIT6 */
	dmg_assembler_generate_opcode_bit, /* OPCODE_BIT7 */
	dmg_assembler_generate_opcode_res, /* OPCODE_RES0 */
	dmg_assembler_generate_opcode_res, /* OPCODE_RES1 */
	dmg_assembler_generate_opcode_res, /* OPCODE_RES2 */
	dmg_assembler_generate_opcode_res, /* OPCODE_RES3 */
	dmg_assembler_generate_opcode_res, /* OPCODE_RES4 */
	dmg_assembler_generate_opcode_res, /* OPCODE_RES5 */
	dmg_assembler_generate_opcode_res, /* OPCODE_RES6 */
	dmg_assembler_generate_opcode_res, /* OPCODE_RES7 */
	dmg_assembler_generate_opcode_rl, /* OPCODE_RL */
	dmg_assembler_generate_opcode_rlc, /* OPCODE_RLC */
	dmg_assembler_generate_opcode_rr, /* OPCODE_RR */
	dmg_assembler_generate_opcode_rrc, /* OPCODE_RRC */
	dmg_assembler_generate_opcode_set, /* OPCODE_SET0 */
	dmg_assembler_generate_opcode_set, /* OPCODE_SET1 */
	dmg_assembler_generate_opcode_set, /* OPCODE_SET2 */
	dmg_assembler_generate_opcode_set, /* OPCODE_SET3 */
	dmg_assembler_generate_opcode_set, /* OPCODE_SET4 */
	dmg_assembler_generate_opcode_set, /* OPCODE_SET5 */
	dmg_assembler_generate_opcode_set, /* OPCODE_SET6 */
	dmg_assembler_generate_opcode_set, /* OPCODE_SET7 */
	dmg_assembler_generate_opcode_sla, /* OPCODE_SLA */
	dmg_assembler_generate_opcode_sra, /* OPCODE_SRA */
	dmg_assembler_generate_opcode_srl, /* OPCODE_SRL */
	dmg_assembler_generate_opcode_swap, /* OPCODE_SWAP */
	};

static int
dmg_assembler_generator_generate_opcode(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_generator_hdlr handler;

	if((tree->token->subtype >= OPCODE_MAX)
			|| !(handler = OPCODE_HANDLER[tree->token->subtype])) {
		result = GENERATOR_ERROR(parser, tree, "Unsupported opcode");
		goto exit;
	}

	if((result = handler(generator, parser, tree, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static dmg_assembler_generator_hdlr STATEMENT_HANDLER[] = {
	NULL, /* TOKEN_END */
	NULL, /* TOKEN_CONDITION */
	dmg_assembler_generator_generate_directive, /* TOKEN_DIRECTIVE */
	NULL, /* TOKEN_IDENTIFIER */
	NULL, /* TOKEN_INEQUALITY */
	dmg_assembler_generator_generate_label, /* TOKEN_LABEL */
	NULL, /* TOKEN_LITERAL */
	NULL, /* TOKEN_MACRO */
	dmg_assembler_generator_generate_opcode, /* TOKEN_OPCODE */
	NULL, /* TOKEN_OPERATOR */
	NULL, /* TOKEN_REGISTER */
	NULL, /* TOKEN_SCALAR */
	NULL, /* TOKEN_SYMBOL */
	};

static int
dmg_assembler_generator_generate_statement(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in bool first_pass
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_generator_hdlr handler;

	if((tree->token->type >= TOKEN_MAX)
			|| !(handler = STATEMENT_HANDLER[tree->token->type])) {
		result = GENERATOR_ERROR(parser, tree, "Unsupported statement");
		goto exit;
	}

	if((result = handler(generator, parser, tree, first_pass)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

int
dmg_assembler_generator_load(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_buffer_t *buffer,
	__in const char *path,
	__in FILE *file
	)
{
	int result;

	if((result = dmg_assembler_banks_allocate(&generator->banks)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_constants_allocate(&generator->defines)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_constants_allocate(&generator->labels)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_parser_load(&generator->parser, buffer, path)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	generator->buffer = buffer;
	generator->file = file;
	generator->path = path;
	generator->root = dirname((char *)path);

exit:
	return result;
}

int
dmg_assembler_generator_run(
	__inout dmg_assembler_generator_t *generator
	)
{
	int result = DMG_STATUS_SUCCESS;
	const dmg_assembler_tree_t *tree;

	for(;;) {

		if(!(tree = dmg_assembler_parser_tree(&generator->parser))) {
			break;
		}

		if((result = dmg_assembler_generator_generate_statement(generator, &generator->parser, tree, true))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(!dmg_assembler_parser_has_next(&generator->parser)
				|| ((result = dmg_assembler_parser_next(&generator->parser)) != DMG_STATUS_SUCCESS)) {
			break;
		}
	}

	dmg_assembler_parser_unload(&generator->parser);

	if((result = dmg_assembler_parser_load(&generator->parser, generator->buffer, generator->path)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	dmg_assembler_constants_free(&generator->defines);

	if((result = dmg_assembler_constants_allocate(&generator->defines)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	generator->bank = 0;
	generator->offset.word = 0;

	for(;;) {

		if(!(tree = dmg_assembler_parser_tree(&generator->parser))) {
			break;
		}

		if((result = dmg_assembler_generator_generate_statement(generator, &generator->parser, tree, false))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(!dmg_assembler_parser_has_next(&generator->parser)
				|| ((result = dmg_assembler_parser_next(&generator->parser)) != DMG_STATUS_SUCCESS)) {
			break;
		}
	}

	if(generator->file) {

		for(uint32_t index = 0; index < generator->banks.count; ++index) {
			dmg_assembler_bank_t *bank = &generator->banks.bank[index];

			fprintf(stdout, "Writing bank[%u] %.02f%% full (%u/%u)\n", index, 100.f * (bank->size.word / (float)BANK_WIDTH),
				bank->size.word, BANK_WIDTH);

			if(fwrite(bank->data, sizeof(uint8_t), sizeof(bank->data), generator->file) != sizeof(bank->data)) {
				result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Failed to write bank %u to file", index);
				goto exit;
			}
		}
	}

exit:
	return result;
}

void
dmg_assembler_generator_unload(
	__inout dmg_assembler_generator_t *generator
	)
{
	dmg_assembler_parser_unload(&generator->parser);
	dmg_assembler_constants_free(&generator->labels);
	dmg_assembler_constants_free(&generator->defines);
	dmg_assembler_banks_free(&generator->banks);
	memset(generator, 0, sizeof(*generator));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
