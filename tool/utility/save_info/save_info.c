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

#include "./save_info_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static dmg_save_info_t g_save_info = {};

static int
dmg_utility_save_info_file_load(
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
dmg_utility_save_info_file_parse(void)
{
	time_t current;
	uint16_t checksum = 0;
	int result = EXIT_SUCCESS;
	uint32_t address, expected;
	const dmg_save_header_t *header;
	char timestamp[TIMESTAMP_LENGTH_MAX] = {};

	fprintf(stdout, "%s -- %.02f KB (%u bytes)\n\n", g_save_info.save, g_save_info.buffer.length / (float)KBYTE, g_save_info.buffer.length);

	if(g_save_info.buffer.length <= (expected = (sizeof(*header) + sizeof(checksum)))) {
		fprintf(stderr, "File is too small -- %.02f KB (%u bytes) (expecting > %.02f KB (%u bytes))\n", g_save_info.buffer.length / (float)KBYTE,
			g_save_info.buffer.length, expected / (float)KBYTE, expected);
		result = EXIT_FAILURE;
		goto exit;
	}

	header = (const dmg_save_header_t *)g_save_info.buffer.data;

	if(header->magic != (expected = SAVE_MAGIC)) {
		fprintf(stdout, "Magic     MISMATCH (Expecting \"%s\")\n", (char *)&expected);
		result = EXIT_FAILURE;
	}

	if(header->version != (expected = SAVE_VERSION)) {
		fprintf(stdout, "Version   MISMATCH (Expecting %u)\n", expected);
		result = EXIT_FAILURE;
	}

	current = header->timestamp;
	if(!strftime(timestamp, TIMESTAMP_LENGTH_MAX, TIMESTAMP_FORMAT, localtime(&current))) {
		memcpy(timestamp, TIMESTAMP_MALFORMED, strlen(TIMESTAMP_MALFORMED));
	}

	fprintf(stdout, "Timestamp %s\n", timestamp);
	fprintf(stdout, "Length    %.02f KB (%u bytes)", header->length / (float)KBYTE, header->length);

	if(header->length != (expected = (g_save_info.buffer.length - (sizeof(*header) + sizeof(checksum))))) {
		fprintf(stdout, ", MISMATCH (Expecting %.02f KB (%u bytes))\n", expected / (float)KBYTE, expected);
	} else {
		fprintf(stdout, "\n");
	}

	for(address = 0; address < (header->length + sizeof(*header)); ++address) {
		checksum += ((uint8_t *)g_save_info.buffer.data)[address];
	}

	if(checksum != (uint16_t)(((uint8_t *)g_save_info.buffer.data)[address] | (((uint8_t *)g_save_info.buffer.data)[address + 1] << CHAR_BIT))) {
		fprintf(stdout, "Checksum  MISMATCH (Expecting %04x)\n", (uint16_t)checksum);
		result = EXIT_FAILURE;
	}

exit:
	return result;
}

static void
dmg_utility_save_info_file_unload(
	__inout dmg_buffer_t *buffer
	)
{

	if(buffer->data) {
		free(buffer->data);
	}

	memset(buffer, 0, sizeof(*buffer));
}

static int
dmg_utility_save_info_parse(
	__in int argc,
	__in char *argv[]
	)
{
	int option, result = EXIT_SUCCESS;

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
dmg_utility_save_info_version(
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
dmg_utility_save_info_usage(
	__in FILE *stream,
	__in bool verbose
	)
{

	if(verbose) {
		dmg_utility_save_info_version(stream, verbose);
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

	if((result = dmg_utility_save_info_parse(argc, argv)) != EXIT_SUCCESS) {
		goto exit;
	}

	if(g_save_info.help) {
		dmg_utility_save_info_usage(stdout, true);
	} else if(g_save_info.version) {
		dmg_utility_save_info_version(stdout, false);
	} else {

		if((result = dmg_utility_save_info_file_load(&g_save_info.buffer, g_save_info.save)) != EXIT_SUCCESS) {
			fprintf(stderr, "%s: Failed to load file -- %s\n", argv[0], g_save_info.save);
			goto exit;
		}

		result = dmg_utility_save_info_file_parse();
	}

exit:
	dmg_utility_save_info_file_unload(&g_save_info.buffer);
	memset(&g_save_info, 0, sizeof(g_save_info));

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
