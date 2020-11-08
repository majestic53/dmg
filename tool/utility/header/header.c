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

#include "./header_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static dmg_utility_header_t g_utility_header = {};

static int
dmg_utility_header_file_parse(
	__in const dmg_buffer_t *buffer
	)
{
	uint16_t checksum = 0;
	int result = EXIT_SUCCESS;
	const dmg_cartridge_header_t *header;

	fprintf(stdout, "%s -- %.02f KB (%u bytes)\n\n", g_utility_header.rom, g_utility_header.buffer.length / (float)KBYTE, g_utility_header.buffer.length);

	if(buffer->length <= ADDRESS_HEADER_END) {
		fprintf(stderr, "File is too small -- %.02f KB (%u bytes) (expecting > %.02f KB (%u bytes))\n", g_utility_header.buffer.length / (float)KBYTE,
			g_utility_header.buffer.length, buffer->length / (float)KBYTE, buffer->length);
		result = EXIT_FAILURE;
		goto exit;
	}

	header = (const dmg_cartridge_header_t *)&((uint8_t *)buffer->data)[ADDRESS_HEADER_BEGIN];
	fprintf(stdout, "Title[%zu]\t\"%s\"\n", strlen(header->title), header->title);
	fprintf(stdout, "Mapper[%u]\t%u (%s)\n", header->mapper, header->mapper, header->mapper < MAPPER_MAX ? MAPPER_STR[header->mapper] : "UNSUPORTED");
	fprintf(stdout, "Rom[%u]\t\t%zu (%s)\n", header->rom, ROM_BANK[header->rom], (header->rom < ROM_MAX) ? ROM_STR[header->rom] : "UNSUPORTED");
	fprintf(stdout, "Ram[%u]\t\t%zu (%s)\n", header->ram, RAM_BANK[header->ram], (header->ram < RAM_MAX) ? RAM_STR[header->ram] : "UNSUPORTED");
	fprintf(stdout, "Checksum[%u]\t%02x (", header->checksum, header->checksum);

	for(uint32_t address = ADDRESS_HEADER_CHECKSUM_BEGIN; address <= ADDRESS_HEADER_CHECKSUM_END; ++address) {
		checksum = (checksum - ((uint8_t *)buffer->data)[address] - 1);
	}

	checksum &= UINT8_MAX;
	fprintf(stdout, "%s)\n", (header->checksum == checksum) ? "MATCH" : "MISMATCH");

exit:
	return result;
}

static int
dmg_utility_header_parse(
	__in int argc,
	__in char *argv[]
	)
{
	int option, result = EXIT_SUCCESS;

	opterr = 1;

	while((option = getopt(argc, argv, OPTIONS)) != -1) {

		switch(option) {
			case OPTION_HELP:
				g_utility_header.help = true;
				break;
			case OPTION_ROM:
				g_utility_header.rom = optarg;
				break;
			case OPTION_VERSION:
				g_utility_header.version = true;
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
dmg_utility_header_version(
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
dmg_utility_header_usage(
	__in FILE *stream,
	__in bool verbose
	)
{

	if(verbose) {
		dmg_utility_header_version(stream, verbose);
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

	if((result = dmg_utility_header_parse(argc, argv)) != EXIT_SUCCESS) {
		goto exit;
	}

	if(g_utility_header.help) {
		dmg_utility_header_usage(stdout, true);
	} else if(g_utility_header.version) {
		dmg_utility_header_version(stdout, false);
	} else {

		if((result = dmg_file_load(&g_utility_header.buffer, g_utility_header.rom)) != EXIT_SUCCESS) {
			fprintf(stderr, "%s: Failed to load file -- %s\n", argv[0], g_utility_header.rom);
			goto exit;
		}

		if((result = dmg_utility_header_file_parse(&g_utility_header.buffer)) != EXIT_SUCCESS) {
			fprintf(stderr, "%s: Failed to parse file -- %s\n", argv[0], g_utility_header.rom);
			goto exit;
		}
	}

exit:
	dmg_file_unload(&g_utility_header.buffer);
	memset(&g_utility_header, 0, sizeof(g_utility_header));

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
