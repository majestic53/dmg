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

#include "./memory_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_memory_load(
	__inout dmg_memory_t *memory,
	__in const dmg_buffer_t *bootrom,
	__in const dmg_buffer_t *rom
	)
{
	int result;

	TRACE(LEVEL_INFORMATION, "Memory loading");

	if(bootrom->data) {

		if((result = dmg_bootrom_load(&memory->bootrom, bootrom)) != ERROR_SUCCESS) {
			goto exit;
		}

		memory->bootrom_enable = true;
	}

	if((result = dmg_mapper_load(&memory->mapper, rom)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_buffer_allocate(&memory->ram, RAM_WIDTH, UINT8_MAX)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_buffer_allocate(&memory->ram_high, RAM_HIGH_WIDTH, UINT8_MAX)) != ERROR_SUCCESS) {
		goto exit;
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "Ram[%04x]=%p", memory->ram.length, memory->ram.data);
	TRACE_FORMAT(LEVEL_VERBOSE, "Ram-High[%04x]=%p", memory->ram_high.length, memory->ram_high.data);
	TRACE(LEVEL_INFORMATION, "Memory loaded");

exit:
	return result;
}

uint8_t
dmg_memory_read(
	__in const dmg_memory_t *memory,
	__in uint16_t address
	)
{
	uint8_t result = 0;

	switch(address) {
		case ADDRESS_RAM_BEGIN ... ADDRESS_RAM_END:
			result = memory->ram.data[address - ADDRESS_RAM_BEGIN];
			break;
		case ADDRESS_RAM_ECHO_BEGIN ... ADDRESS_RAM_ECHO_END:
			result = memory->ram.data[address - ADDRESS_RAM_ECHO_BEGIN];
			break;
		case ADDRESS_RAM_HIGH_BEGIN ... ADDRESS_RAM_HIGH_END:
			result = memory->ram_high.data[address - ADDRESS_RAM_HIGH_BEGIN];
			break;
		case ADDRESS_RAM_SWAP_BEGIN ... ADDRESS_RAM_SWAP_END:
			result = dmg_mapper_read_ram(&memory->mapper, address);
			break;
		case ADDRESS_RAM_UNUSED_BEGIN ... ADDRESS_RAM_UNUSED_END:
			TRACE_FORMAT(LEVEL_WARNING, "Unused memory read [%04x]->%02x", address, result);
			break;
		case ADDRESS_ROM_BEGIN ... ADDRESS_ROM_END:
		case ADDRESS_ROM_SWAP_BEGIN ... ADDRESS_ROM_SWAP_END:

			if(memory->bootrom_enable && (address <= ADDRESS_BOOTROM_END)) {
				result = dmg_bootrom_read(&memory->bootrom, address);
			} else {
				result = dmg_mapper_read_rom(&memory->mapper, address);
			}
			break;
		default:
			result = UINT8_MAX;

			TRACE_FORMAT(LEVEL_WARNING, "Unsupported memory read [%04x]->%02x", address, result);
			break;
	}

	return result;
}

void
dmg_memory_unload(
	__inout dmg_memory_t *memory
	)
{
	TRACE(LEVEL_INFORMATION, "Memory unloading");

	dmg_buffer_free(&memory->ram_high);
	dmg_buffer_free(&memory->ram);
	dmg_mapper_unload(&memory->mapper);
	dmg_bootrom_unload(&memory->bootrom);
	memset(memory, 0, sizeof(*memory));

	TRACE(LEVEL_INFORMATION, "Memory unloaded");
}

void
dmg_memory_write(
	__inout dmg_memory_t *memory,
	__in uint16_t address,
	__in uint8_t value
	)
{

	switch(address) {
		case ADDRESS_BOOTROM_DISABLE:

			if(memory->bootrom_enable) {
				memory->bootrom_enable = false;

				TRACE_FORMAT(LEVEL_INFORMATION, "Bootrom disabled [%04x]<-%02x", address, value);
			}
			break;
		case ADDRESS_RAM_BEGIN ... ADDRESS_RAM_END:
			memory->ram.data[address - ADDRESS_RAM_BEGIN] = value;
			break;
		case ADDRESS_RAM_ECHO_BEGIN ... ADDRESS_RAM_ECHO_END:
			memory->ram.data[address - ADDRESS_RAM_ECHO_BEGIN] = value;
			break;
		case ADDRESS_RAM_HIGH_BEGIN ... ADDRESS_RAM_HIGH_END:
			memory->ram_high.data[address - ADDRESS_RAM_HIGH_BEGIN] = value;
			break;
		case ADDRESS_RAM_SWAP_BEGIN ... ADDRESS_RAM_SWAP_END:
			dmg_mapper_write_ram(&memory->mapper, address, value);
			break;
		case ADDRESS_RAM_UNUSED_BEGIN ... ADDRESS_RAM_UNUSED_END:
			TRACE_FORMAT(LEVEL_WARNING, "Unused memory write [%04x]<-%02x", address, value);
			break;
		case ADDRESS_ROM_BEGIN ... ADDRESS_ROM_END:
		case ADDRESS_ROM_SWAP_BEGIN ... ADDRESS_ROM_SWAP_END:
			dmg_mapper_write_rom(&memory->mapper, address, value);
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported memory write [%04x]<-%02x", address, value);
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
