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

#include "./asm_type.h"

static dmg_asm_t g_asm = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef ASM_PARSE_CHARACTERS

static int
dmg_utility_asm_parse_characters(void)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_stream_t stream = {};

	if((result = dmg_assembler_stream_load(&stream, &g_asm.buffer, g_asm.source)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	for(;;) {
		int type;
		char value = dmg_assembler_stream_character(&stream, &type);

		fprintf(stdout, "[%u/%u] {%i} \'%c\' (%02x)\n", stream.position, stream.buffer->length, type,
			(isprint(value) && !isspace(value)) ? value : CHARACTER_FILL, value);

		if(!dmg_assembler_stream_has_next(&stream)
				|| ((result = dmg_assembler_stream_next(&stream)) != DMG_STATUS_SUCCESS)) {
			break;
		}
	}

exit:
	dmg_assembler_stream_unload(&stream);

	return result;
}

#endif /* ASM_PARSE_CHARACTERS */

#ifdef ASM_PARSE_TOKENS

static int
dmg_utility_asm_parse_tokens(void)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_lexer_t lexer = {};

	if((result = dmg_assembler_lexer_load(&lexer, &g_asm.buffer, g_asm.source)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	for(;;) {
		const dmg_assembler_token_t *token = dmg_assembler_lexer_token(&lexer);

		fprintf(stdout, "[%i:%i]", token->type, token->subtype);

		if((token->type > TOKEN_END) && (token->type < TOKEN_MAX)) {
			fprintf(stdout, " \"");

			for(uint32_t index = 0; index < token->literal.length; ++index) {
				fprintf(stdout, "%c", token->literal.str[index]);
			}

			fprintf(stdout, "\"");

			if(token->type == TOKEN_SCALAR) {
				fprintf(stdout, " %04x (%u)", token->scalar.word, token->scalar.word);
			}
		} else {
			fprintf(stdout, " \'%c\' (%02x)", (isprint(token->scalar.low) && !isspace(token->scalar.low))
				? token->scalar.low : CHARACTER_FILL, token->scalar.low);
		}

		fprintf(stdout, " (%s@%u)\n", lexer.stream.path, token->line);

		if(!dmg_assembler_lexer_has_next(&lexer)
				|| ((result = dmg_assembler_lexer_next(&lexer)) != DMG_STATUS_SUCCESS)) {
			break;
		}
	}

exit:
	dmg_assembler_lexer_unload(&lexer);

	return result;
}

#endif /* ASM_PARSE_TOKENS */

#ifdef ASM_PARSE_TREES

static int
dmg_utility_asm_parse_tree(
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__in uint32_t depth
	)
{
	int result = DMG_STATUS_SUCCESS;

	for(uint32_t index = 0; index < depth; ++index) {
		fprintf(stdout, "\t");
	}

	if(tree && tree->parent) {
		const dmg_assembler_token_t *token = tree->parent;

		fprintf(stdout, "{%u} [%i:%i]", tree->count, token->type, token->subtype);

		if((token->type > TOKEN_END) && (token->type < TOKEN_MAX)) {
			fprintf(stdout, " \"");

			for(uint32_t index = 0; index < token->literal.length; ++index) {
				fprintf(stdout, "%c", token->literal.str[index]);
			}

			fprintf(stdout, "\"");

			if(token->type == TOKEN_SCALAR) {
				fprintf(stdout, " %04x (%u)", token->scalar.word, token->scalar.word);
			}
		} else {
			fprintf(stdout, " \'%c\' (%02x)", (isprint(token->scalar.low) && !isspace(token->scalar.low))
				? token->scalar.low : CHARACTER_FILL, token->scalar.low);
		}

		fprintf(stdout, " (%s@%u)\n", parser->lexer.stream.path, token->line);

		for(uint32_t index = 0; index < tree->count; ++index) {

			if((result = dmg_utility_asm_parse_tree(parser, (const dmg_assembler_tree_t *)tree->child[index], depth + 1))
					!= DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}
	} else {
		fprintf(stdout, "EOF\n");
	}

exit:
	return result;
}

static int
dmg_utility_asm_parse_trees(void)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_parser_t parser = {};

	if((result = dmg_assembler_parser_load(&parser, &g_asm.buffer, g_asm.source)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	for(;;) {

		if((result = dmg_utility_asm_parse_tree(&parser, dmg_assembler_parser_tree(&parser), 0)) != DMG_STATUS_SUCCESS) {
			break;
		}

		if(!dmg_assembler_parser_has_next(&parser)
				|| ((result = dmg_assembler_parser_next(&parser)) != DMG_STATUS_SUCCESS)) {
			break;
		}
	}

exit:
	dmg_assembler_parser_unload(&parser);

	return result;
}

#endif /* ASM_PARSE_TOKENS */

static int
dmg_utility_asm_assemble(void)
{
	int length, result = DMG_STATUS_SUCCESS;

	TRACE_TOOL_MESSAGE("%s -- %.02f KB (%u bytes)\n\n", g_asm.source, g_asm.buffer.length / (float)KBYTE, g_asm.buffer.length);

#ifdef ASM_PARSE_CHARACTERS
	if((result = dmg_utility_asm_parse_characters()) != DMG_STATUS_SUCCESS) {
		goto exit;
	}
#endif /* ASM_PARSE_CHARACTERS */

#ifdef ASM_PARSE_TOKENS
	if((result = dmg_utility_asm_parse_tokens()) != DMG_STATUS_SUCCESS) {
		goto exit;
	}
#endif /* ASM_PARSE_TOKENS */

#ifdef ASM_PARSE_TREES
	if((result = dmg_utility_asm_parse_trees()) != DMG_STATUS_SUCCESS) {
		goto exit;
	}
#endif /* ASM_PARSE_TOKENS */

	fseek(g_asm.file, 0, SEEK_END);
	length = ftell(g_asm.file);
	fseek(g_asm.file, 0, SEEK_SET);
	TRACE_TOOL_MESSAGE("\n%s -- %.02f KB (%u bytes)\n", g_asm.output, length / (float)KBYTE, length);

exit:
	return result;
}

static void
dmg_utility_asm_output_unload(void)
{

	if(g_asm.file) {
		fclose(g_asm.file);
		g_asm.file = NULL;
	}
}

static int
dmg_utility_asm_output_load(
	__in const char *path
	)
{
	char path_full[PATH_MAX] = {};
	int result = DMG_STATUS_SUCCESS;

	if(!g_asm.output || !strlen(g_asm.output)) {
		char *path_end;

		if((path_end = strrchr(path, PATH_DELIMITER))) {
			memcpy(path_full, path, (path_end - path) + 1);
		}

		strcat(path_full, PATH_OUTPUT);
	} else {
		strcpy(path_full, g_asm.output);
	}

	if(!(g_asm.file = fopen(path_full, "wb"))) {
		result = DMG_STATUS_FAILURE;
		goto exit;
	}

exit:
	return result;
}

static int
dmg_utility_asm_parse(
	__in int argc,
	__in char *argv[]
	)
{
	int option, result = DMG_STATUS_SUCCESS;

	opterr = 1;

	while((option = getopt(argc, argv, OPTIONS)) != -1) {

		switch(option) {
			case OPTION_HELP:
				g_asm.help = true;
				break;
			case OPTION_OUTPUT:
				g_asm.output = optarg;
				break;
			case OPTION_SOURCE:
				g_asm.source = optarg;
				break;
			case OPTION_VERSION:
				g_asm.version = true;
				break;
			case '?':
				result = DMG_STATUS_FAILURE;
				goto exit;
			default:
				result = DMG_STATUS_FAILURE;
				goto exit;
		}
	}

exit:
	return result;
}

static int
dmg_utility_asm_source_load(void)
{
	FILE *file = NULL;
	int length, result = DMG_STATUS_SUCCESS;

	if(!(file = fopen(g_asm.source, "rb"))) {
		result = DMG_STATUS_FAILURE;
		goto exit;
	}

	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	if(length < 0) {
		result = DMG_STATUS_FAILURE;
		goto exit;
	}

	memset(&g_asm.buffer, 0, sizeof(g_asm.buffer));

	if(length > 0) {

		if(!(g_asm.buffer.data = (void *)malloc(length))) {
			result = DMG_STATUS_FAILURE;
			goto exit;
		}

		if(fread(g_asm.buffer.data, sizeof(uint8_t), length, file) != length) {
			result = DMG_STATUS_FAILURE;
			goto exit;
		}

		g_asm.buffer.length = length;
	}

exit:

	if(file) {
		fclose(file);
		file = NULL;
	}

	return result;
}

static void
dmg_utility_asm_source_unload(void)
{

	if(g_asm.buffer.data) {
		free(g_asm.buffer.data);
	}

	memset(&g_asm.buffer, 0, sizeof(g_asm.buffer));
}

int
main(
	__in int argc,
	__in char *argv[]
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((result = dmg_utility_asm_parse(argc, argv)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(g_asm.help) {
		dmg_tool_usage(stdout, true, DMG_USAGE, FLAG_STR, FLAG_DESCRIPTION_STR, FLAG_MAX);
	} else if(g_asm.version) {
		dmg_tool_version(stdout, false);
	} else {

		if((result = dmg_utility_asm_source_load()) != DMG_STATUS_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to load source file -- %s\n", argv[0], g_asm.source);
			goto exit;
		}

		if((result = dmg_utility_asm_output_load(argv[0])) != DMG_STATUS_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to create output file -- %s\n", argv[0], g_asm.output);
			goto exit;
		}

		if((result = dmg_utility_asm_assemble()) != DMG_STATUS_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to assemble source file -- %s\n", argv[0], dmg_error_get());
			goto exit;
		}
	}

exit:
	dmg_utility_asm_output_unload();
	dmg_utility_asm_source_unload();
	memset(&g_asm, 0, sizeof(g_asm));

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
