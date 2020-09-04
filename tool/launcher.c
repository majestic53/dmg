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

	if(!(buffer->data = (uint8_t *)malloc(length))) {
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
	__inout dmg_launcher_t *launcher,
	__in int argc,
	__in char *argv[]
	)
{
	int option, result = EXIT_SUCCESS;

	opterr = 1;

	while((option = getopt(argc, argv, OPTIONS)) != -1) {

		switch(option) {
			case OPTION_BOOTROM:
				launcher->bootrom = optarg;
				break;
			case OPTION_HELP:
				launcher->help = true;
				break;
			case OPTION_ROM:
				launcher->rom = optarg;
				break;
			case OPTION_VERSION:
				launcher->version = true;
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
dmg_launcher_setup(
	__inout dmg_launcher_t *launcher
	)
{
	memcpy(launcher->configuration.input.button, BUTTON, sizeof(uint32_t) * DMG_BUTTON_MAX);
	memcpy(launcher->configuration.input.direction, DIRECTION, sizeof(uint32_t) * DMG_DIRECTION_MAX);
	memcpy(launcher->configuration.output.palette, PALETTE, sizeof(dmg_color_t) * DMG_PALETTE_MAX);
	launcher->configuration.output.scale = SCALE;
	launcher->configuration.output.transfer = TRANSFER;
}

static void
dmg_launcher_unload(
	__inout dmg_launcher_t *launcher
	)
{

	if(launcher->configuration.rom.data) {
		free(launcher->configuration.rom.data);
	}

	if(launcher->configuration.bootrom.data) {
		free(launcher->configuration.bootrom.data);
	}

	memset(launcher, 0, sizeof(*launcher));
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
	dmg_launcher_t launcher = {};

	if((result = dmg_launcher_parse(&launcher, argc, argv)) != EXIT_SUCCESS) {
		goto exit;
	}

	if(launcher.help) {
		dmg_launcher_usage(stdout, true);
	} else if(launcher.version) {
		dmg_launcher_version(stdout, false);
	} else {
		dmg_launcher_setup(&launcher);

		if(launcher.bootrom) {

			if((result = dmg_launcher_load(&launcher.configuration.bootrom, launcher.bootrom)) != EXIT_SUCCESS) {
				fprintf(stderr, "%s: Failed to load file -- %s\n", argv[0], launcher.bootrom);
				goto exit;
			}
		}

		if((result = dmg_launcher_load(&launcher.configuration.rom, launcher.rom)) != EXIT_SUCCESS) {
			fprintf(stderr, "%s: Failed to load file -- %s\n", argv[0], launcher.rom);
			goto exit;
		}

		if((result = dmg(&launcher.configuration)) != EXIT_SUCCESS) {
			fprintf(stderr, "%s: Internal error -- %s\n", argv[0], dmg_error());
			goto exit;
		}
	}

exit:
	dmg_launcher_unload(&launcher);

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
