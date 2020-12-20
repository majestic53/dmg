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

#include "./asm_type.h"

static dmg_asm_t g_asm = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_utility_asm_assemble(void)
{
	int length, result = EXIT_SUCCESS;

	TRACE_TOOL_MESSAGE("%s -- %.02f KB (%u bytes)\n\n", g_asm.source, g_asm.buffer.length / (float)KBYTE, g_asm.buffer.length);

	// TODO

	fseek(g_asm.file, 0, SEEK_END);
	length = ftell(g_asm.file);
	fseek(g_asm.file, 0, SEEK_SET);
	TRACE_TOOL_MESSAGE("\n%s -- %.02f KB (%u bytes)\n", g_asm.output, length / (float)KBYTE, length);

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
	int result = EXIT_SUCCESS;
	char path_full[PATH_MAX] = {};

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
		result = EXIT_FAILURE;
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
	int option, result = EXIT_SUCCESS;

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
				result = EXIT_FAILURE;
				goto exit;
			default:
				result = EXIT_FAILURE;
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
	int length, result = EXIT_SUCCESS;

	if(!(file = fopen(g_asm.source, "rb"))) {
		result = EXIT_FAILURE;
		goto exit;
	}

	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	if(length <= 0) {
		result = EXIT_FAILURE;
		goto exit;
	}

	if(!(g_asm.buffer.data = (void *)malloc(length))) {
		result = EXIT_FAILURE;
		goto exit;
	}

	if(fread(g_asm.buffer.data, sizeof(uint8_t), length, file) != length) {
		result = EXIT_FAILURE;
		goto exit;
	}

	g_asm.buffer.length = length;

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

static void
dmg_utility_asm_version(
	__in FILE *stream,
	__in bool verbose
	)
{
	const dmg_version_t *version;

	if(verbose) {
		TRACE_TOOL(stream, LEVEL_NONE, "%s", DMG);
	}

	if((version = dmg_version_get())) {

		if(verbose) {
			TRACE_TOOL(stream, LEVEL_NONE, "%s", " ");
		}

		TRACE_TOOL(stream, LEVEL_NONE, "%u.%u.%u\n", version->major, version->minor, version->patch);
	} else {
		TRACE_TOOL(stream, LEVEL_NONE, "%s", "\n");
	}

	if(verbose) {
		TRACE_TOOL(stream, LEVEL_NONE, "%s\n", DMG_NOTICE);
	}
}

static void
dmg_utility_asm_usage(
	__in FILE *stream,
	__in bool verbose
	)
{

	if(verbose) {
		dmg_utility_asm_version(stream, verbose);
		TRACE_TOOL(stream, LEVEL_NONE, "%s", "\n");
	}

	TRACE_TOOL(stream, LEVEL_NONE, "%s\n", DMG_USAGE);

	if(verbose) {

		for(int flag = 0; flag < FLAG_MAX; ++flag) {
			TRACE_TOOL(stream, LEVEL_NONE, "\n%s\t%s", FLAG_STR[flag], FLAG_DESCRIPTION_STR[flag]);
		}

		TRACE_TOOL(stream, LEVEL_NONE, "%s", "\n");
	}
}

int
main(
	__in int argc,
	__in char *argv[]
	)
{
	int result = EXIT_SUCCESS;

	if((result = dmg_utility_asm_parse(argc, argv)) != EXIT_SUCCESS) {
		goto exit;
	}

	if(g_asm.help) {
		dmg_utility_asm_usage(stdout, true);
	} else if(g_asm.version) {
		dmg_utility_asm_version(stdout, false);
	} else {

		if((result = dmg_utility_asm_source_load()) != EXIT_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to load source file -- %s\n", argv[0], g_asm.source);
			goto exit;
		}

		if((result = dmg_utility_asm_output_load(argv[0])) != EXIT_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to create output file -- %s\n", argv[0], g_asm.output);
			goto exit;
		}

		if((result = dmg_utility_asm_assemble()) != EXIT_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to disassemble source file -- %s\n", argv[0], g_asm.source);
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
