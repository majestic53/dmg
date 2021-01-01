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

#include "./rom_fix_type.h"

static dmg_rom_fix_t g_rom_fix = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_utility_rom_fix_parse(
	__in int argc,
	__in char *argv[]
	)
{
	int option, result = EXIT_SUCCESS;

	opterr = 1;

	while((option = getopt(argc, argv, OPTIONS)) != -1) {

		switch(option) {
			case OPTION_HELP:
				g_rom_fix.help = true;
				break;
			case OPTION_ROM:
				g_rom_fix.rom = optarg;
				break;
			case OPTION_VERSION:
				g_rom_fix.version = true;
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
dmg_utility_rom_fix_rom_load(void)
{
	int length, result = EXIT_SUCCESS;

	if(!(g_rom_fix.file = fopen(g_rom_fix.rom, "rb"))) {
		result = EXIT_FAILURE;
		goto exit;
	}

	fseek(g_rom_fix.file, 0, SEEK_END);
	length = ftell(g_rom_fix.file);
	fseek(g_rom_fix.file, 0, SEEK_SET);

	if(length <= 0) {
		result = EXIT_FAILURE;
		goto exit;
	}

	if(!(g_rom_fix.buffer.data = (void *)malloc(length))) {
		result = EXIT_FAILURE;
		goto exit;
	}

	if(fread(g_rom_fix.buffer.data, sizeof(uint8_t), length, g_rom_fix.file) != length) {
		result = EXIT_FAILURE;
		goto exit;
	}

	g_rom_fix.buffer.length = length;

exit:

	if(g_rom_fix.file) {
		fclose(g_rom_fix.file);
		g_rom_fix.file = NULL;
	}

	return result;
}

static int
dmg_utility_rom_fix_rom_parse(void)
{
	uint16_t expected = 0;
	int result = EXIT_SUCCESS;
	uint8_t *found = &((uint8_t *)g_rom_fix.buffer.data)[ADDRESS_HEADER_CHECKSUM];

	for(uint16_t address = ADDRESS_HEADER_CHECKSUM_BEGIN; address <= ADDRESS_HEADER_CHECKSUM_END; ++address) {
		expected = (expected - ((uint8_t *)g_rom_fix.buffer.data)[address] - 1);
	}

	if((expected &= UINT8_MAX) != *found) {
		TRACE_TOOL_MESSAGE("Fixed checksum (%02x -> %02x)\n", *found, expected);
		*found = expected;

		if(!(g_rom_fix.file = fopen(g_rom_fix.rom, "wb"))) {
			result = EXIT_FAILURE;
			goto exit;
		}

		if(fwrite(g_rom_fix.buffer.data, sizeof(uint8_t), g_rom_fix.buffer.length, g_rom_fix.file) != g_rom_fix.buffer.length) {
			result = EXIT_FAILURE;
			goto exit;
		}
	}

exit:

	if(g_rom_fix.file) {
		fclose(g_rom_fix.file);
		g_rom_fix.file = NULL;
	}

	return result;
}

static void
dmg_utility_rom_fix_rom_unload(void)
{

	if(g_rom_fix.file) {
		fclose(g_rom_fix.file);
		g_rom_fix.file = NULL;
	}

	if(g_rom_fix.buffer.data) {
		free(g_rom_fix.buffer.data);
	}

	memset(&g_rom_fix.buffer, 0, sizeof(g_rom_fix.buffer));
}

int
main(
	__in int argc,
	__in char *argv[]
	)
{
	int result = EXIT_SUCCESS;

	if((result = dmg_utility_rom_fix_parse(argc, argv)) != EXIT_SUCCESS) {
		goto exit;
	}

	if(g_rom_fix.help) {
		dmg_tool_usage(stdout, true, DMG_USAGE, FLAG_STR, FLAG_DESCRIPTION_STR, FLAG_MAX);
	} else if(g_rom_fix.version) {
		dmg_tool_version(stdout, false);
	} else {

		if((result = dmg_utility_rom_fix_rom_load()) != EXIT_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to load rom file -- %s\n", argv[0], g_rom_fix.rom);
			goto exit;
		}

		if((result = dmg_utility_rom_fix_rom_parse()) != EXIT_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Unsupported rom file -- %s\n", argv[0], g_rom_fix.rom);
			goto exit;
		}
	}

exit:
	dmg_utility_rom_fix_rom_unload();
	memset(&g_rom_fix, 0, sizeof(g_rom_fix));

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
