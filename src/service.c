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

#include "./service_type.h"

static dmg_service_t g_service = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NDEBUG

static void
dmg_service_save_trace(
	__in int level,
	__inout const dmg_save_header_t *header
	)
{
	time_t  current = header->timestamp;
	char timestamp[TIMESTAMP_LENGTH_MAX] = {};
	uint32_t length = (header->length + sizeof(dmg_save_header_t) + sizeof(uint16_t));

	if(!strftime(timestamp, TIMESTAMP_LENGTH_MAX, TIMESTAMP_FORMAT, localtime(&current))) {
		memcpy(timestamp, TIMESTAMP_MALFORMED, strlen(TIMESTAMP_MALFORMED));
	}

	TRACE_FORMAT(level, "Save magic=%08x", header->magic);
	TRACE_FORMAT(level, "Save version=%u", header->version);
	TRACE_FORMAT(level, "Save timestamp=%08x (%s)", header->timestamp, timestamp);
	TRACE_FORMAT(level, "Save length=%.02f KB (%u bytes)", length / (float)KBYTE, length);
}

static void
dmg_service_trace(
	__in int level
	)
{

	for(int index = 0; index < DMG_BUTTON_MAX; ++index) {
		TRACE_FORMAT(LEVEL_VERBOSE, "Service button[%zu]=%u", index, g_service.input.button[index]);
	}

	for(int index = 0; index < DMG_DIRECTION_MAX; ++index) {
		TRACE_FORMAT(LEVEL_VERBOSE, "Service direction[%zu]=%u", index, g_service.input.direction[index]);
	}
}

#endif /* NDEBUG */

bool
dmg_service_button(
	__in int button
	)
{
	bool result = false;

#ifdef SDL
	result = dmg_sdl_button(g_service.input.button[button]);
#endif /* SDL */

	return result;
}

bool
dmg_service_direction(
	__in int direction
	)
{
	bool result = false;

#ifdef SDL
	result = dmg_sdl_direction(g_service.input.direction[direction]);
#endif /* SDL */

	return result;
}

int
dmg_service_export(
	__in dmg_service_handler handler,
	__in const char *path
	)
{
	FILE *file = NULL;
	uint16_t checksum = 0;
	int result = ERROR_SUCCESS;
	uint32_t address = 0, length;
	dmg_save_header_t header = {};

	if(path) {
		TRACE_FORMAT(LEVEL_INFORMATION, "Save file exporting: %s", path);

		if(!(file = fopen(path, "wb+"))) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "Failed to open save file: %s", path);
			goto exit;
		}

		header.magic = SAVE_MAGIC;
		header.version = SAVE_VERSION;
		header.timestamp = time(NULL);

		if(fwrite(&header, sizeof(uint8_t), sizeof(header), file) != sizeof(header)) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "Failed to write to save file: %s", path);
			goto exit;
		}

		if((result = handler(file)) != ERROR_SUCCESS) {
			goto exit;
		}

		length = (ftell(file) - sizeof(dmg_save_header_t));
		fseek(file, offsetof(dmg_save_header_t, length), SEEK_SET);

		if(fwrite(&length, sizeof(length), sizeof(uint8_t), file) != sizeof(uint8_t)) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "Failed to write to save file: %s", path);
			goto exit;
		}

		fseek(file, 0, SEEK_SET);

		for(; address < (length + sizeof(dmg_save_header_t)); ++address) {
			uint8_t value;

			if(fread(&value, sizeof(value), sizeof(uint8_t), file) != sizeof(uint8_t)) {
				result = ERROR_SET_FORMAT(ERROR_FAILURE, "Failed to read from save file: %s", path);
				goto exit;
			}

			checksum += value;
		}

		fseek(file, 0, SEEK_END);

		if(fwrite(&checksum, sizeof(checksum), sizeof(uint8_t), file) != sizeof(uint8_t)) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "Failed to write to save file: %s", path);
			goto exit;
		}

		header.length = length;
		TRACE_SERVICE_SAVE(LEVEL_VERBOSE, &header);
		TRACE(LEVEL_INFORMATION, "Save file exported");
	}

exit:

	if(file) {
		fclose(file);
		file = NULL;
	}

	return result;
}

int
dmg_service_export_data(
	__in FILE *file,
	__in const void *data,
	__in uint32_t length
	)
{
	int result = ERROR_SUCCESS;

	if(fwrite(data, sizeof(uint8_t), length, file) != length) {
		result = ERROR_SET(ERROR_FAILURE, "Failed to write to file");
		goto exit;
	}

exit:
	return result;
}

