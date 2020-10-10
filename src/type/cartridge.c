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

#include "./cartridge_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_cartridge_validate(
	__in const dmg_buffer_t *buffer,
	__out const dmg_header_t **header,
	__out uint32_t *rom,
	__out uint32_t *ram
	)
{
	uint16_t checksum;
	uint32_t address, length;
	int result = ERROR_SUCCESS;

	if(!buffer) {
		result = ERROR_SET(ERROR_INVALID, "Cartridge is NULL");
		goto exit;
	}

	if(!buffer->data) {
		result = ERROR_SET(ERROR_INVALID, "Cartridge data is NULL");
		goto exit;
	}

	if(buffer->length < ROM_WIDTH) {
		result = ERROR_SET_FORMAT(ERROR_INVALID, "Cartridge length mismatch: %u (expecting >= %u)",
				buffer->length, ROM_WIDTH);
		goto exit;
	}

	*header = (const dmg_header_t *)&(((uint8_t *)buffer->data)[HEADER_BEGIN]);

	TRACE_FORMAT(LEVEL_VERBOSE, "Cartridge title: %s", (*header)->title);

	checksum = 0;
	for(address = HEADER_CHECKSUM_BEGIN; address <= HEADER_CHECKSUM_END; ++address) {
		checksum = (checksum - ((uint8_t *)buffer->data)[address] - 1);
	}

	if((checksum &= UINT8_MAX) != (*header)->checksum) {
		result = ERROR_SET_FORMAT(ERROR_INVALID, "Cartridge checksum mismatch: %02x (expecting %02x)",
				checksum, (*header)->checksum);
		goto exit;
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "Cartridge checksum: %02x", checksum);

	if((*header)->rom >= ROM_MAX) {
		result = ERROR_SET_FORMAT(ERROR_INVALID, "Cartridge rom type unsupported: %u (expecting < %u)",
				(*header)->rom, ROM_MAX);
		goto exit;
	}

	*rom = ROM_BANK[(*header)->rom];

	TRACE_FORMAT(LEVEL_VERBOSE, "Cartridge rom banks: %u", *rom);

	if((length = (ROM_WIDTH * *rom)) != buffer->length) {
		result = ERROR_SET_FORMAT(ERROR_INVALID, "Cartridge length mismatch: %u (expecting %u)",
				buffer->length, length);
		goto exit;
	}

	if((*header)->ram >= RAM_MAX) {
		result = ERROR_SET_FORMAT(ERROR_INVALID, "Cartridge ram type unsupported: %u (expecting < %u)",
				(*header)->ram, RAM_MAX);
		goto exit;
	}

	*ram = RAM_BANK[(*header)->ram];

	TRACE_FORMAT(LEVEL_VERBOSE, "Cartridge ram banks: %u", *ram);

exit:
	return result;
}

int
dmg_cartridge_load(
	__inout dmg_cartridge_t *cartridge,
	__in const dmg_buffer_t *buffer
	)
{
	int result;
	uint32_t index, ram = RAM_MAX, rom = ROM_MAX;

	TRACE(LEVEL_INFORMATION, "Cartridge loading");

	if((result = dmg_cartridge_validate(buffer, &cartridge->header, &rom, &ram)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_bank_allocate(&cartridge->rom, rom)) != ERROR_SUCCESS) {
		goto exit;
	}

	for(index = 0; index < cartridge->rom.count; ++index) {
		cartridge->rom.buffer[index].data = (((uint8_t *)buffer->data) + (ROM_WIDTH * index));
		cartridge->rom.buffer[index].length = ROM_WIDTH;

		TRACE_FORMAT(LEVEL_VERBOSE, "Cartridge rom[%u][%u]=%p", index, cartridge->rom.buffer[index].length,
			cartridge->rom.buffer[index].data);
	}

	if((result = dmg_bank_allocate(&cartridge->ram, ram)) != ERROR_SUCCESS) {
		goto exit;
	}

	for(index = 0; index < cartridge->ram.count; ++index) {

		if((result = dmg_buffer_allocate(&cartridge->ram.buffer[index], RAM_WIDTH, UINT8_MAX)) != ERROR_SUCCESS) {
			goto exit;
		}

		TRACE_FORMAT(LEVEL_VERBOSE, "Cartridge ram[%u][%u]=%p", index, cartridge->ram.buffer[index].length,
			cartridge->ram.buffer[index].data);
	}

	dmg_cartridge_ram_enable(cartridge, true);

	TRACE(LEVEL_INFORMATION, "Cartridge loaded");

exit:
	return result;
}

