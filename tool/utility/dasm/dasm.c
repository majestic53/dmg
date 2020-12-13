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

#include "./dasm_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static dmg_dasm_t g_dasm = {};

static void
dmg_utility_dasm_file_close(void)
{

	if(g_dasm.file) {
		fclose(g_dasm.file);
		g_dasm.file = NULL;
	}
}

static int
dmg_utility_dasm_file_disassemble_bank(
	__inout uint32_t *address,
	__in uint32_t bank
	)
{
	int result = EXIT_SUCCESS;

	// TODO: PARSE BANK N

	return result;
}

static int
dmg_utility_dasm_file_disassemble_comment(void)
{
	uint32_t address;
	int result = EXIT_SUCCESS;
	const dmg_version_t *version;
	const dmg_cartridge_header_t *header;

	if((version = dmg_version_get())) {
		fprintf(g_dasm.file, "%c %s %u.%u.%u\n", COMMENT_DELIMITER, DMG, version->major, version->minor, version->patch);
	} else {
		fprintf(g_dasm.file, "\n");
	}

	fprintf(g_dasm.file, "%c %s\n\n%c %s -- %.02f KB (%u bytes)\n\n", COMMENT_DELIMITER, DMG_NOTICE, COMMENT_DELIMITER, g_dasm.rom,
		g_dasm.buffer.length / (float)KBYTE, g_dasm.buffer.length);

	header = (const dmg_cartridge_header_t *)&((uint8_t *)g_dasm.buffer.data)[ADDRESS_HEADER_BEGIN];
	fprintf(g_dasm.file, "%c Title     \"", COMMENT_DELIMITER);

	for(address = 0; address < CARTRIDGE_HEADER_TITLE_LENGTH; ++address) {
		char value = header->title[address];

		if(!value) {
			break;
		}

		if(!isprint(value) && !isspace(value)) {
			fprintf(g_dasm.file, "\\%02x", value);
		} else {
			fprintf(g_dasm.file, "%c", value);
		}
	}

	fprintf(g_dasm.file, "\"\n%c Type      ", COMMENT_DELIMITER);

	switch(header->cgb) {
		case CGB_SUPPORT:
			fprintf(g_dasm.file, "GB/GBC (Gameboy/Gameboy Color)");
			break;
		case CGB_SUPPORT_ONLY:
			fprintf(g_dasm.file, "GBC Only (Gameboy Color Only)");
			result = EXIT_FAILURE;
			break;
		default:
			fprintf(g_dasm.file, "GB (Gameboy)");
			break;
	}

	if(header->sgb == SGB_SUPPORT) {
		fprintf(g_dasm.file, ", SBC (Super Gameboy)");
	}

	fprintf(g_dasm.file, "\n%c Region    %s", COMMENT_DELIMITER, header->destination ? "U (International)" : "JP (Japan)");
	fprintf(g_dasm.file, "\n%c Mapper    ", COMMENT_DELIMITER);

	if((header->mapper >= MAPPER_MAX) || !strlen(MAPPER_STR[header->mapper])) {
		result = EXIT_FAILURE;
	} else {
		fprintf(g_dasm.file, "%s\n", MAPPER_STR[header->mapper]);
	}

	fprintf(g_dasm.file, "%c Rom       ", COMMENT_DELIMITER);

	if(header->rom >= ROM_MAX) {
		result = EXIT_FAILURE;
	} else {
		fprintf(g_dasm.file, "%s\n", ROM_STR[header->rom]);
	}

	fprintf(g_dasm.file, "%c Ram       ", COMMENT_DELIMITER);

	if(header->ram >= RAM_MAX) {
		result = EXIT_FAILURE;
	} else {
		fprintf(g_dasm.file, "%s\n\n", RAM_STR[header->ram]);
	}

	return result;
}

static int
dmg_utility_dasm_file_disassemble_header(
	__inout uint32_t *address,
	__inout uint32_t *banks
	)
{
	int result = EXIT_SUCCESS;
	const dmg_cartridge_header_t *header;

	header = (const dmg_cartridge_header_t *)&((uint8_t *)g_dasm.buffer.data)[ADDRESS_HEADER_BEGIN];
	*banks = ROM_BANK[header->rom];

	// TODO: PARSE HEADER (0x0100-0x014f)

	return result;
}

static int
dmg_utility_dasm_file_disassemble_vectors(
	__inout uint32_t *address
	)
{
	int result = EXIT_SUCCESS;

	// TODO: PARSE VECTOR TABLE (0x0000-0x00ff)

	return result;
}

