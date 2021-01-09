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

#include "./launcher_type.h"

static dmg_launcher_t g_launcher = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_launcher_file_load(
	__inout dmg_buffer_t *buffer,
	__in const char *path
	)
{
	FILE *file = NULL;
	int length, result = DMG_STATUS_SUCCESS;

	if(!(file = fopen(path, "rb"))) {
		result = DMG_STATUS_FAILURE;
		goto exit;
	}

	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	if(length <= 0) {
		result = DMG_STATUS_FAILURE;
		goto exit;
	}

	if(!(buffer->data = (void *)malloc(length))) {
		result = DMG_STATUS_FAILURE;
		goto exit;
	}

	if(fread(buffer->data, sizeof(uint8_t), length, file) != length) {
		result = DMG_STATUS_FAILURE;
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
	int option, result = DMG_STATUS_SUCCESS;

	opterr = 1;

	while((option = getopt(argc, argv, OPTIONS)) != -1) {

		switch(option) {
			case OPTION_BOOTROM:
				g_launcher.bootrom = optarg;
				break;
			case OPTION_CAPTURE:
				g_launcher.capture = true;
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
			case OPTION_SERIAL_CLIENT:
			case OPTION_SERIAL_SERVER:
				g_launcher.serial.enable = true;
				g_launcher.serial.client = (option == OPTION_SERIAL_CLIENT);
				g_launcher.serial.port = (uint16_t)strtol(optarg, NULL, 10);
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

static unsigned
dmg_launcher_serial_out(
	__in unsigned in
	)
{
	unsigned result = UINT8_MAX;

	g_launcher.serial.data <<= DATA_SHIFT;
	g_launcher.serial.data |= (in & DATA_MASK);

	if(++g_launcher.serial.length == CHAR_BIT) {
		g_launcher.serial.length = 0;

		if(g_launcher.capture) {

			if(!isprint(g_launcher.serial.data) && !isspace(g_launcher.serial.data)) {
				TRACE_TOOL_MESSAGE("\\%02x", g_launcher.serial.data);
			} else {
				TRACE_TOOL_MESSAGE("%c", g_launcher.serial.data);
			}

			fflush(stdout);
		}
	}

	if(g_launcher.serial.enable) {
		result = dmg_launcher_socket_transfer(&g_launcher.serial.socket, in & DATA_MASK, g_launcher.serial.client);
	}

	return result;
}

int
main(
	__in int argc,
	__in char *argv[]
	)
{
	int result = DMG_STATUS_SUCCESS;

	g_launcher.palette = DEFAULT_PALETTE;

	if((result = dmg_launcher_parse(argc, argv)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(g_launcher.help) {
		dmg_tool_usage(stdout, true, DMG_USAGE, FLAG_STR, FLAG_DESCRIPTION_STR, FLAG_MAX);
	} else if(g_launcher.version) {
		dmg_tool_version(stdout, false);
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

		g_launcher.configuration.serial_out = dmg_launcher_serial_out;

		if(g_launcher.bootrom) {

			if((result = dmg_launcher_file_load(&g_launcher.configuration.bootrom, g_launcher.bootrom)) != DMG_STATUS_SUCCESS) {
				TRACE_TOOL_ERROR("%s: Failed to load bootrom file -- %s\n", argv[0], g_launcher.bootrom);
				goto exit;
			}
		}

		if((result = dmg_launcher_file_load(&g_launcher.configuration.rom, g_launcher.rom)) != DMG_STATUS_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to load rom file -- %s\n", argv[0], g_launcher.rom);
			goto exit;
		}

		if(g_launcher.serial.enable && (result = dmg_launcher_socket_open(&g_launcher.serial.socket, g_launcher.serial.port, g_launcher.serial.client))
				!= DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_load(&g_launcher.configuration)) != DMG_STATUS_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to load -- %s\n", argv[0], dmg_error());
			goto exit;
		}

		if(g_launcher.debug) {

			if((result = dmg_launcher_debug(argv[0], g_launcher.rom, g_launcher.configuration.save_in)) != DMG_STATUS_SUCCESS) {
				TRACE_TOOL_ERROR("%s: Failed to run debugger -- %s\n", argv[0], dmg_error());
				goto exit;
			}
		} else if((result = dmg_run(NULL, 0)) != DMG_STATUS_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to run -- %s\n", argv[0], dmg_error());
			goto exit;
		}
	}

exit:
	dmg_unload();
	dmg_launcher_socket_close(&g_launcher.serial.socket);
	dmg_launcher_file_unload(&g_launcher.configuration.rom);
	dmg_launcher_file_unload(&g_launcher.configuration.bootrom);
	memset(&g_launcher, 0, sizeof(g_launcher));

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
