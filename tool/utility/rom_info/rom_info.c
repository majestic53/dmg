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

#include "./rom_info_type.h"

static dmg_rom_info_t g_rom_info = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_utility_rom_info_parse(
	__in int argc,
	__in char *argv[]
	)
{
	int option, result = EXIT_SUCCESS;

	opterr = 1;

	while((option = getopt(argc, argv, OPTIONS)) != -1) {

		switch(option) {
			case OPTION_HELP:
				g_rom_info.help = true;
				break;
			case OPTION_ROM:
				g_rom_info.rom = optarg;
				break;
			case OPTION_VERSION:
				g_rom_info.version = true;
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
dmg_utility_rom_info_rom_load(void)
{
	FILE *file = NULL;
	int length, result = EXIT_SUCCESS;

	if(!(file = fopen(g_rom_info.rom, "rb"))) {
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

	if(!(g_rom_info.buffer.data = (void *)malloc(length))) {
		result = EXIT_FAILURE;
		goto exit;
	}

	if(fread(g_rom_info.buffer.data, sizeof(uint8_t), length, file) != length) {
		result = EXIT_FAILURE;
		goto exit;
	}

	g_rom_info.buffer.length = length;

exit:

	if(file) {
		fclose(file);
		file = NULL;
	}

	return result;
}

static int
dmg_utility_rom_info_rom_parse(void)
{
	uint32_t address;
	const char *mapper;
	uint16_t checksum = 0;
	int result = EXIT_SUCCESS;
	const dmg_cartridge_header_t *header;

	TRACE_TOOL_MESSAGE("%s -- %.02f KB (%u bytes)\n\n", g_rom_info.rom, g_rom_info.buffer.length / (float)KBYTE, g_rom_info.buffer.length);

	if(g_rom_info.buffer.length <= ADDRESS_HEADER_END) {
		TRACE_TOOL_ERROR("File is too small -- %.02f KB (%u bytes) (expecting > %.02f KB (%i bytes))\n", g_rom_info.buffer.length / (float)KBYTE,
			g_rom_info.buffer.length, ADDRESS_HEADER_END / (float)KBYTE, ADDRESS_HEADER_END);
		result = EXIT_FAILURE;
		goto exit;
	}

	header = (const dmg_cartridge_header_t *)&((uint8_t *)g_rom_info.buffer.data)[ADDRESS_HEADER_BEGIN];
	TRACE_TOOL_MESSAGE("%s", "Title     \"");

	for(address = 0; address < CARTRIDGE_HEADER_TITLE_LENGTH; ++address) {
		char value = header->title[address];

		if(!value) {
			break;
		}

		if(!isprint(value) && !isspace(value)) {
			TRACE_TOOL_MESSAGE("\\%02x", value);
		} else {
			TRACE_TOOL_MESSAGE("%c", value);
		}
	}

	TRACE_TOOL_MESSAGE("%s", "\"\nType      ");

	switch(header->cgb) {
		case CGB_SUPPORT:
			TRACE_TOOL_MESSAGE("%s", "GB/GBC (Gameboy/Gameboy Color)");
			break;
		case CGB_SUPPORT_ONLY:
			TRACE_TOOL_MESSAGE("%s", "GBC Only (Gameboy Color Only)");
			result = EXIT_FAILURE;
			break;
		default:
			TRACE_TOOL_MESSAGE("%s", "GB (Gameboy)");
			break;
	}

	if(header->sgb == SGB_SUPPORT) {
		TRACE_TOOL_MESSAGE("%s", ", SBC (Super Gameboy)");
	}

	TRACE_TOOL_MESSAGE("\nRegion    %s", header->destination ? "U (International)" : "JP (Japan)");
	TRACE_TOOL_MESSAGE("%s", "\nMapper    ");
	mapper = dmg_tool_syntax_mapper_string(header->mapper);

	if((header->mapper >= MAPPER_MAX) || !strlen(mapper)) {
		TRACE_TOOL_MESSAGE("UNSUPPORTED (%u)\n", header->mapper);
		result = EXIT_FAILURE;
	} else {
		TRACE_TOOL_MESSAGE("%s\n", mapper);
	}

	TRACE_TOOL_MESSAGE("%s", "Rom       ");

	if(header->rom >= ROM_MAX) {
		TRACE_TOOL_MESSAGE("%s", "UNSUPPORTED\n");
		result = EXIT_FAILURE;
	} else {
		TRACE_TOOL_MESSAGE("%s\n", dmg_tool_syntax_rom_string(header->rom));
	}

	TRACE_TOOL_MESSAGE("%s", "Ram       ");

	if(header->ram >= RAM_MAX) {
		TRACE_TOOL_MESSAGE("%s", "UNSUPPORTED\n");
		result = EXIT_FAILURE;
	} else {
		TRACE_TOOL_MESSAGE("%s\n", dmg_tool_syntax_ram_string(header->ram));
	}

	for(address = ADDRESS_HEADER_CHECKSUM_BEGIN; address <= ADDRESS_HEADER_CHECKSUM_END; ++address) {
		checksum = (checksum - ((uint8_t *)g_rom_info.buffer.data)[address] - 1);
	}

	if(header->checksum != (checksum &= UINT8_MAX)) {
		TRACE_TOOL_MESSAGE("Checksum  MISMATCH (Expecting %02x)\n", checksum);
		result = EXIT_FAILURE;
	}

exit:
	return result;
}

static void
dmg_utility_rom_info_rom_unload(void)
{

	if(g_rom_info.buffer.data) {
		free(g_rom_info.buffer.data);
	}

	memset(&g_rom_info.buffer, 0, sizeof(g_rom_info.buffer));
}

static void
dmg_utility_rom_info_version(
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
dmg_utility_rom_info_usage(
	__in FILE *stream,
	__in bool verbose
	)
{

	if(verbose) {
		dmg_utility_rom_info_version(stream, verbose);
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

	if((result = dmg_utility_rom_info_parse(argc, argv)) != EXIT_SUCCESS) {
		goto exit;
	}

	if(g_rom_info.help) {
		dmg_utility_rom_info_usage(stdout, true);
	} else if(g_rom_info.version) {
		dmg_utility_rom_info_version(stdout, false);
	} else {

		if((result = dmg_utility_rom_info_rom_load()) != EXIT_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to load rom file -- %s\n", argv[0], g_rom_info.rom);
			goto exit;
		}

		if((result = dmg_utility_rom_info_rom_parse()) != EXIT_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Unsupported rom file -- %s\n", argv[0], g_rom_info.rom);
			goto exit;
		}
	}

exit:
	dmg_utility_rom_info_rom_unload();
	memset(&g_rom_info, 0, sizeof(g_rom_info));

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
