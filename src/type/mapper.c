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

#include "./mapper_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NDEBUG

static void
dmg_mapper_trace(
	__in int level,
	__inout const dmg_mapper_t *mapper
	)
{

	if(mapper->cartridge.header) {
		TRACE_FORMAT(level, "Mapper type=%u", mapper->cartridge.header->mapper);
	}

	TRACE_FORMAT(level, "Mapper rom bank=%u", mapper->rom);
	TRACE_FORMAT(level, "Mapper rom-swap bank=%u", mapper->rom_swap);
	TRACE_FORMAT(level, "Mapper ram bank=%u", mapper->ram);
}

#endif /* NDEBUG */

static void
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

static uint8_t
dmg_mapper_mbc3_read_ram(
	__in const dmg_mapper_t *mapper,
	__in uint16_t address
	)
{
	int type;
	uint8_t result = 0;

	switch((type = mapper->map.mbc3.mode)) {
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

	return result;
}

static void
dmg_mapper_mbc3_write_ram(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	)
{
	int type;

	switch((type = mapper->map.mbc3.mode)) {
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
}

static void
dmg_mapper_mbc3_write_rom(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	)
{

	switch(address) {
		case ADDRESS_MBC3_LATCH_BEGIN ... ADDRESS_MBC3_LATCH_END:

			if((value & MBC3_RTC_LATCH) == MBC3_RTC_LATCH) {

				// TODO: LATCH RTC

				TRACE_FORMAT(LEVEL_VERBOSE, "MBC3 rtc latch %u:%u:%u:%u",
					((mapper->map.mbc3.rtc.day.upper.msb << CHAR_BIT) | mapper->map.mbc3.rtc.day.lower),
					mapper->map.mbc3.rtc.hour, mapper->map.mbc3.rtc.minute, mapper->map.mbc3.rtc.second);
			}
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

static void
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

int
dmg_mapper_export(
	__in const dmg_mapper_t *mapper,
	__in FILE *file
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Mapper exporting");
	TRACE_MAPPER(LEVEL_VERBOSE, mapper);

	if((result = dmg_service_export_data(file, &mapper->map, sizeof(mapper->map))) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_cartridge_export(&mapper->cartridge, file)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &mapper->ram, sizeof(mapper->ram))) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &mapper->rom, sizeof(mapper->rom))) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &mapper->rom_swap, sizeof(mapper->rom_swap))) != ERROR_SUCCESS) {
		goto exit;
	}

	TRACE(LEVEL_INFORMATION, "Mapper exported");

exit:
	return result;
}

int
dmg_mapper_import(
	__inout dmg_mapper_t *mapper,
	__in FILE *file
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Mapper importing");

	if((result = dmg_service_import_data(file, &mapper->map, sizeof(mapper->map))) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_cartridge_import(&mapper->cartridge, file)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &mapper->ram, sizeof(mapper->ram))) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &mapper->rom, sizeof(mapper->rom))) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &mapper->rom_swap, sizeof(mapper->rom_swap))) != ERROR_SUCCESS) {
		goto exit;
	}

	TRACE_MAPPER(LEVEL_VERBOSE, mapper);
	TRACE(LEVEL_INFORMATION, "Mapper imported");

exit:
	return result;
}

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
		case MAPPER_MBC1:
		case MAPPER_MBC1_RAM:
		case MAPPER_MBC1_RAM_BATTERY:
		case MAPPER_MBC3_RAM:
		case MAPPER_MBC3_RAM_BATTERY:
		case MAPPER_MBC5:
		case MAPPER_MBC5_RAM:
		case MAPPER_MBC5_RAM_BATTERY:
		case MAPPER_MBC5_RUMBLE:
		case MAPPER_MBC5_RUMBLE_RAM:
		case MAPPER_MBC5_RUMBLE_RAM_BATTERY:
			mapper->ram = 0;
			mapper->rom = 0;
			mapper->rom_swap = (mapper->rom + 1);
			break;
		default:
			result = ERROR_SET_FORMAT(ERROR_INVALID, "Unsupported mapper type: %u", type);
			goto exit;
	}

	TRACE_MAPPER(LEVEL_VERBOSE, mapper);
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
	int type;
	uint8_t result = 0;

	switch(address) {
		case ADDRESS_RAM_SWAP_BEGIN ... ADDRESS_RAM_SWAP_END:

			switch((type = mapper->cartridge.header->mapper)) {
				case MAPPER_MBC3_RAM:
				case MAPPER_MBC3_RAM_BATTERY:
					result = dmg_mapper_mbc3_read_ram(mapper, address - ADDRESS_RAM_SWAP_BEGIN);
					break;
				default:
					result = dmg_cartridge_read_ram(&mapper->cartridge, mapper->ram, address - ADDRESS_RAM_SWAP_BEGIN);
					break;
			}
			break;
		default:
			result = UINT8_MAX;
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported mapper ram read [%u][%04x]->%02x", mapper->ram, address, result);
			break;
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
	int type;

	switch(address) {
		case ADDRESS_RAM_SWAP_BEGIN ... ADDRESS_RAM_SWAP_END:

			switch((type = mapper->cartridge.header->mapper)) {
				case MAPPER_MBC3_RAM:
				case MAPPER_MBC3_RAM_BATTERY:
					dmg_mapper_mbc3_write_ram(mapper, address - ADDRESS_RAM_SWAP_BEGIN, value);
					break;
				default:
					dmg_cartridge_write_ram(&mapper->cartridge, mapper->ram, address - ADDRESS_RAM_SWAP_BEGIN, value);
					break;
			}
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported mapper ram write [%u][%04x]->%02x", mapper->ram, address, value);
			break;
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
		case MAPPER_MBC1:
		case MAPPER_MBC1_RAM:
		case MAPPER_MBC1_RAM_BATTERY:
			dmg_mapper_mbc1_write_rom(mapper, address, value);
			break;
		case MAPPER_MBC3_RAM:
		case MAPPER_MBC3_RAM_BATTERY:
			dmg_mapper_mbc3_write_rom(mapper, address, value);
			break;
		case MAPPER_MBC5:
		case MAPPER_MBC5_RAM:
		case MAPPER_MBC5_RAM_BATTERY:
		case MAPPER_MBC5_RUMBLE:
		case MAPPER_MBC5_RUMBLE_RAM:
		case MAPPER_MBC5_RUMBLE_RAM_BATTERY:
			dmg_mapper_mbc5_write_rom(mapper, address, value);
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported mapper type: %u", type);
			break;
	}

	TRACE_MAPPER(LEVEL_VERBOSE, mapper);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