int
dmg_service_import(
	__in dmg_service_handler handler,
	__in const char *path
	)
{
	FILE *file = NULL;
	uint32_t address = 0;
	uint16_t checksum = 0, expected = 0;
	dmg_buffer_t buffer = {};
	const dmg_save_header_t *header;
	int length, result = EXIT_SUCCESS;

	if(path) {
		TRACE_FORMAT(LEVEL_INFORMATION, "Save file importing: %s", path);

		if(!(file = fopen(path, "rb"))) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "Failed to open save file: %s", path);
			goto exit;
		}

		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);

		if(length < (sizeof(dmg_save_header_t) + sizeof(uint16_t))) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "Malformed save file: %s", path);
			goto exit;
		}

		if((result = dmg_buffer_allocate(&buffer, length, 0)) != ERROR_SUCCESS) {
			goto exit;
		}

		if(fread(buffer.data, sizeof(uint8_t), buffer.length, file) != length) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "Failed to read from save file: %s", path);
			goto exit;
		}

		for(; address < (buffer.length - sizeof(uint16_t)); ++address) {
			checksum += ((uint8_t *)buffer.data)[address];
		}

		expected = ((uint8_t *)buffer.data)[address] | (((uint8_t *)buffer.data)[address + 1] << CHAR_BIT);
		header = (const dmg_save_header_t *)buffer.data;
		TRACE_SERVICE_SAVE(LEVEL_VERBOSE, header);

		if(header->magic != SAVE_MAGIC) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "Save file magic number mismatch: %s (%u != %u)", path,
					header->magic, SAVE_MAGIC);
			goto exit;
		}

		if(header->version != SAVE_VERSION) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "Save file version mismatch: %s (%u != %u)", path,
					header->version, SAVE_VERSION);
			goto exit;
		}

		length = (buffer.length - (sizeof(dmg_save_header_t) + sizeof(uint16_t)));
		if(header->length != length) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "Save file length mismatch: %s (%u != %u)", path,
					header->length, length);
			goto exit;
		}

		if(expected != checksum) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "Save file checksum mismatch: %s (%04x != %04x)", path,
					expected, checksum);
			goto exit;
		}

		fseek(file, sizeof(dmg_save_header_t), SEEK_SET);

		if((result = handler(file)) != ERROR_SUCCESS) {
			goto exit;
		}

		TRACE(LEVEL_INFORMATION, "Save file imported");
	}

exit:
	dmg_buffer_free(&buffer);

	if(file) {
		fclose(file);
		file = NULL;
	}

	return result;
}

int
dmg_service_import_data(
	__in FILE *file,
	__in void *data,
	__in uint32_t length
	)
{
	int result = ERROR_SUCCESS;

	if(fread(data, sizeof(uint8_t), length, file) != length) {
		result = ERROR_SET(ERROR_FAILURE, "Failed to read from file");
		goto exit;
	}

exit:
	return result;
}

int
dmg_service_load(
	__in const dmg_t *configuration,
	__in const char *title
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Service loading");

#ifdef SDL

	if((result = dmg_sdl_load(configuration, title)) != ERROR_SUCCESS) {
		return result;
	}
#endif /* SDL */

	for(int index = 0; index < DMG_BUTTON_MAX; ++index) {
		g_service.input.button[index] = configuration->button[index];
	}

	for(int index = 0; index < DMG_DIRECTION_MAX; ++index) {
		g_service.input.direction[index] = configuration->direction[index];
	}

	TRACE_SERVICE(LEVEL_VERBOSE);
	TRACE(LEVEL_INFORMATION, "Service loaded");

	return result;
}

void
dmg_service_pixel(
	__in uint8_t color,
	__in uint8_t x,
	__in uint8_t y
	)
{
#ifdef SDL
	dmg_sdl_pixel(color, x, y);
#endif /* SDL */
}

bool
dmg_service_poll(void)
{
#ifdef SDL
	return dmg_sdl_poll();
#endif /* SDL */
}

void
dmg_service_sync(void)
{
#ifdef SDL
	dmg_sdl_sync();
#endif /* SDL */
}

void
dmg_service_unload(void)
{
	TRACE(LEVEL_INFORMATION, "Service unloading");

#ifdef SDL
	dmg_sdl_unload();
#endif /* SDL */
	memset(&g_service, 0, sizeof(g_service));

	TRACE(LEVEL_INFORMATION, "Service unloaded");
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
