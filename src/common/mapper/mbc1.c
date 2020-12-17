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

#include "./mbc1_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
dmg_mapper_mbc1_write_rom(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	)
{

	switch(address) {
		case ADDRESS_MBC1_MODE_BEGIN ... ADDRESS_MBC1_MODE_END:
			mapper->map.mbc1.mode = value;
			break;
		case ADDRESS_MBC1_RAM_ENABLE_BEGIN ... ADDRESS_MBC1_RAM_ENABLE_END:
			dmg_cartridge_ram_enable(&mapper->cartridge, (value & NIBBLE_MAX) == RAM_ENABLE);
			break;
		case ADDRESS_MBC1_ROM_LOWER_BEGIN ... ADDRESS_MBC1_ROM_LOWER_END:
			mapper->map.mbc1.lower = value;
			break;
		case ADDRESS_MBC1_ROM_UPPER_BEGIN ... ADDRESS_MBC1_ROM_UPPER_END:
			mapper->map.mbc1.upper = value;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported MBC1 write [%u/%u][%04x]<-%02x", mapper->rom, mapper->rom_swap,
				address, value);
			break;
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "MBC1 mode=%i", mapper->map.mbc1.mode);
	TRACE_FORMAT(LEVEL_VERBOSE, "MBC1 banks={%02x, %02x}, %02x", mapper->map.mbc1.lower, mapper->map.mbc1.upper, mapper->map.mbc1.raw);

	switch(mapper->map.mbc1.mode) {
		case MBC1_MODE_RAM:
			mapper->ram = mapper->map.mbc1.upper;
			mapper->rom_swap = mapper->map.mbc1.lower;
			break;
		case MBC1_MODE_ROM:
			mapper->ram = 0;
			mapper->rom_swap = mapper->map.mbc1.raw;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported mbc1 mode %u", mapper->map.mbc1.mode);
			break;
	}

	mapper->ram %= mapper->cartridge.ram.count;
	mapper->rom_swap %= mapper->cartridge.rom.count;

	switch(mapper->rom_swap) {
		case MBC1_BANK_00:
		case MBC1_BANK_20:
		case MBC1_BANK_40:
		case MBC1_BANK_60:
			++mapper->rom_swap;
			break;
		default:
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
