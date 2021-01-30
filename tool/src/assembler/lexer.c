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

#include "./lexer_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_assembler_lexer_token_parse_alpha(
	__inout dmg_assembler_lexer_t *lexer,
	__inout dmg_assembler_token_t *token
	)
{
	dmg_assembler_string_t string = {};
	int result = DMG_STATUS_SUCCESS, type;

	if((result = dmg_assembler_string_allocate(&string)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

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

	for(; token->literal.length < ALPHA_LENGTH_MAX;) {
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

		if(dmg_tool_syntax_is_condition_string(string.str, &type)) {
			token->type = TOKEN_CONDITION;
			token->subtype = type;
		} else if(dmg_tool_syntax_is_macro_string(string.str, &type)) {
			token->type = TOKEN_MACRO;
			token->subtype = type;
		} else if(dmg_tool_syntax_is_opcode_string(string.str, &type)) {
			token->type = TOKEN_OPCODE;
			token->subtype = type;
		} else if(dmg_tool_syntax_is_register_string(string.str, &type)) {
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
	__inout dmg_assembler_lexer_t *lexer,
	__inout dmg_assembler_token_t *token
	)
{
	dmg_assembler_string_t string = {};
	int result = DMG_STATUS_SUCCESS, type;

	if((result = dmg_assembler_string_allocate(&string)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

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

	for(; token->literal.length < DIRECTIVE_LENGTH_MAX;) {
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

	if(!dmg_tool_syntax_is_directive_string(string.str, &type)) {
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
dmg_assembler_lexer_token_parse_literal(
	__inout dmg_assembler_lexer_t *lexer,
	__inout dmg_assembler_token_t *token
	)
{
	char value;
	int result = DMG_STATUS_SUCCESS, type;
	dmg_assembler_string_t string_scalar = {}, string = {};
	bool literal_character = false, literal_string = false;

	if((result = dmg_assembler_string_allocate(&string)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_string_append(&string, value = dmg_assembler_stream_character(&lexer->stream, &type))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(value == DELIMITER_LITERAL_CHARACTER[0]) {
		literal_character = true;
	} else if(value == DELIMITER_LITERAL_STRING[0]) {
		literal_string = true;
	} else {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Expecting literal \"%s\" (%s@%u)", string.str,
						lexer->stream.path, lexer->stream.line);
		goto exit;
	}

	if(!dmg_assembler_stream_has_next(&lexer->stream)
			|| ((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS)) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Unterminated literal \"%s\" (%s@%u)", string.str,
						lexer->stream.path, lexer->stream.line);
		goto exit;
	}

	token->type = TOKEN_LITERAL;
	token->subtype = 0;
	token->literal.str = dmg_assembler_stream_character_str(&lexer->stream);
	token->literal.length = 0;

	for(uint32_t length = 0;; ++length) {

		if((result = dmg_assembler_string_append(&string, value = dmg_assembler_stream_character(&lexer->stream, &type))) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(literal_character && (value == DELIMITER_LITERAL_CHARACTER[0])) {

			if(length != 1) {
				result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Unsupported character literal \"%s\" (%s@%u)", string.str,
								lexer->stream.path, lexer->stream.line);
				goto exit;
			}

			break;
		} else if(literal_string && (value == DELIMITER_LITERAL_STRING[0])) {
			break;
		} else if(value == CHARACTER_NEWLINE) {
			result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Unterminated literal \"%s\" (%s@%u)", string.str,
							lexer->stream.path, lexer->stream.line);
			goto exit;
		}

		if(!dmg_assembler_stream_has_next(&lexer->stream)
				|| ((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS)) {
			result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Unterminated literal \"%s\" (%s@%u)", string.str,
							lexer->stream.path, lexer->stream.line);
			goto exit;
		}

		++token->literal.length;

		if(value == DELIMITER_LITERAL_ESCAPE[0]) {
			char *end;
			int base, count = 0;
			bool binary = false, decimal = false, hexidecimal = false;

			if((result = dmg_assembler_string_allocate(&string_scalar)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			if((value = dmg_assembler_stream_character(&lexer->stream, &type)) == DELIMITER_BINARY[0]) {
				binary = true;
				base = BASE_BINARY;
				count = COUNT_BINARY_ESCAPE_MAX;
			} else if(value == DELIMITER_HEXIDECIMAL[0]) {
				hexidecimal = true;
				base = BASE_HEXIDECIMAL;
				count = COUNT_HEXIDECIMAL_ESCAPE_MAX;
			} else if((type & CHARACTER_DECIMAL) == CHARACTER_DECIMAL) {
				decimal = true;
				base = BASE_DECIMAL;
				count = COUNT_DECIMAL_ESCAPE_MAX;

				if((result = dmg_assembler_string_append(&string, value)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}

				if((result = dmg_assembler_string_append(&string_scalar, value)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
			}

			if(!dmg_assembler_stream_has_next(&lexer->stream)
					|| ((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS)) {
				result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Unterminated literal \"%s\" (%s@%u)", string.str,
								lexer->stream.path, lexer->stream.line);
				goto exit;
			}

			++token->literal.length;

			if(binary || decimal || hexidecimal) {

				for(; count >= 0; count--) {
					value = dmg_assembler_stream_character(&lexer->stream, &type);

					if((literal_character && (value == DELIMITER_LITERAL_CHARACTER[0]))
							|| (literal_string && (value == DELIMITER_LITERAL_STRING[0]))) {
						break;
					}

					if(binary) {

						if((type & CHARACTER_BINARY) != CHARACTER_BINARY) {
							break;
						}
					} else if(hexidecimal) {

						if((type & CHARACTER_HEXIDECIMAL) != CHARACTER_HEXIDECIMAL) {
							break;
						}
					} else if((type & CHARACTER_DECIMAL) != CHARACTER_DECIMAL) {
						break;
					}

					if((result = dmg_assembler_string_append(&string, value)) != DMG_STATUS_SUCCESS) {
						goto exit;
					}

					if((result = dmg_assembler_string_append(&string_scalar, value)) != DMG_STATUS_SUCCESS) {
						goto exit;
					}

					++token->literal.length;

					if(!dmg_assembler_stream_has_next(&lexer->stream)
							|| ((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS)) {
						break;
					}
				}

				if(binary) {

					if((type & CHARACTER_BINARY) == CHARACTER_BINARY) {
						result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Out-of-range binary scalar \"%s\" (%s@%u)",
										string.str, lexer->stream.path, lexer->stream.line);
						goto exit;
					}
				} else if(hexidecimal) {

					if((type & CHARACTER_HEXIDECIMAL) == CHARACTER_HEXIDECIMAL) {
						result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Out-of-range hexidecimal scalar \"%s\" (%s@%u)",
										string.str, lexer->stream.path, lexer->stream.line);
						goto exit;
					}
				} else if((type & CHARACTER_DECIMAL) == CHARACTER_DECIMAL) {
					result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Out-of-range scalar \"%s\" (%s@%u)",
									string.str, lexer->stream.path, lexer->stream.line);
					goto exit;
				}

				if(strtol(string_scalar.str, &end, base) > UINT8_MAX) {
					result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Out-of-range scalar \"%s\" (%s@%u)", string.str,
									lexer->stream.path, lexer->stream.line);
					goto exit;
				}
			} else if(!dmg_tool_syntax_is_escape_character(value, &type)) {
				result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Unsupported character escape \"%s\" (%s@%u)", string.str,
								lexer->stream.path, lexer->stream.line);
				goto exit;
			}

			dmg_assembler_string_free(&string_scalar);
		}
	}

	if(!dmg_assembler_stream_has_next(&lexer->stream)
			|| ((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS)) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Unterminated literal \"%s\" (%s@%u)", string.str,
						lexer->stream.path, lexer->stream.line);
		goto exit;
	}

exit:
	dmg_assembler_string_free(&string_scalar);
	dmg_assembler_string_free(&string);

	return result;
}

static int
dmg_assembler_lexer_token_parse_scalar(
	__inout dmg_assembler_lexer_t *lexer,
	__inout dmg_assembler_token_t *token
	)
{
	long scalar;
	char *end, value;
	dmg_assembler_string_t string = {};
	int base = BASE_DECIMAL, count = COUNT_DECIMAL_MAX, result = DMG_STATUS_SUCCESS, type;
	bool binary = false, hexidecimal = false, negate = false, not = false, subtract = false;

	if((result = dmg_assembler_string_allocate(&string)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(token->type == TOKEN_OPERATOR) {

		switch(token->subtype) {
			case OPERATOR_ARITHMETIC_SUBTRACT:
				subtract = true;
				break;
			case OPERATOR_UNARY_NEGATE:
				negate = true;
				break;
			case OPERATOR_UNARY_NOT:
				not = true;
				break;
			default:
				token->line = lexer->stream.line;
				token->literal.str = dmg_assembler_stream_character_str(&lexer->stream);
				token->literal.length = 0;
				break;
		}
	} else {
		token->line = lexer->stream.line;
		token->literal.str = dmg_assembler_stream_character_str(&lexer->stream);
		token->literal.length = 0;
	}

	token->type = TOKEN_SCALAR;
	token->subtype = 0;
	value = dmg_assembler_stream_character(&lexer->stream, &type);

	if((type & CHARACTER_SYMBOL) == CHARACTER_SYMBOL) {

		if((result = dmg_assembler_string_append(&string, value)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(value == DELIMITER_BINARY[0]) {
			binary = true;
			base = BASE_BINARY;
			count = COUNT_BINARY_MAX;
		} else if(value == DELIMITER_HEXIDECIMAL[0]) {
			hexidecimal = true;
			base = BASE_HEXIDECIMAL;
			count = COUNT_HEXIDECIMAL_MAX;
		} else {
			result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Unsupported scalar \"%s\" (%s@%u)", string.str,
							lexer->stream.path, lexer->stream.line);
			goto exit;
		}

		if(!dmg_assembler_stream_has_next(&lexer->stream)
				|| ((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS)) {
			result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Unterminated scalar \"%s\" (%s@%u)", string.str,
							lexer->stream.path, lexer->stream.line);
			goto exit;
		}

		value = dmg_assembler_stream_character(&lexer->stream, &type);

		if(binary) {

			if((type & CHARACTER_BINARY) != CHARACTER_BINARY) {
				result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Expecting binary scalar \"%s\" (%s@%u)", string.str,
								lexer->stream.path, lexer->stream.line);
				goto exit;
			}
		} else if(hexidecimal) {

			if((type & CHARACTER_HEXIDECIMAL) != CHARACTER_HEXIDECIMAL) {
				result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Expecting hexidecimal scalar \"%s\" (%s@%u)", string.str,
								lexer->stream.path, lexer->stream.line);
				goto exit;
			}
		} else if((type & CHARACTER_DECIMAL) != CHARACTER_DECIMAL) {
			result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Expecting scalar \"%s\" (%s@%u)", string.str,
							lexer->stream.path, lexer->stream.line);
			goto exit;
		}

		string.str[--string.length] = CHARACTER_EOF;
		++token->literal.length;
	}

	if((result = dmg_assembler_string_append(&string, value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	++token->literal.length;

	for(; count > 0; count--) {

		if(!dmg_assembler_stream_has_next(&lexer->stream)
				|| ((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS)) {
			break;
		}

		value = dmg_assembler_stream_character(&lexer->stream, &type);

		if(binary) {

			if((type & CHARACTER_BINARY) != CHARACTER_BINARY) {
				break;
			}
		} else if(hexidecimal) {

			if((type & CHARACTER_HEXIDECIMAL) != CHARACTER_HEXIDECIMAL) {
				break;
			}
		} else if((type & CHARACTER_DECIMAL) != CHARACTER_DECIMAL) {
			break;
		}

		if((result = dmg_assembler_string_append(&string, value)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		++token->literal.length;
	}

	if(binary) {

		if((type & CHARACTER_BINARY) == CHARACTER_BINARY) {
			result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Out-of-range binary scalar \"%s\" (%s@%u)", string.str,
							lexer->stream.path, lexer->stream.line);
			goto exit;
		}
	} else if(hexidecimal) {

		if((type & CHARACTER_HEXIDECIMAL) == CHARACTER_HEXIDECIMAL) {
			result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Out-of-range hexidecimal scalar \"%s\" (%s@%u)", string.str,
							lexer->stream.path, lexer->stream.line);
			goto exit;
		}
	} else if((type & CHARACTER_DECIMAL) == CHARACTER_DECIMAL) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Out-of-range scalar \"%s\" (%s@%u)", string.str,
						lexer->stream.path, lexer->stream.line);
		goto exit;
	}

	if((scalar = strtol(string.str, &end, base)) > UINT16_MAX) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Out-of-range scalar \"%s\" (%s@%u)", string.str,
						lexer->stream.path, lexer->stream.line);
		goto exit;
	}

	token->scalar.word = (uint16_t)scalar;

	if(subtract) {
		token->scalar.word = -token->scalar.word;
	} else if(negate) {
		token->scalar.word = ~token->scalar.word;
	} else if(not) {
		token->scalar.word = !token->scalar.word;
	}

exit:
	dmg_assembler_string_free(&string);

	return result;
}

static int
dmg_assembler_lexer_token_parse_symbol(
	__inout dmg_assembler_lexer_t *lexer,
	__inout dmg_assembler_token_t *token
	)
{
	char value;
	dmg_assembler_string_t string = {};
	int result = DMG_STATUS_SUCCESS, type;

	if((result = dmg_assembler_string_allocate(&string)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_string_append(&string, (value = dmg_assembler_stream_character(&lexer->stream, &type)))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((type & CHARACTER_SYMBOL) != CHARACTER_SYMBOL) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Expecting symbol \"%s\" (%s@%u)", string.str,
						lexer->stream.path, lexer->stream.line);
		goto exit;
	}

	if((value == DELIMITER_BINARY[0]) || (value == DELIMITER_HEXIDECIMAL[0])) {

		if((result = dmg_assembler_lexer_token_parse_scalar(lexer, token)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if(value == DELIMITER_DIRECTIVE[0]) {

		if((result = dmg_assembler_lexer_token_parse_directive(lexer, token)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if(value == DELIMITER_IDENTIFIER[0]) {

		if((result = dmg_assembler_lexer_token_parse_alpha(lexer, token)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else if((value == DELIMITER_LITERAL_CHARACTER[0]) || (value == DELIMITER_LITERAL_STRING[0])) {

		if((result = dmg_assembler_lexer_token_parse_literal(lexer, token)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	} else {
		token->line = lexer->stream.line;
		token->literal.str = dmg_assembler_stream_character_str(&lexer->stream);
		token->literal.length = 1;

		if(!dmg_assembler_stream_has_next(&lexer->stream)
				|| ((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS)) {

			if(!dmg_tool_syntax_is_operator_string(string.str, &type)
					&& !dmg_tool_syntax_is_symbol_string(string.str, &type)) {
				result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Unsupported symbol \"%s\" (%s@%u)", string.str,
								lexer->stream.path, lexer->stream.line);
			}
			goto exit;
		}

		value = dmg_assembler_stream_character(&lexer->stream, &type);

		if((type & CHARACTER_SYMBOL) == CHARACTER_SYMBOL) {

			if((result = dmg_assembler_string_append(&string, value)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			if(!dmg_tool_syntax_is_inequality_string(string.str, &type)
					&& !dmg_tool_syntax_is_operator_string(string.str, &type)
					&& !dmg_tool_syntax_is_symbol_string(string.str, &type)) {
				string.str[--string.length] = CHARACTER_EOF;
			} else if(!dmg_assembler_stream_has_next(&lexer->stream)
					|| ((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS)) {
				goto exit;
			} else {
				++token->literal.length;
			}
		}

		if(dmg_tool_syntax_is_inequality_string(string.str, &type)) {
			token->type = TOKEN_INEQUALITY;
		} else if(dmg_tool_syntax_is_operator_string(string.str, &type)) {
			token->type = TOKEN_OPERATOR;
		} else if(dmg_tool_syntax_is_symbol_string(string.str, &type)) {
			token->type = TOKEN_SYMBOL;
		} else {
			result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Unsupported symbol \"%s\" (%s@%u)", string.str,
							lexer->stream.path, lexer->stream.line);
			goto exit;
		}

		token->subtype = type;

		if(token->type == TOKEN_OPERATOR) {

			switch(token->subtype) {
				case OPERATOR_ARITHMETIC_SUBTRACT:
				case OPERATOR_UNARY_NEGATE:
				case OPERATOR_UNARY_NOT:
					value = dmg_assembler_stream_character(&lexer->stream, &type);

					if((((type & CHARACTER_DECIMAL) == CHARACTER_DECIMAL)
							|| (value == DELIMITER_BINARY[0])
							|| (value == DELIMITER_HEXIDECIMAL[0]))
							&& ((result = dmg_assembler_lexer_token_parse_scalar(lexer, token))
								!= DMG_STATUS_SUCCESS)) {
						goto exit;
					}
					break;
				default:
					break;
			}
		}
	}

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
		dmg_assembler_token_t *token;

		if((result = dmg_assembler_token_add(&lexer->tokens, &token)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		value = dmg_assembler_stream_character(&lexer->stream, &type);

		if((type & CHARACTER_END) != CHARACTER_END) {

			if((result = dmg_assembler_stream_next(&lexer->stream)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		} else if((type & CHARACTER_ALPHA) == CHARACTER_ALPHA) {

			if((result = dmg_assembler_lexer_token_parse_alpha(lexer, token)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		} else if((type & CHARACTER_DECIMAL) == CHARACTER_DECIMAL) {

			if((result = dmg_assembler_lexer_token_parse_scalar(lexer, token)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		} else if((type & CHARACTER_SYMBOL) == CHARACTER_SYMBOL) {

			if((result = dmg_assembler_lexer_token_parse_symbol(lexer, token)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		} else {
			result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Unsupported character \'%c\' (%s@%u)", value,
							lexer->stream.path, lexer->stream.line);
			goto exit;
		}
	}

exit:
	return result;
}

bool
dmg_assembler_lexer_has_next(
	__in const dmg_assembler_lexer_t *lexer
	)
{
	return (lexer->position < lexer->tokens.count);
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

	while(dmg_assembler_stream_has_next(&lexer->stream)) {

		if((result = dmg_assembler_lexer_token_parse(lexer)) != DMG_STATUS_SUCCESS) {
			break;
		}

		++lexer->position;
	}

	lexer->position = 0;

exit:
	return result;
}

int
dmg_assembler_lexer_next(
	__inout dmg_assembler_lexer_t *lexer
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!dmg_assembler_lexer_has_next(lexer)) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "No next token %u", lexer->position);
		goto exit;
	}

	++lexer->position;

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
