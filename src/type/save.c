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

#ifndef NDEBUG

static void
dmg_save_trace(
	__in int level,
	__inout const dmg_save_header_t *header
	)
{
	time_t  current = header->timestamp;
	char timestamp[TIMESTAMP_LENGTH_MAX] = {};

	if(!strftime(timestamp, TIMESTAMP_LENGTH_MAX, TIMESTAMP_FORMAT, localtime(&current))) {
		memcpy(timestamp, TIMESTAMP_MALFORMED, strlen(TIMESTAMP_MALFORMED));
	}

	TRACE_FORMAT(level, "Save magic=%08x", header->magic);
	TRACE_FORMAT(level, "Save version=%u", header->version);
	TRACE_FORMAT(level, "Save timestamp=%08x (%s)", header->timestamp, timestamp);
	TRACE_FORMAT(level, "Save length=%.01f KB (%u bytes)", header->length);
}

#endif /* NDEBUG */

int
dmg_save_export(
	__in const void *context,
	__in FILE *file
	)
{
	uint32_t length;
	int result = ERROR_SUCCESS;
	const dmg_runtime_t *runtime;
	dmg_save_header_t header = {};
	uint16_t address = 0, checksum = 0;

	header.magic = SAVE_MAGIC;
	header.version = SAVE_VERSION;
	header.timestamp = time(NULL);

	if(fwrite(&header, sizeof(uint8_t), sizeof(header), file) != sizeof(header)) {
		result = ERROR_FAILURE;
		goto exit;
	}

	// TODO: READ FROM RUNTIME
	runtime = (const dmg_runtime_t *)context;
	(void)runtime;
	// ---

	length = (ftell(file) - sizeof(dmg_save_header_t));
	fseek(file, offsetof(dmg_save_header_t, length), SEEK_SET);

	if(fwrite(&length, sizeof(length), 1, file) != 1) {
		result = ERROR_FAILURE;
		goto exit;
	}

	fseek(file, 0, SEEK_SET);

	for(; address < (length + sizeof(dmg_save_header_t)); ++address) {
		uint8_t value;

		if(fread(&value, sizeof(value), 1, file) != value) {
			result = ERROR_FAILURE;
			goto exit;
		}

		checksum += value;
	}

	fseek(file, 0, SEEK_END);

	if(fwrite(&checksum, sizeof(checksum), 1, file) != 1) {
		result = ERROR_FAILURE;
		goto exit;
	}

	header.length = length;
	TRACE_SAVE(LEVEL_VERBOSE, &header);

exit:
	return result;
}

int
dmg_save_import(
	__in void *context,
	__in FILE *file
	)
{
	uint32_t address = 0;
	uint16_t checksum = 0;
	dmg_runtime_t *runtime;
	dmg_buffer_t buffer = {};
	const dmg_save_header_t *header;
	int length, result = EXIT_SUCCESS;

	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	if(length < (sizeof(dmg_save_header_t) + sizeof(uint16_t))) {
		result = ERROR_FAILURE;
		goto exit;
	}

	if((result = dmg_buffer_allocate(&buffer, length, 0)) != ERROR_SUCCESS) {
		result = ERROR_FAILURE;
		goto exit;
	}

	if(fread(buffer.data, sizeof(uint8_t), buffer.length, file) != length) {
		result = ERROR_FAILURE;
		goto exit;
	}

	for(; address < (buffer.length - sizeof(uint16_t)); ++address) {
		checksum += ((uint8_t *)buffer.data)[address];
	}

	header = (const dmg_save_header_t *)buffer.data;
	TRACE_SAVE(LEVEL_VERBOSE, header);

	if((header->magic != SAVE_MAGIC)
			|| (header->version != SAVE_VERSION)
			|| (header->length != buffer.length - (sizeof(dmg_save_header_t) + sizeof(uint16_t)))
			|| (((uint16_t *)buffer.data)[address / sizeof(uint16_t)] != checksum)) {
		result = ERROR_FAILURE;
		goto exit;
	}

	// TODO: WRITE TO RUNTIME
	runtime = (dmg_runtime_t *)context;
	(void)runtime;
	// ---

exit:
	dmg_buffer_free(&buffer);

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
