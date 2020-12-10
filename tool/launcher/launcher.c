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

#include "./launcher_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static dmg_launcher_t g_launcher = {};

static void
dmg_launcher_version(
	__in FILE *stream,
	__in bool verbose
	)
{
	const dmg_version_t *version;

	if(verbose) {
		fprintf(stream, "%s", DMG);
	}

	if((version = dmg_version())) {

		if(verbose) {
			fprintf(stream, " ");
		}

		fprintf(stream, "%u.%u.%u\n", version->major, version->minor, version->patch);
	} else {
		fprintf(stream, "\n");
	}

	if(verbose) {
		fprintf(stream, "%s\n", DMG_NOTICE);
	}
}

static unsigned
dmg_launcher_capture(
	__in unsigned in
	)
{
	g_launcher.capture.data <<= DATA_SHIFT;
	g_launcher.capture.data |= (in & DATA_MASK);

	if(++g_launcher.capture.length == CHAR_BIT) {
		g_launcher.capture.length = 0;

		if(!isprint(g_launcher.capture.data) && !isspace(g_launcher.capture.data)) {
			fprintf(stdout, "\\%02x", g_launcher.capture.data);
		} else {
			fprintf(stdout, "%c", g_launcher.capture.data);
		}

		fflush(stdout);
	}

	return UINT8_MAX;
}

static int
dmg_launcher_debug_help(
	__in const char *argument[],
	__in uint32_t count
	)
{

	for(int debug = 0; debug < DEBUG_MAX; ++debug) {
		fprintf(stdout, "%c\t%s\n", DEBUG_CHAR[debug], DEBUG_DESCRIPTION_STR[debug]);
	}

	return DMG_STATUS_SUCCESS;
}

static int
dmg_launcher_debug_read(
	__in const char *argument[],
	__in uint32_t count
	)
{
	uint16_t address, offset = 1;
	int result = DMG_STATUS_SUCCESS;
	char str[ARGUMENT_READ_WIDTH + 1] = {};
	dmg_action_t request = {}, response = {};

	switch(count) {
		case (ARGUMENT_READ + 1):
			offset = strtoul(argument[1], NULL, 16);
		case ARGUMENT_READ:
			address = strtoul(argument[0], NULL, 16);
			break;
		default:
			result = DMG_STATUS_INVALID;
			goto exit;
	}

	LEVEL_COLOR(stdout, LEVEL_VERBOSE);
	fprintf(stdout, "Address: %04x\n", address);
	fprintf(stdout, "Offset: %04x\n", offset);
	LEVEL_COLOR(stdout, LEVEL_NONE);
	request.type = DMG_ACTION_READ;
	count = 0;

	for(uint32_t index = address; index < (address + offset); ++index) {
		request.address = (index % (UINT16_MAX + 1));

		if(count == ARGUMENT_READ_WIDTH) {
			count = 0;
		}

		if(!count) {

			if(strlen(str)) {
				fprintf(stdout, "\t%s", str);
				memset(str, 0, sizeof(str));
			}

			fprintf(stdout, "\n%04x |", request.address);
		}

		if((result = dmg_action(&request, &response)) == DMG_STATUS_SUCCESS) {
			str[count] = ((isprint((char)response.data.byte) && !isspace((char)response.data.byte))
					? response.data.byte : CHARACTER_FILL);
			fprintf(stdout, " %02x", response.data.byte);
		}

		++count;
	}

	if(offset) {

		if(strlen(str)) {
			fprintf(stdout, "\t%s", str);
			memset(str, 0, sizeof(str));
		}

		fprintf(stdout, "\n");
	}

exit:
	return result;
}

static int
dmg_launcher_debug_run(
	__in const char *argument[],
	__in uint32_t count
	)
{
	uint16_t breakpoint[ARGUMENT_MAX] = {};

	for(uint32_t index = 0; index < count; ++index) {
		breakpoint[index] = strtoul(argument[index], NULL, 16);
	}

	if(count) {
		LEVEL_COLOR(stdout, LEVEL_VERBOSE);
		fprintf(stdout, "Breakpoint[%u]: {", count);

		for(uint32_t index = 0; index < count; ++index) {
			fprintf(stdout, " %04x", breakpoint[index]);
		}

		fprintf(stdout, " }\n");
		LEVEL_COLOR(stdout, LEVEL_NONE);
	}

	return dmg_run(breakpoint, count);
}