static int
dmg_utility_dasm_file_disassemble(void)
{
	int result = EXIT_SUCCESS;
	uint32_t address = 0, banks, length = 0;

	fprintf(stdout, "%s -- %.02f KB (%u bytes)\n\n", g_dasm.rom, g_dasm.buffer.length / (float)KBYTE, g_dasm.buffer.length);

	if(g_dasm.buffer.length <= ADDRESS_HEADER_END) {
		fprintf(stderr, "File is too small -- %.02f KB (%u bytes) (expecting > %.02f KB (%i bytes))\n", g_dasm.buffer.length / (float)KBYTE,
			g_dasm.buffer.length, ADDRESS_HEADER_END / (float)KBYTE, ADDRESS_HEADER_END);
		result = EXIT_FAILURE;
		goto exit;
	}

	if((result = dmg_utility_dasm_file_disassemble_comment()) != EXIT_SUCCESS) {
		goto exit;
	}

	fprintf(stdout, "Parsing vector table...  ");

	if((result = dmg_utility_dasm_file_disassemble_vectors(&address)) != EXIT_SUCCESS) {
		goto exit;
	}

	LEVEL_COLOR(stdout, LEVEL_INFORMATION);
	fprintf(stdout, "[Done]\n");
	LEVEL_COLOR(stdout, LEVEL_NONE);
	fprintf(stdout, "Parsing header...        ");

	if((result = dmg_utility_dasm_file_disassemble_header(&address, &banks)) != EXIT_SUCCESS) {
		goto exit;
	}

	LEVEL_COLOR(stdout, LEVEL_INFORMATION);
	fprintf(stdout, "[Done]\n");
	LEVEL_COLOR(stdout, LEVEL_NONE);

	for(uint32_t bank = 0; bank < banks; ++bank) {
		fprintf(stdout, "Parsing bank[%02x]...      ", bank);

		if((result = dmg_utility_dasm_file_disassemble_bank(&address, bank)) != EXIT_SUCCESS) {
			goto exit;
		}

		LEVEL_COLOR(stdout, LEVEL_INFORMATION);
		fprintf(stdout, "[Done]\n");
		LEVEL_COLOR(stdout, LEVEL_NONE);
	}

	fseek(g_dasm.file, 0, SEEK_END);
	length = ftell(g_dasm.file);
	fseek(g_dasm.file, 0, SEEK_SET);
	fprintf(stdout, "\n%s -- %.02f KB (%u bytes)\n", g_dasm.output, length / (float)KBYTE, length);

exit:
	return result;
}

static int
dmg_utility_dasm_file_open(
	__in const char *path
	)
{
	int result = EXIT_SUCCESS;
	char path_full[PATH_MAX] = {};

	if(!g_dasm.output || !strlen(g_dasm.output)) {
		char *path_end;

		if((path_end = strrchr(path, PATH_DELIMITER))) {
			memcpy(path_full, path, (path_end - path) + 1);
		}

		strcat(path_full, PATH_OUTPUT);
	} else {
		strcpy(path_full, g_dasm.output);
	}

	if(!(g_dasm.file = fopen(path_full, "wb"))) {
		result = EXIT_FAILURE;
		goto exit;
	}

exit:
	return result;
}

static int
dmg_utility_dasm_file_load(void)
{
	FILE *file = NULL;
	int length, result = EXIT_SUCCESS;

	if(!(file = fopen(g_dasm.rom, "rb"))) {
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

	if(!(g_dasm.buffer.data = (void *)malloc(length))) {
		result = EXIT_FAILURE;
		goto exit;
	}

	if(fread(g_dasm.buffer.data, sizeof(uint8_t), length, file) != length) {
		result = EXIT_FAILURE;
		goto exit;
	}

	g_dasm.buffer.length = length;

exit:

	if(file) {
		fclose(file);
		file = NULL;
	}

	return result;
}

static void
dmg_utility_dasm_file_unload(void)
{

	if(g_dasm.buffer.data) {
		free(g_dasm.buffer.data);
	}

	memset(&g_dasm.buffer, 0, sizeof(g_dasm.buffer));
}

static int
dmg_utility_dasm_parse(
	__in int argc,
	__in char *argv[]
	)
{
	int option, result = EXIT_SUCCESS;

	opterr = 1;

	while((option = getopt(argc, argv, OPTIONS)) != -1) {

		switch(option) {
			case OPTION_HELP:
				g_dasm.help = true;
				break;
			case OPTION_OUTPUT:
				g_dasm.output = optarg;
				break;
			case OPTION_ROM:
				g_dasm.rom = optarg;
				break;
			case OPTION_VERSION:
				g_dasm.version = true;
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
dmg_utility_dasm_version(
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
dmg_utility_dasm_usage(
	__in FILE *stream,
	__in bool verbose
	)
{

	if(verbose) {
		dmg_utility_dasm_version(stream, verbose);
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

	if((result = dmg_utility_dasm_parse(argc, argv)) != EXIT_SUCCESS) {
		goto exit;
	}

	if(g_dasm.help) {
		dmg_utility_dasm_usage(stdout, true);
	} else if(g_dasm.version) {
		dmg_utility_dasm_version(stdout, false);
	} else {

		if((result = dmg_utility_dasm_file_load()) != EXIT_SUCCESS) {
			LEVEL_COLOR(stderr, LEVEL_ERROR);
			fprintf(stderr, "%s: Failed to load file -- %s\n", argv[0], g_dasm.rom);
			LEVEL_COLOR(stderr, LEVEL_NONE);
			goto exit;
		}

		if((result = dmg_utility_dasm_file_open(argv[0])) != EXIT_SUCCESS) {
			LEVEL_COLOR(stderr, LEVEL_ERROR);
			fprintf(stderr, "%s: Failed to open file -- %s\n", argv[0], g_dasm.output);
			LEVEL_COLOR(stderr, LEVEL_NONE);
			goto exit;
		}

		if((result = dmg_utility_dasm_file_disassemble()) != EXIT_SUCCESS) {
			LEVEL_COLOR(stderr, LEVEL_ERROR);
			fprintf(stderr, "%s: Failed to disassemble file -- %s\n", argv[0], g_dasm.rom);
			LEVEL_COLOR(stderr, LEVEL_NONE);
			goto exit;
		}
	}

exit:
	dmg_utility_dasm_file_close();
	dmg_utility_dasm_file_unload();
	memset(&g_dasm, 0, sizeof(g_dasm));

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
