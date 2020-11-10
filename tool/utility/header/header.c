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
	uint32_t address;
	uint16_t checksum = 0;
	int result = EXIT_SUCCESS;
	const dmg_cartridge_header_t *header;

	fprintf(stdout, "%s -- %.02f KB (%u bytes)\n\n", g_utility_header.rom, g_utility_header.buffer.length / (float)KBYTE, g_utility_header.buffer.length);

	if(buffer->length <= ADDRESS_HEADER_END) {
		fprintf(stderr, "File is too small -- %.02f KB (%u bytes) (expecting > %.02f KB (%i bytes))\n", g_utility_header.buffer.length / (float)KBYTE,
			g_utility_header.buffer.length, ADDRESS_HEADER_END / (float)KBYTE, ADDRESS_HEADER_END);
		result = EXIT_FAILURE;
		goto exit;
	}

	header = (const dmg_cartridge_header_t *)&((uint8_t *)buffer->data)[ADDRESS_HEADER_BEGIN];
	fprintf(stdout, "Title     | \"");

	for(address = 0; address < CARTRIDGE_HEADER_TITLE_LENGTH; ++address) {
		char value = header->title[address];

		if(!value) {
			break;
		}

		if(!isprint(value) && !isspace(value)) {
			fprintf(stdout, "\\%02x", value);
		} else {
			fprintf(stdout, "%c", value);
		}
	}

	fprintf(stdout, "\"\nMapper    | ");

	if(header->mapper >= MAPPER_MAX) {
		fprintf(stdout, "UNSUPPORTED\n");
		result = EXIT_FAILURE;
	} else {
		fprintf(stdout, "%s\n", MAPPER_STR[header->mapper]);
	}

	fprintf(stdout, "Rom       | ");

	if(header->rom >= ROM_MAX) {
		fprintf(stdout, "UNSUPPORTED\n");
		result = EXIT_FAILURE;
	} else {
		fprintf(stdout, "%s\n", ROM_STR[header->rom]);
	}

	fprintf(stdout, "Ram       | ");

	if(header->ram >= RAM_MAX) {
		fprintf(stdout, "UNSUPPORTED\n");
		result = EXIT_FAILURE;
	} else {
		fprintf(stdout, "%s\n", RAM_STR[header->ram]);
	}

	for(address = ADDRESS_HEADER_CHECKSUM_BEGIN; address <= ADDRESS_HEADER_CHECKSUM_END; ++address) {
		checksum = (checksum - ((uint8_t *)buffer->data)[address] - 1);
	}

	checksum &= UINT8_MAX;
	if(header->checksum != checksum) {
		fprintf(stdout, "Checksum  | MISMATCH (Expecting %02x)\n", checksum);
		result = EXIT_FAILURE;
	}

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

	if((version = dmg_version_get())) {

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

		result = dmg_utility_header_file_parse(&g_utility_header.buffer);
	}

exit:
	dmg_file_unload(&g_utility_header.buffer);
	memset(&g_utility_header, 0, sizeof(g_utility_header));

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