static int
dmg_launcher_debug_step(
	__in const char *argument[],
	__in uint32_t count
	)
{
	uint16_t breakpoint[ARGUMENT_MAX] = {}, instruction = 1;

	if(count >= 1) {
		instruction = strtoul(argument[0], NULL, 10);

		for(uint32_t index = 1; index < count; ++index) {
			breakpoint[index - 1] = strtoul(argument[index], NULL, 16);
		}

		--count;
	}

	LEVEL_COLOR(stdout, LEVEL_VERBOSE);
	fprintf(stdout, "Instructions: %u\n", instruction);

	if(count) {
		fprintf(stdout, "Breakpoint[%u]: {", count);

		for(uint32_t index = 0; index < count; ++index) {
			fprintf(stdout, " %04x", breakpoint[index]);
		}

		fprintf(stdout, " }\n");
	}

	LEVEL_COLOR(stdout, LEVEL_NONE);

	return dmg_step(instruction, breakpoint, count);
}

static int
dmg_launcher_debug_version(
	__in const char *argument[],
	__in uint32_t count
	)
{
	dmg_launcher_version(stdout, false);

	return DMG_STATUS_SUCCESS;
}

static int
dmg_launcher_debug_write(
	__in const char *argument[],
	__in uint32_t count
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_action_t request = {}, response = {};

	switch(count) {
		case (ARGUMENT_WRITE + 1):
			count = strtoul(argument[2], NULL, 16);
			break;
		case ARGUMENT_WRITE:
			count = 1;
			break;
		default:
			result = DMG_STATUS_INVALID;
			goto exit;
	}

	request.address = strtoul(argument[0], NULL, 16);
	request.data.byte = strtoul(argument[1], NULL, 16);
	LEVEL_COLOR(stdout, LEVEL_VERBOSE);
	fprintf(stdout, "Address: %04x\n", request.address);
	fprintf(stdout, "Value: %02x\n", request.data.byte);
	fprintf(stdout, "Count: %04x\n", count);
	LEVEL_COLOR(stdout, LEVEL_NONE);
	request.type = DMG_ACTION_WRITE;

	for(uint32_t index = 0; index < count; ++index) {
		result = dmg_action(&request, &response);
		++request.address;
	}

exit:
	return result;
}

static const dmg_launcher_debug_hdlr DEBUG_HANDLER[] = {
	NULL, /* DEBUG_EXIT */
	dmg_launcher_debug_help, /* DEBUG_HELP */
	dmg_launcher_debug_read, /* DEBUG_READ */
	dmg_launcher_debug_run,/* DEBUG_RUN */
	dmg_launcher_debug_step, /* DEBUG_STEP */
	dmg_launcher_debug_version, /* DEBUG_VERSION */
	dmg_launcher_debug_write, /* DEBUG_WRITE */
	};

static int
dmg_launcher_debug_header(
	__in const char *path
	)
{
	int result;
	char *path_end, path_full[PATH_MAX] = {};

	if((path_end = strrchr(path, '/'))) {
		memcpy(path_full, path, (path_end - path) + 1);
	}

	LEVEL_COLOR(stdout, LEVEL_INFORMATION);
	dmg_launcher_version(stdout, true);
	LEVEL_COLOR(stdout, LEVEL_VERBOSE);
	fprintf(stdout, "\n");
	strcat(path_full, PATH_ROM_INFO);
	strcat(path_full, g_launcher.rom);

	if((result = system(path_full)) != EXIT_SUCCESS) {
		goto exit;
	}

	if(g_launcher.configuration.save_in) {
		fprintf(stdout, "\n");
		memset(path_full, 0, sizeof(path_full));

		if((path_end = strrchr(path, '/'))) {
			memcpy(path_full, path, (path_end - path) + 1);
		}

		strcat(path_full, PATH_SAVE_INFO);
		strcat(path_full, g_launcher.configuration.save_in);

		if((result = system(path_full)) != EXIT_SUCCESS) {
			goto exit;
		}
	}

exit:
	LEVEL_COLOR(stdout, LEVEL_NONE);

	return result;
}

