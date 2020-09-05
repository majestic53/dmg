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
	__out uint32_t *rom_banks,
	__out uint32_t *ram_banks
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

	*header = (const dmg_header_t *)&buffer->data[HEADER_BEGIN];

	checksum = 0;
	for(address = HEADER_CHECKSUM_BEGIN; address <= HEADER_CHECKSUM_END; ++address) {
		checksum = (checksum - buffer->data[address] - 1);
	}

	checksum &= UINT8_MAX;
	if(checksum != (*header)->checksum) {
		result = ERROR_SET_FORMAT(ERROR_INVALID, "Cartridge checksum mismatch: %02x (expecting %02x)",
				checksum, (*header)->checksum);
		goto exit;
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "Cartridge checksum=%02x", checksum);

	if((*header)->rom >= ROM_MAX) {
		result = ERROR_SET_FORMAT(ERROR_INVALID, "Cartridge rom type unsupported: %u (expecting < %u)",
				(*header)->rom, ROM_MAX);
		goto exit;
	}

	*rom_banks = ROM_BANK[(*header)->rom];

	TRACE_FORMAT(LEVEL_VERBOSE, "Cartridge rom banks=%u", *rom_banks);

	length = (ROM_WIDTH * *rom_banks);
	if(buffer->length != length) {
		result = ERROR_SET_FORMAT(ERROR_INVALID, "Cartridge length mismatch: %u (expecting %u)",
				buffer->length, length);
		goto exit;
	}

	if((*header)->ram >= RAM_MAX) {
		result = ERROR_SET_FORMAT(ERROR_INVALID, "Cartridge ram type unsupported: %u (expecting < %u)",
				(*header)->ram, RAM_MAX);
		goto exit;
	}

	*ram_banks = RAM_BANK[(*header)->ram];

	TRACE_FORMAT(LEVEL_VERBOSE, "Cartridge ram banks=%u", *ram_banks);

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
	uint32_t index;

	TRACE(LEVEL_INFORMATION, "Cartridge loading");

	if((result = dmg_cartridge_validate(buffer, &cartridge->header, &cartridge->rom_banks, &cartridge->ram_banks))
			!= ERROR_SUCCESS) {
		goto exit;
	}

	cartridge->rom = (dmg_buffer_t *)malloc(cartridge->rom_banks * sizeof(dmg_buffer_t));
	if(!cartridge->rom) {
		result = ERROR_SET(ERROR_FAILURE, "Failed to allocate rom banks");
		goto exit;
	}

	for(index = 0; index < cartridge->rom_banks; ++index) {
		cartridge->rom[index].data = (buffer->data + (ROM_WIDTH * index));
		cartridge->rom[index].length = ROM_WIDTH;

		TRACE_FORMAT(LEVEL_VERBOSE, "Cartridge rom[%u][%u]=%p", index, cartridge->rom[index].length,
			cartridge->rom[index].data);
	}

	cartridge->ram = (dmg_buffer_t *)malloc(cartridge->ram_banks * sizeof(dmg_buffer_t));
	if(!cartridge->ram) {
		result = ERROR_SET(ERROR_FAILURE, "Failed to allocate ram banks");
		goto exit;
	}

	for(index = 0; index < cartridge->ram_banks; ++index) {

		if((result = dmg_buffer_load(&cartridge->ram[index], RAM_WIDTH, UINT8_MAX)) != ERROR_SUCCESS) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "Failed to allocate ram bank %u", index);
			goto exit;
		}

		TRACE_FORMAT(LEVEL_VERBOSE, "Cartridge ram[%u][%u]=%p", index, cartridge->ram[index].length,
			cartridge->ram[index].data);
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "Cartridge title=%s", cartridge->header->title);
	TRACE(LEVEL_INFORMATION, "Cartridge loaded");

exit:
	return result;
}

uint8_t
dmg_cartridge_read_ram(
	__in const dmg_cartridge_t *cartridge,
	__in uint32_t bank,
	__in uint16_t address
	)
{
	uint8_t result = 0;

	if(bank >= cartridge->ram_banks) {
		result = UINT8_MAX;

		TRACE_FORMAT(LEVEL_WARNING, "Unsupported cartridge ram bank [%u][%04x]->%02x", bank, address, result);
		goto exit;
	}

	switch(address) {
		case 0 ... (RAM_WIDTH - 1):
			result = cartridge->ram[bank].data[address];
			break;
		default:
			result = UINT8_MAX;

			TRACE_FORMAT(LEVEL_WARNING, "Unsupported cartridge ram read [%u][%04x]->%02x", bank, address, result);
			break;
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

	if(bank >= cartridge->rom_banks) {
		result = UINT8_MAX;

		TRACE_FORMAT(LEVEL_WARNING, "Unsupported cartridge rom bank [%u][%04x]->%02x", bank, address, result);
		goto exit;
	}

	switch(address) {
		case 0 ... (ROM_WIDTH - 1):
			result = cartridge->rom[bank].data[address];
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

	if(cartridge->ram) {

		for(uint32_t index = 0; index < cartridge->ram_banks; ++index) {
			dmg_buffer_unload(&cartridge->ram[index]);
		}

		free(cartridge->ram);
	}

	if(cartridge->rom) {
		free(cartridge->rom);
	}

	memset(cartridge, 0, sizeof(*cartridge));

	TRACE(LEVEL_INFORMATION, "Cartridge unloaded");
}

void
dmg_cartridge_write_ram(
	__in dmg_cartridge_t *cartridge,
	__in uint32_t bank,
	__in uint16_t address,
	__in uint8_t value
	)
{

	if(bank < cartridge->ram_banks) {

		switch(address) {
			case 0 ... (RAM_WIDTH - 1):
				cartridge->ram[bank].data[address] = value;
				break;
			default:
				TRACE_FORMAT(LEVEL_WARNING, "Unsupported cartridge ram write [%u][%04x]<-%02x", bank, address, value);
				break;
		}
	} else {
		TRACE_FORMAT(LEVEL_WARNING, "Unsupported cartridge ram bank [%u][%04x]<-%02x", bank, address, value);
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
