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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "./launcher_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static dmg_launcher_t g_launcher = {};

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
dmg_launcher_load(
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
				g_launcher.configuration.transfer = dmg_launcher_capture;
				break;
			case OPTION_HELP:
				g_launcher.help = true;
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
dmg_launcher_setup(void)
{
	memcpy(g_launcher.configuration.button, BUTTON, sizeof(uint32_t) * DMG_BUTTON_MAX);
	memcpy(g_launcher.configuration.direction, DIRECTION, sizeof(uint32_t) * DMG_DIRECTION_MAX);

	if(g_launcher.palette >= PALETTE_MAX) {
		g_launcher.palette = PALETTE_GREY;
	}

	memcpy(g_launcher.configuration.palette, &(PALETTE[g_launcher.palette]), sizeof(uint32_t) * DMG_PALETTE_MAX);

	if(!g_launcher.configuration.scale) {
		g_launcher.configuration.scale = SCALE;
	}

	if(!g_launcher.configuration.transfer) {
		g_launcher.configuration.transfer = TRANSFER;
	}
}

static void
dmg_launcher_unload(void)
{

	if(g_launcher.configuration.rom.data) {
		free(g_launcher.configuration.rom.data);
	}

	if(g_launcher.configuration.bootrom.data) {
		free(g_launcher.configuration.bootrom.data);
	}

	memset(&g_launcher, 0, sizeof(g_launcher));
}

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

	if((result = dmg_launcher_parse(argc, argv)) != EXIT_SUCCESS) {
		goto exit;
	}

	if(g_launcher.help) {
		dmg_launcher_usage(stdout, true);
	} else if(g_launcher.version) {
		dmg_launcher_version(stdout, false);
	} else {
		dmg_launcher_setup();

		if(g_launcher.bootrom) {

			if((result = dmg_launcher_load(&g_launcher.configuration.bootrom, g_launcher.bootrom)) != EXIT_SUCCESS) {
				fprintf(stderr, "%s: Failed to load file -- %s\n", argv[0], g_launcher.bootrom);
				goto exit;
			}
		}

		if((result = dmg_launcher_load(&g_launcher.configuration.rom, g_launcher.rom)) != EXIT_SUCCESS) {
			fprintf(stderr, "%s: Failed to load file -- %s\n", argv[0], g_launcher.rom);
			goto exit;
		}

		if((result = dmg(&g_launcher.configuration)) != EXIT_SUCCESS) {
			fprintf(stderr, "%s: Internal error -- %s\n", argv[0], dmg_error());
			goto exit;
		}
	}

exit:
	dmg_launcher_unload();

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