static int
dmg_launcher_debug_prompt(
	__in char *prompt,
	__in uint32_t length
	)
{
	int result;
	dmg_action_t request = {}, response = {};

	request.type = DMG_ACTION_CYCLE;

	if((result = dmg_action(&request, &response)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

#ifdef COLOR
	snprintf(prompt, length, "%s%s%u%s%s", LEVEL_STR[LEVEL_INFORMATION], PROMPT_PREFIX, response.data.word, PROMPT_POSTFIX,
			LEVEL_STR[LEVEL_NONE]);
#else
	snprintf(prompt, length, "%s%u%s", PROMPT_PREFIX, response.data.word, PROMPT_POSTFIX);
#endif /* COLOR */

exit:
	return result;
}

static int
dmg_launcher_debug(
	__in const char *path
	)
{
	int result;
	bool complete = false;

	stifle_history(HISTORY_MAX);

	if((result = dmg_launcher_debug_header(path)) != EXIT_SUCCESS) {
		goto exit;
	}

	while(!complete) {
		char *input, prompt[PROMPT_MAX] = {};

		if((result = dmg_launcher_debug_prompt(prompt, PROMPT_MAX)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((input = readline(prompt))) {

			if(strlen(input)) {
				int debug = 0;
				bool first = true;
				uint32_t count = 0;
				dmg_action_t request = {}, response = {};
				const char *argument[ARGUMENT_MAX] = {}, *next;

				for(; debug < DEBUG_MAX; ++debug) {

					if(input[0] == DEBUG_CHAR[debug]) {
						break;
					}
				}

				if(debug >= DEBUG_MAX) {
					LEVEL_COLOR(stderr, LEVEL_ERROR);
					fprintf(stderr, "Unsupported command: %s\n", input);
					LEVEL_COLOR(stderr, LEVEL_NONE);
					goto cleanup;
				}

				add_history(input);
				next = strtok(input, " ");

				while(next) {

					if(count >= ARGUMENT_MAX) {
						LEVEL_COLOR(stderr, LEVEL_ERROR);
						fprintf(stderr, "Too many arguments: %u\n", count + 1);
						LEVEL_COLOR(stderr, LEVEL_NONE);
						goto cleanup;
					}

					if(!first) {
						argument[count++] = next;
					}

					next = strtok(NULL, " ");
					first = false;
				}

				switch(debug) {
					case DEBUG_EXIT:
						complete = true;
						break;
					case (DEBUG_EXIT + 1) ... (DEBUG_MAX - 1):

						switch(DEBUG_HANDLER[debug](argument, count)) {
							case DMG_STATUS_SUCCESS:
								break;
							case DMG_STATUS_BREAKPOINT:
								LEVEL_COLOR(stdout, LEVEL_WARNING);
								request.type = DMG_ACTION_READ;
								request.address = DMG_REGISTER_PC;
								request.data.dword = UINT32_MAX;

								if(dmg_action(&request, &response) == DMG_STATUS_SUCCESS) {
									fprintf(stdout, "Breakpoint: %04x\n", response.data.word);
								} else {
									fprintf(stdout, "Breakpoint\n");
								}

								LEVEL_COLOR(stdout, LEVEL_NONE);
								break;
							default:
								LEVEL_COLOR(stderr, LEVEL_ERROR);
								fprintf(stderr, "Command failed: %s\n", input);
								LEVEL_COLOR(stderr, LEVEL_NONE);
								break;
						}
						break;
					default:
						LEVEL_COLOR(stderr, LEVEL_ERROR);
						fprintf(stderr, "Unsupported command type: %i\n", debug);
						LEVEL_COLOR(stderr, LEVEL_NONE);
						break;
				}
			}

cleanup:
			free(input);
		}
	}

exit:
	return result;
}

static int
dmg_launcher_file_load(
	__inout dmg_buffer_t *buffer,
	__in const char *path
	)
{
	FILE *file = NULL;
	int length, result = EXIT_SUCCESS;

	if(!(file = fopen(path, "rb"))) {
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

	if(!(buffer->data = (void *)malloc(length))) {
		result = EXIT_FAILURE;
		goto exit;
	}

	if(fread(buffer->data, sizeof(uint8_t), length, file) != length) {
		result = EXIT_FAILURE;
		goto exit;
	}

	buffer->length = length;

exit:

	if(file) {
		fclose(file);
		file = NULL;
	}

	return result;
}

static void
dmg_launcher_file_unload(
	__inout dmg_buffer_t *buffer
	)
{

	if(buffer->data) {
		free(buffer->data);
	}

	memset(buffer, 0, sizeof(*buffer));
}

static int
dmg_launcher_parse(
	__in int argc,
	__in char *argv[]
	)
{
	int option, result = EXIT_SUCCESS;

	opterr = 1;

	while((option = getopt(argc, argv, OPTIONS)) != -1) {

		switch(option) {
			case OPTION_BOOTROM:
				g_launcher.bootrom = optarg;
				break;
			case OPTION_CAPTURE:
				g_launcher.configuration.serial_out = dmg_launcher_capture;
				break;
			case OPTION_DEBUG:
				g_launcher.debug = true;
				break;
			case OPTION_HELP:
				g_launcher.help = true;
				break;
			case OPTION_INPUT:
				g_launcher.configuration.save_in = optarg;
				break;
			case OPTION_OUTPUT:
				g_launcher.configuration.save_out = optarg;
				break;
			case OPTION_PALETTE:
				g_launcher.palette = strtol(optarg, NULL, 10);
				break;
			case OPTION_ROM:
				g_launcher.rom = optarg;
				break;
			case OPTION_SCALE:
				g_launcher.configuration.scale = strtol(optarg, NULL, 10);
				break;
			case OPTION_VERSION:
				g_launcher.version = true;
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

static void
dmg_launcher_usage(
	__in FILE *stream,
	__in bool verbose
	)
{

	if(verbose) {
		dmg_launcher_version(stream, verbose);
		fprintf(stream, "\n");
	}

	fprintf(stream, "%s\n", DMG_USAGE);

	if(verbose) {

		for(int flag = 0; flag < FLAG_MAX; ++flag) {
			fprintf(stream, "\n%s\t%s", FLAG_STR[flag], FLAG_DESCRIPTION_STR[flag]);
		}

		fprintf(stream, "\n");
	}
}

int
main(
	__in int argc,
	__in char *argv[]
	)
{
	int result = EXIT_SUCCESS;

	g_launcher.palette = DEFAULT_PALETTE;

	if((result = dmg_launcher_parse(argc, argv)) != EXIT_SUCCESS) {
		goto exit;
	}

	if(g_launcher.help) {
		dmg_launcher_usage(stdout, true);
	} else if(g_launcher.version) {
		dmg_launcher_version(stdout, false);
	} else {
		memcpy(g_launcher.configuration.button, BUTTON, sizeof(uint32_t) * DMG_BUTTON_MAX);
		memcpy(g_launcher.configuration.direction, DIRECTION, sizeof(uint32_t) * DMG_DIRECTION_MAX);

		if(g_launcher.palette >= PALETTE_MAX) {
			g_launcher.palette = DEFAULT_PALETTE;
		}

		memcpy(g_launcher.configuration.palette, &(PALETTE[g_launcher.palette]), sizeof(uint32_t) * DMG_PALETTE_MAX);

		if(!g_launcher.configuration.scale) {
			g_launcher.configuration.scale = DEFAULT_SCALE;
		}

		if(!g_launcher.configuration.serial_out) {
			g_launcher.configuration.serial_out = DEFAULT_OUT;
		}

		if(g_launcher.bootrom) {

			if((result = dmg_launcher_file_load(&g_launcher.configuration.bootrom, g_launcher.bootrom)) != EXIT_SUCCESS) {
				fprintf(stderr, "%s: Failed to load file -- %s\n", argv[0], g_launcher.bootrom);
				goto exit;
			}
		}

		if((result = dmg_launcher_file_load(&g_launcher.configuration.rom, g_launcher.rom)) != EXIT_SUCCESS) {
			fprintf(stderr, "%s: Failed to load file -- %s\n", argv[0], g_launcher.rom);
			goto exit;
		}

		if((result = ((dmg_load(&g_launcher.configuration) == DMG_STATUS_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE))) {
			fprintf(stderr, "%s: Internal error -- %s\n", argv[0], dmg_error());
			result = EXIT_FAILURE;
			goto exit;
		}

		if(g_launcher.debug) {
			result = ((dmg_launcher_debug(argv[0]) == DMG_STATUS_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE);
		} else {
			result = ((dmg_run(NULL, 0) == DMG_STATUS_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE);
		}
	}

exit:
	dmg_unload();
	dmg_launcher_file_unload(&g_launcher.configuration.rom);
	dmg_launcher_file_unload(&g_launcher.configuration.bootrom);
	memset(&g_launcher, 0, sizeof(g_launcher));

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
