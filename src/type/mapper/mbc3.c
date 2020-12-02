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

#include "./mbc3_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uint8_t
dmg_mapper_mbc3_read_ram(
	__in const dmg_mapper_t *mapper,
	__in uint16_t address
	)
{
	uint8_t result = 0;
	int type = mapper->map.mbc3.mode;

	if(mapper->cartridge.enable) {

		switch(type) {
			case MBC3_MODE_RAM_0 ... MBC3_MODE_RAM_7:
				result = dmg_cartridge_read_ram(&mapper->cartridge, mapper->ram, address);
				break;
			case MBC3_MODE_RTC_SEC:
				result = mapper->map.mbc3.rtc.second;
				break;
			case MBC3_MODE_RTC_MIN:
				result = mapper->map.mbc3.rtc.minute;
				break;
			case MBC3_MODE_RTC_HOUR:
				result = mapper->map.mbc3.rtc.hour;
				break;
			case MBC3_MODE_RTC_DAY_LOW:
				result = mapper->map.mbc3.rtc.day.lower;
				break;
			case MBC3_MODE_RTC_DAY_HIGH:
				result = mapper->map.mbc3.rtc.day.upper.raw;
				break;
			default:
				TRACE_FORMAT(LEVEL_WARNING, "Unsupported MBC3 mode %u", type);
				break;
		}
	} else {
		result = UINT8_MAX;
		TRACE_FORMAT(LEVEL_WARNING, "MBC3 ram disabled [%u][%04x]->%02x", type, address, result);
	}

	return result;
}

void
dmg_mapper_mbc3_write_ram(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	)
{
	int type = mapper->map.mbc3.mode;

	if(mapper->cartridge.enable) {

		switch(type) {
			case MBC3_MODE_RAM_0 ... MBC3_MODE_RAM_7:
				dmg_cartridge_write_ram(&mapper->cartridge, mapper->ram, address, value);
				break;
			case MBC3_MODE_RTC_SEC:
				mapper->map.mbc3.rtc.second = value;
				break;
			case MBC3_MODE_RTC_MIN:
				mapper->map.mbc3.rtc.minute = value;
				break;
			case MBC3_MODE_RTC_HOUR:
				mapper->map.mbc3.rtc.hour = value;
				break;
			case MBC3_MODE_RTC_DAY_LOW:
				mapper->map.mbc3.rtc.day.lower = value;
				break;
			case MBC3_MODE_RTC_DAY_HIGH:
				mapper->map.mbc3.rtc.day.upper.raw = value;
				break;
			default:
				TRACE_FORMAT(LEVEL_WARNING, "Unsupported MBC3 mode %u", type);
				break;
		}
	} else {
		TRACE_FORMAT(LEVEL_WARNING, "MBC3 ram disabled [%u][%04x]<-%02x", type, address, value);
	}
}

void
dmg_mapper_mbc3_write_rom(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	)
{

	switch(address) {
		case ADDRESS_MBC3_LATCH_BEGIN ... ADDRESS_MBC3_LATCH_END:

			if(!mapper->map.mbc3.latch && (value & MBC3_RTC_LATCH) == MBC3_RTC_LATCH) {
				time_t current = time(NULL);
				struct tm *local = localtime(&current);

				mapper->map.mbc3.rtc.second = local->tm_sec;
				mapper->map.mbc3.rtc.minute = local->tm_min;
				mapper->map.mbc3.rtc.hour = local->tm_hour;
				mapper->map.mbc3.rtc.day.lower = (local->tm_yday & UINT8_MAX);
				mapper->map.mbc3.rtc.day.upper.msb = (local->tm_yday > UINT8_MAX);

				TRACE_FORMAT(LEVEL_VERBOSE, "MBC3 rtc latch %u:%u:%u:%u",
					((mapper->map.mbc3.rtc.day.upper.msb << CHAR_BIT) | mapper->map.mbc3.rtc.day.lower),
					mapper->map.mbc3.rtc.hour, mapper->map.mbc3.rtc.minute, mapper->map.mbc3.rtc.second);
			}

			mapper->map.mbc3.latch = value;
			break;
		case ADDRESS_MBC3_RAM_ENABLE_BEGIN ... ADDRESS_MBC3_RAM_ENABLE_END:
			dmg_cartridge_ram_enable(&mapper->cartridge, (value & NIBBLE_MAX) == RAM_ENABLE);
			break;
		case ADDRESS_MBC3_RAM_BEGIN ... ADDRESS_MBC3_RAM_END:

			switch(value) {
				case MBC3_MODE_RAM_0 ... MBC3_MODE_RAM_7:
					mapper->ram = ((value & MBC3_RAM_MASK) % mapper->cartridge.ram.count);
					mapper->map.mbc3.mode = value;
					break;
				case MBC3_MODE_RTC_SEC ... MBC3_MODE_RTC_DAY_HIGH:
					mapper->map.mbc3.mode = value;
					break;
				default:
					TRACE_FORMAT(LEVEL_WARNING, "Unsupported MBC3 mode %u", value);
					break;
			}
			break;
		case ADDRESS_MBC3_ROM_BEGIN ... ADDRESS_MBC3_ROM_END:
			mapper->map.mbc3.rom = value;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported MBC3 write [%u/%u][%04x]<-%02x", mapper->rom, mapper->rom_swap,
				address, value);
			break;
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "MBC3 bank=%02x", mapper->map.mbc3.rom);

	if(!(mapper->rom_swap = (mapper->map.mbc3.rom % mapper->cartridge.rom.count))) {
		++mapper->rom_swap;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
