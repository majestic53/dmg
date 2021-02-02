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

#include "./save_info_type.h"

static dmg_save_info_t g_save_info = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_utility_save_info_parse(
	__in int argc,
	__in char *argv[]
	)
{
	int option, result = DMG_STATUS_SUCCESS;

	opterr = 1;

	while((option = getopt(argc, argv, OPTIONS)) != -1) {

		switch(option) {
			case OPTION_HELP:
				g_save_info.help = true;
				break;
			case OPTION_SAVE:
				g_save_info.save = optarg;
				break;
			case OPTION_VERSION:
				g_save_info.version = true;
				break;
			case '?':
				result = DMG_STATUS_FAILURE;
				goto exit;
			default:
				result = DMG_STATUS_FAILURE;
				goto exit;
		}
	}

	if(!g_save_info.save) {
		TRACE_TOOL_ERROR("%s: Missing save path -- %s\n", argv[0], g_save_info.save);
		result = DMG_STATUS_INVALID;
		goto exit;
	}

exit:
	return result;
}

static int
dmg_utility_save_info_save_load(void)
{
	FILE *file = NULL;
	int length = 0, result = DMG_STATUS_SUCCESS;

	if((result = dmg_tool_file_open(g_save_info.save, true, false, &file, &length)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(!(g_save_info.buffer.data = (void *)calloc(length, sizeof(uint8_t)))) {
		result = DMG_STATUS_FAILURE;
		goto exit;
	}

	if(fread(g_save_info.buffer.data, sizeof(uint8_t), length, file) != length) {
		result = DMG_STATUS_FAILURE;
		goto exit;
	}

	g_save_info.buffer.length = length;

exit:
	dmg_tool_file_close(&file);

	return result;
}

static int
dmg_utility_save_info_save_parse(void)
{
	time_t current;
	uint16_t checksum = 0;
	uint32_t address, expected;
	int result = DMG_STATUS_SUCCESS;
	const dmg_save_header_t *header;
	char timestamp[TIMESTAMP_LENGTH_MAX] = {};

	TRACE_TOOL_MESSAGE("%s -- %.02f KB (%u bytes)\n\n", g_save_info.save, g_save_info.buffer.length / (float)KBYTE, g_save_info.buffer.length);

	if(g_save_info.buffer.length <= (expected = (sizeof(*header) + sizeof(checksum)))) {
		TRACE_TOOL_ERROR("File is too small -- %.02f KB (%u bytes) (expecting > %.02f KB (%u bytes))\n", g_save_info.buffer.length / (float)KBYTE,
			g_save_info.buffer.length, expected / (float)KBYTE, expected);
		result = DMG_STATUS_FAILURE;
		goto exit;
	}

	header = (const dmg_save_header_t *)g_save_info.buffer.data;

	if(header->magic != (expected = SAVE_MAGIC)) {
		TRACE_TOOL_MESSAGE("Magic     MISMATCH (Expecting \"%s\")\n", (char *)&expected);
		result = DMG_STATUS_FAILURE;
	}

	if(header->version != (expected = SAVE_VERSION)) {
		TRACE_TOOL_MESSAGE("Version   MISMATCH (Expecting %u)\n", expected);
		result = DMG_STATUS_FAILURE;
	}

	current = header->timestamp;
	if(!strftime(timestamp, TIMESTAMP_LENGTH_MAX, TIMESTAMP_FORMAT, localtime(&current))) {
		memcpy(timestamp, TIMESTAMP_MALFORMED, strlen(TIMESTAMP_MALFORMED));
	}

	TRACE_TOOL_MESSAGE("Timestamp %s\n", timestamp);
	TRACE_TOOL_MESSAGE("Length    %.02f KB (%u bytes)", header->length / (float)KBYTE, header->length);

	if(header->length != (expected = (g_save_info.buffer.length - (sizeof(*header) + sizeof(checksum))))) {
		TRACE_TOOL_MESSAGE(", MISMATCH (Expecting %.02f KB (%u bytes))\n", expected / (float)KBYTE, expected);
	} else {
		TRACE_TOOL_MESSAGE("%s", "\n");

		for(address = 0; address < (header->length + sizeof(*header)); ++address) {
			checksum += ((uint8_t *)g_save_info.buffer.data)[address];
		}

		if(checksum != (uint16_t)(((uint8_t *)g_save_info.buffer.data)[address] | (((uint8_t *)g_save_info.buffer.data)[address + 1] << CHAR_BIT))) {
			TRACE_TOOL_MESSAGE("Checksum  MISMATCH (Expecting %04x)\n", (uint16_t)checksum);
			result = DMG_STATUS_FAILURE;
		}
	}

exit:
	return result;
}

static void
dmg_utility_save_info_save_unload(void)
{

	if(g_save_info.buffer.data) {
		free(g_save_info.buffer.data);
	}

	memset(&g_save_info.buffer, 0, sizeof(g_save_info.buffer));
}

int
main(
	__in int argc,
	__in char *argv[]
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((result = dmg_utility_save_info_parse(argc, argv)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(g_save_info.help) {
		dmg_tool_usage(stdout, true, DMG_USAGE, FLAG_STR, FLAG_DESCRIPTION_STR, FLAG_MAX);
	} else if(g_save_info.version) {
		dmg_tool_version(stdout, false);
	} else {

		if((result = dmg_utility_save_info_save_load()) != DMG_STATUS_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to load save file -- %s\n", argv[0], g_save_info.save);
			goto exit;
		}

		if((result = dmg_utility_save_info_save_parse()) != DMG_STATUS_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Unsupported save file -- %s\n", argv[0], g_save_info.save);
			goto exit;
		}
	}

exit:
	dmg_utility_save_info_save_unload();
	memset(&g_save_info, 0, sizeof(g_save_info));

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
