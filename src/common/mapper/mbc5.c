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

#include "./mbc5_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
dmg_mapper_mbc5_write_rom(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	)
{

	switch(address) {
		case ADDRESS_MBC5_RAM_ENABLE_BEGIN ... ADDRESS_MBC5_RAM_ENABLE_END:
			dmg_cartridge_ram_enable(&mapper->cartridge, (value & NIBBLE_MAX) == RAM_ENABLE);
			break;
		case ADDRESS_MBC5_RAM_BEGIN ... ADDRESS_MBC5_RAM_END:
			mapper->ram = ((value & MBC5_RAM_MASK) % mapper->cartridge.ram.count);
			break;
		case ADDRESS_MBC5_ROM_LOWER_BEGIN ... ADDRESS_MBC5_ROM_LOWER_END:
			mapper->map.mbc5.lower = value;
			break;
		case ADDRESS_MBC5_ROM_UPPER_BEGIN ... ADDRESS_MBC5_ROM_UPPER_END:
			mapper->map.mbc5.upper = value;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported MBC5 write [%u/%u][%04x]<-%02x", mapper->rom, mapper->rom_swap,
				address, value);
			break;
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "MBC5 banks={%02x, %02x}, %02x", mapper->map.mbc5.lower, mapper->map.mbc5.upper, mapper->map.mbc5.raw & MBC5_ROM_MASK);
	mapper->rom_swap = ((mapper->map.mbc5.raw & MBC5_ROM_MASK) % mapper->cartridge.rom.count);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
