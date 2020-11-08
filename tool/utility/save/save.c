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

#include "./save_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static dmg_utility_save_t g_utility_save = {};

static int
dmg_utility_save_file_parse(
	__in const dmg_buffer_t *buffer
	)
{
	int result = EXIT_SUCCESS;

	fprintf(stdout, "%s -- %.02f KB (%u bytes)\n\n", g_utility_save.save, g_utility_save.buffer.length / (float)KBYTE, g_utility_save.buffer.length);

	// TODO

	return result;
}

static int
dmg_utility_save_parse(
	__in int argc,
	__in char *argv[]
	)
{
	int option, result = EXIT_SUCCESS;

	opterr = 1;

	while((option = getopt(argc, argv, OPTIONS)) != -1) {

		switch(option) {
			case OPTION_HELP:
				g_utility_save.help = true;
				break;
			case OPTION_SAVE:
				g_utility_save.save = optarg;
				break;
			case OPTION_VERSION:
				g_utility_save.version = true;
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
dmg_utility_save_version(
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
dmg_utility_save_usage(
	__in FILE *stream,
	__in bool verbose
	)
{

	if(verbose) {
		dmg_utility_save_version(stream, verbose);
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

	if((result = dmg_utility_save_parse(argc, argv)) != EXIT_SUCCESS) {
		goto exit;
	}

	if(g_utility_save.help) {
		dmg_utility_save_usage(stdout, true);
	} else if(g_utility_save.version) {
		dmg_utility_save_version(stdout, false);
	} else {

		if((result = dmg_file_load(&g_utility_save.buffer, g_utility_save.save)) != EXIT_SUCCESS) {
			fprintf(stderr, "%s: Failed to load file -- %s\n", argv[0], g_utility_save.save);
			goto exit;
		}

		result = dmg_utility_save_file_parse(&g_utility_save.buffer);
	}

exit:
	dmg_file_unload(&g_utility_save.buffer);
	memset(&g_utility_save, 0, sizeof(g_utility_save));

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
