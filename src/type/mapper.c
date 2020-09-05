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

#include "./mapper_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_mapper_load(
	__inout dmg_mapper_t *mapper,
	__in const dmg_buffer_t *buffer
	)
{
	int result, type;

	TRACE(LEVEL_INFORMATION, "Mapper loading");

	if((result = dmg_cartridge_load(&mapper->cartridge, buffer)) != ERROR_SUCCESS) {
		goto exit;
	}

	switch((type = mapper->cartridge.header->mapper)) {
		case MAPPER_ROM_ONLY:
			mapper->ram = 0;
			mapper->ram_enable = true;
			mapper->rom = 0;
			mapper->rom_swap = (mapper->rom + 1);
			break;
		default:
			result = ERROR_SET_FORMAT(ERROR_INVALID, "Unsupported mapper type: %u", type);
			goto exit;
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "Mapper type: %u", mapper->cartridge.header->mapper);
	TRACE_FORMAT(LEVEL_VERBOSE, "Mapper rom bank: %u", mapper->rom);
	TRACE_FORMAT(LEVEL_VERBOSE, "Mapper rom-swap bank: %u", mapper->rom_swap);
	TRACE_FORMAT(LEVEL_VERBOSE, "Mapper ram bank: %u", mapper->ram);
	TRACE_FORMAT(LEVEL_VERBOSE, "Mapper ram-enable: %x", mapper->ram_enable);
	TRACE(LEVEL_INFORMATION, "Mapper loaded");

exit:
	return result;
}

uint8_t
dmg_mapper_read_ram(
	__in const dmg_mapper_t *mapper,
	__in uint16_t address
	)
{
	uint8_t result = 0;

	if(mapper->ram_enable) {

		switch(address) {
			case ADDRESS_RAM_SWAP_BEGIN ... ADDRESS_RAM_SWAP_END:
				result = dmg_cartridge_read_ram(&mapper->cartridge, mapper->ram, address - ADDRESS_RAM_SWAP_BEGIN);
				break;
			default:
				result = UINT8_MAX;

				TRACE_FORMAT(LEVEL_WARNING, "Unsupported mapper ram read [%u][%04x]->%02x", mapper->ram, address, result);
				break;
		}
	} else {
		result = UINT8_MAX;

		TRACE_FORMAT(LEVEL_WARNING, "Mapper ram disabled [%u][%04x]->%02x", mapper->ram, address, result);
	}

	return result;
}

uint8_t
dmg_mapper_read_rom(
	__in const dmg_mapper_t *mapper,
	__in uint16_t address
	)
{
	uint8_t result = 0;

	switch(address) {
		case ADDRESS_ROM_BEGIN ... ADDRESS_ROM_END:
			result = dmg_cartridge_read_rom(&mapper->cartridge, mapper->rom, address - ADDRESS_ROM_BEGIN);
			break;
		case ADDRESS_ROM_SWAP_BEGIN ... ADDRESS_ROM_SWAP_END:
			result = dmg_cartridge_read_rom(&mapper->cartridge, mapper->rom_swap, address - ADDRESS_ROM_SWAP_BEGIN);
			break;
		default:
			result = UINT8_MAX;

			TRACE_FORMAT(LEVEL_WARNING, "Unsupported mapper rom read [%u/%u][%04x]->%02x", mapper->rom, mapper->rom_swap,
				address, result);
			break;
	}

	return result;
}

void
dmg_mapper_unload(
	__inout dmg_mapper_t *mapper
	)
{
	TRACE(LEVEL_INFORMATION, "Mapper unloading");

	dmg_cartridge_unload(&mapper->cartridge);
	memset(mapper, 0, sizeof(*mapper));

	TRACE(LEVEL_INFORMATION, "Mapper unloaded");
}

void
dmg_mapper_write_ram(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	)
{

	if(mapper->ram_enable) {

		switch(address) {
			case ADDRESS_RAM_SWAP_BEGIN ... ADDRESS_RAM_SWAP_END:
				dmg_cartridge_write_ram(&mapper->cartridge, mapper->ram, address - ADDRESS_RAM_SWAP_BEGIN, value);
				break;
			default:
				TRACE_FORMAT(LEVEL_WARNING, "Unsupported mapper ram write [%u][%04x]->%02x", mapper->ram, address, value);
				break;
		}
	} else {
		TRACE_FORMAT(LEVEL_WARNING, "Mapper ram disabled [%u][%04x]<-%02x", mapper->ram, address, value);
	}
}

void
dmg_mapper_write_rom(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	)
{
	int type;

	switch((type = mapper->cartridge.header->mapper)) {
		case MAPPER_ROM_ONLY:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported mapper rom write [%u/%u][%04x]->%02x", mapper->rom, mapper->rom_swap,
				address, value);
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported mapper type: %u", type);
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
