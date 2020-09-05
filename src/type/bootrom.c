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

#include "./bootrom_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_bootrom_validate(
	__in const dmg_buffer_t *buffer
	)
{
	uint16_t checksum = 0;
	int index = 0, result = ERROR_SUCCESS;

	if(!buffer) {
		result = ERROR_SET(ERROR_INVALID, "Bootrom is NULL");
		goto exit;
	}

	if(!buffer->data) {
		result = ERROR_SET(ERROR_INVALID, "Bootrom data is NULL");
		goto exit;
	}

	if(buffer->length != BOOTROM_WIDTH) {
		result = ERROR_SET_FORMAT(ERROR_INVALID, "Bootrom length mismatch: %u (expecting %u)",
				buffer->length, BOOTROM_WIDTH);
		goto exit;
	}

	for(; index < BOOTROM_WIDTH; ++index) {
		checksum += buffer->data[index];
	}

	if(checksum != BOOTROM_CHECKSUM) {
		result = ERROR_SET_FORMAT(ERROR_INVALID, "Bootrom checksum mismatch: %04x (expecting %04x)",
				checksum, BOOTROM_CHECKSUM);
		goto exit;
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "Bootrom checksum=%04x", checksum);

exit:
	return result;
}

int
dmg_bootrom_load(
	__inout dmg_bootrom_t *bootrom,
	__in const dmg_buffer_t *buffer
	)
{
	int result;

	TRACE(LEVEL_INFORMATION, "Bootrom loading");

	if((result = dmg_bootrom_validate(buffer)) != ERROR_SUCCESS) {
		goto exit;
	}

	bootrom->buffer = buffer;

	TRACE_FORMAT(LEVEL_VERBOSE, "Bootrom[%04x]=%p", bootrom->buffer->length, bootrom->buffer->data);
	TRACE(LEVEL_INFORMATION, "Bootrom loaded");

exit:
	return result;
}

uint8_t
dmg_bootrom_read(
	__inout dmg_bootrom_t *bootrom,
	__in uint16_t address
	)
{
	uint8_t result = 0;

	switch(address) {
		case ADDRESS_BOOTROM_BEGIN ... ADDRESS_BOOTROM_END:
			result = bootrom->buffer->data[address];
			break;
		default:
			result = UINT8_MAX;
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported read [%04x]->%02x", address, result);
			break;
	}

	return result;
}

void
dmg_bootrom_unload(
	__inout dmg_bootrom_t *bootrom
	)
{
	TRACE(LEVEL_INFORMATION, "Bootrom unloading");

	memset(bootrom, 0, sizeof(*bootrom));

	TRACE(LEVEL_INFORMATION, "Bootrom unloaded");
}

void
dmg_bootrom_write(
	__inout dmg_bootrom_t *bootrom,
	__in uint16_t address,
	__in uint8_t value
	)
{
	TRACE_FORMAT(LEVEL_WARNING, "Unsupported write [%04x]<-%02x", address, value);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