void
dmg_cartridge_ram_enable(
	__inout dmg_cartridge_t *cartridge,
	__in bool enable
	)
{
	cartridge->enable = enable;

	TRACE_FORMAT(LEVEL_VERBOSE, "Cartridge ram-enable: %x", cartridge->enable);
}

uint8_t
dmg_cartridge_read_ram(
	__in const dmg_cartridge_t *cartridge,
	__in uint32_t bank,
	__in uint16_t address
	)
{
	uint8_t result = 0;

	if(bank >= cartridge->ram.count) {
		result = UINT8_MAX;

		TRACE_FORMAT(LEVEL_WARNING, "Unsupported cartridge ram bank [%u][%04x]->%02x", bank, address, result);
		goto exit;
	}

	if(cartridge->enable) {

		switch(address) {
			case 0 ... (RAM_WIDTH - 1):
				result = ((uint8_t *)cartridge->ram.buffer[bank].data)[address];
				break;
			default:
				result = UINT8_MAX;

				TRACE_FORMAT(LEVEL_WARNING, "Unsupported cartridge ram read [%u][%04x]->%02x", bank, address, result);
				break;
		}
	} else {
		result = UINT8_MAX;

		TRACE_FORMAT(LEVEL_WARNING, "Cartridge ram disabled [%u][%04x]->%02x", bank, address, result);
	}

exit:
	return result;
}

uint8_t
dmg_cartridge_read_rom(
	__in const dmg_cartridge_t *cartridge,
	__in uint32_t bank,
	__in uint16_t address
	)
{
	uint8_t result = 0;

	if(bank >= cartridge->rom.count) {
		result = UINT8_MAX;

		TRACE_FORMAT(LEVEL_WARNING, "Unsupported cartridge rom bank [%u][%04x]->%02x", bank, address, result);
		goto exit;
	}

	switch(address) {
		case 0 ... (ROM_WIDTH - 1):
			result = ((uint8_t *)cartridge->rom.buffer[bank].data)[address];
			break;
		default:
			result = UINT8_MAX;

			TRACE_FORMAT(LEVEL_WARNING, "Unsupported cartridge rom read [%u][%04x]->%02x", bank, address, result);
			break;
	}

exit:
	return result;
}

void
dmg_cartridge_unload(
	__inout dmg_cartridge_t *cartridge
	)
{
	TRACE(LEVEL_INFORMATION, "Cartridge unloading");

	for(uint32_t index = 0; index < cartridge->ram.count; ++index) {
		dmg_buffer_free(&cartridge->ram.buffer[index]);
	}

	dmg_bank_free(&cartridge->ram);
	dmg_bank_free(&cartridge->rom);

	memset(cartridge, 0, sizeof(*cartridge));

	TRACE(LEVEL_INFORMATION, "Cartridge unloaded");
}

void
dmg_cartridge_write_ram(
	__inout dmg_cartridge_t *cartridge,
	__in uint32_t bank,
	__in uint16_t address,
	__in uint8_t value
	)
{

	if(bank < cartridge->ram.count) {

		if(cartridge->enable) {

			switch(address) {
				case 0 ... (RAM_WIDTH - 1):
					((uint8_t *)cartridge->ram.buffer[bank].data)[address] = value;
					break;
				default:
					TRACE_FORMAT(LEVEL_WARNING, "Unsupported cartridge ram write [%u][%04x]<-%02x", bank, address, value);
					break;
			}
		} else {
			TRACE_FORMAT(LEVEL_WARNING, "Cartridge ram disabled [%u][%04x]<-%02x", bank, address, value);
		}
	} else {
		TRACE_FORMAT(LEVEL_WARNING, "Unsupported cartridge ram bank [%u][%04x]<-%02x", bank, address, value);
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
