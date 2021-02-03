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

static int
dmg_assembler_generator_error_tree(
	__in const dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree,
	__inout dmg_assembler_string_t *string,
	__in uint32_t depth
	)
{
	int result = DMG_STATUS_SUCCESS;
	char str[GENERATOR_ERROR_STR_MAX] = {};

	for(uint32_t index = 0; index < depth; ++index) {

		if((result = dmg_assembler_string_append_character(string, '\t')) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	if(tree && tree->parent) {
		const dmg_assembler_token_t *token = tree->parent;

		snprintf(str, GENERATOR_ERROR_STR_MAX, "{%u} [%i", tree->count, token->type);

		if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(token->subtype != TOKEN_SUBTYPE_UNDEFINED) {
			snprintf(str, GENERATOR_ERROR_STR_MAX, ":%i", token->subtype);

			if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}

		if((result = dmg_assembler_string_append_character(string, ']')) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((token->type > TOKEN_END) && (token->type < TOKEN_MAX)) {

			if((result = dmg_assembler_string_append_character(string, '\"')) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			for(uint32_t index = 0; index < token->literal.length; ++index) {

				if((result = dmg_assembler_string_append_character(string, token->literal.str[index]))
						!= DMG_STATUS_SUCCESS) {
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

		snprintf(str, GENERATOR_ERROR_STR_MAX, " (%s@%u)\n", generator->parser.lexer.stream.path, token->line);

		if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		for(uint32_t index = 0; index < tree->count; ++index) {

			if((result = dmg_assembler_generator_error_tree(generator, (const dmg_assembler_tree_t *)tree->child[index], string, depth + 1))
					!= DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}
	}

exit:
	return result;
}

static int
dmg_assembler_generator_error(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree,
	__in const char *message,
	...
	)
{
	dmg_assembler_string_t string = {}, tree_string = {};

	if(dmg_assembler_string_allocate(&string) == DMG_STATUS_SUCCESS) {

		if(tree->parent->literal.length) {

			for(uint32_t index = 0; index < tree->parent->literal.length; ++index) {

				if(dmg_assembler_string_append_character(&string, tree->parent->literal.str[index]) != DMG_STATUS_SUCCESS) {
					break;
				}
			}
		} else {
			strcpy(string.str, "EOF");
		}
	}

	if(dmg_assembler_string_allocate(&tree_string) == DMG_STATUS_SUCCESS) {
		dmg_assembler_generator_error_tree(generator, tree, &tree_string, 1);
	}

	ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s \"%s\" (%s@%u)\n%s", message, string.str, generator->parser.lexer.stream.path, tree->parent->line, tree_string);
	dmg_assembler_string_free(&tree_string);
	dmg_assembler_string_free(&string);

	return DMG_STATUS_FAILURE;
}

static int
dmg_assembler_generator_generate_directive(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(tree->parent->type != TOKEN_DIRECTIVE) {
		result = GENERATOR_ERROR(generator, tree, "Expecting directive");
		goto exit;
	}

	// TODO

exit:
	return result;
}

static int
dmg_assembler_generator_generate_label(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(tree->parent->type != TOKEN_LABEL) {
		result = GENERATOR_ERROR(generator, tree, "Expecting label");
		goto exit;
	}

	// TODO

exit:
	return result;
}

static int
dmg_assembler_generator_generate_opcode(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(tree->parent->type != TOKEN_OPCODE) {
		result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
		goto exit;
	}

	// TODO

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

	if((result = dmg_assembler_parser_load(&generator->parser, buffer, path)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_banks_allocate(&generator->banks)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_globals_allocate(&generator->globals)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	generator->file = file;

exit:
	return result;
}

int
dmg_assembler_generator_run(
	__inout dmg_assembler_generator_t *generator
	)
{
	int result = DMG_STATUS_SUCCESS;

	for(;;) {
		const dmg_assembler_tree_t *tree = dmg_assembler_parser_tree(&generator->parser);

		switch(tree->parent->type) {
			case TOKEN_DIRECTIVE:

				if((result = dmg_assembler_generator_generate_directive(generator, tree)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
				break;
			case TOKEN_LABEL:

				if((result = dmg_assembler_generator_generate_label(generator, tree)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
				break;
			case TOKEN_OPCODE:

				if((result = dmg_assembler_generator_generate_opcode(generator, tree)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
				break;
			default:
				result = GENERATOR_ERROR(generator, tree, "Expecting statement");
				goto exit;
		}

		if(!dmg_assembler_parser_has_next(&generator->parser)
				|| ((result = dmg_assembler_parser_next(&generator->parser)) != DMG_STATUS_SUCCESS)) {
			break;
		}
	}

	for(uint32_t index = 0; index < generator->banks.count; ++index) {
		dmg_assembler_bank_t *bank = &generator->banks.bank[index];

		if(fwrite(bank->data, sizeof(uint8_t), sizeof(bank->data), generator->file) != sizeof(bank->data)) {
			result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Failed to write bank %u to file", index);
			goto exit;
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
	dmg_assembler_globals_free(&generator->globals);
	dmg_assembler_banks_free(&generator->banks);
	dmg_assembler_parser_unload(&generator->parser);
	memset(generator, 0, sizeof(*generator));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
