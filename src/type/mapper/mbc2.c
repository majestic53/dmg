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

#include "./mbc2_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uint8_t
dmg_mapper_mbc2_read_ram(
	__in const dmg_mapper_t *mapper,
	__in uint16_t address
	)
{
	return dmg_cartridge_read_ram(&mapper->cartridge, mapper->ram, address % MBC2_RAM_WIDTH);
}

void
dmg_mapper_mbc2_write_ram(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	)
{
	dmg_cartridge_write_ram(&mapper->cartridge, mapper->ram, address % MBC2_RAM_WIDTH, value | MBC2_RAM_MASK);
}

void
dmg_mapper_mbc2_write_rom(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	)
{
	dmg_processor_register_t check = {};

	check.word = address;

	switch(address) {
		case ADDRESS_MBC2_RAM_ENABLE_BEGIN ... ADDRESS_MBC2_RAM_ENABLE_END:
		case ADDRESS_MBC2_ROM_BEGIN ... ADDRESS_MBC2_ROM_END:

			if(check.high_lsb) {
				mapper->map.mbc2.raw = value;
				TRACE_FORMAT(LEVEL_VERBOSE, "MBC2 bank=%02x", mapper->map.mbc2.bank);

				if(!(mapper->rom_swap = mapper->map.mbc2.bank)) {
					++mapper->rom_swap;
				}

				mapper->rom_swap %= mapper->cartridge.rom.count;
			} else {
				dmg_cartridge_ram_enable(&mapper->cartridge, (value & NIBBLE_MAX) == RAM_ENABLE);
			}
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported MBC2 write [%u/%u][%04x]<-%02x", mapper->rom, mapper->rom_swap,
				address, value);
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
