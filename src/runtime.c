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

#include "./runtime_type.h"

static dmg_runtime_t g_runtime = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_runtime_load(
	__in const dmg_t *configuration
	)
{
	int result = ERROR_SUCCESS;

	TRACE_FORMAT(LEVEL_INFORMATION, "Runtime loading ver.%u.%u.%u",
		dmg_version_get()->major, dmg_version_get()->minor, dmg_version_get()->patch);

	if(!configuration) {
		result = ERROR_SET(ERROR_INVALID, "Configuration is NULL");
		goto exit;
	}

	if((result = dmg_service_load()) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_memory_load(&g_runtime.memory, &configuration->bootrom, &configuration->rom)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_processor_load(&g_runtime.processor, &configuration->bootrom)) != ERROR_SUCCESS) {
		goto exit;
	}

	// TODO: LOAD SUBSYSTEMS

	TRACE(LEVEL_INFORMATION, "Runtime loaded");

exit:
	return result;
}

static int
dmg_runtime_loop(void)
{
	uint32_t cycle = 0;
	int result = EXIT_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Runtime loop entry");

	g_runtime.cycle = 0;
	g_runtime.cycle_last = 0;

	for(;;) {

		if(!dmg_service_poll()) {
			TRACE(LEVEL_WARNING, "Runtime exiting");
			break;
		}

		while(cycle < CYCLE_PER_SEC) {
			g_runtime.cycle_last = dmg_processor_step(&g_runtime.processor);
			g_runtime.cycle += g_runtime.cycle_last;
			cycle += g_runtime.cycle_last;

			// TODO: LOOP SUBSYSTEMS
		}

		cycle %= CYCLE_PER_SEC;
		dmg_service_sync();
	}

	TRACE(LEVEL_INFORMATION, "Runtime loop exit");

	return result;
}

static void
dmg_runtime_unload(void)
{
	TRACE(LEVEL_INFORMATION, "Runtime unloading");

	// TODO: UNLOAD SUBSYSTEMS

	dmg_processor_unload(&g_runtime.processor);
	dmg_memory_unload(&g_runtime.memory);
	dmg_service_unload();

	TRACE(LEVEL_INFORMATION, "Runtime unloaded");

	memset(&g_runtime, 0, sizeof(g_runtime));
}

int
dmg_runtime(
	__in const dmg_t *configuration
	)
{
	int result;

	TRACE_ENABLE(&g_runtime.cycle);
	ERROR_CLEAR();

	if((result = dmg_runtime_load(configuration)) == ERROR_SUCCESS) {
		result = dmg_runtime_loop();
	}

	dmg_runtime_unload();

	return result;
}

void
dmg_runtime_interrupt(
	__in int type
	)
{
	dmg_runtime_write(ADDRESS_INTERRUPT_FLAG, dmg_runtime_read(ADDRESS_INTERRUPT_FLAG) | (1 << type));
}

uint8_t
dmg_runtime_read(
	__in uint16_t address
	)
{
	uint8_t result = 0;

	switch(address) {
		case ADDRESS_INTERRUPT_ENABLE:
		case ADDRESS_INTERRUPT_FLAG:
			result = dmg_processor_read(&g_runtime.processor, address);
			break;
		case ADDRESS_RAM_BEGIN ... ADDRESS_RAM_END:
		case ADDRESS_RAM_ECHO_BEGIN ... ADDRESS_RAM_ECHO_END:
		case ADDRESS_RAM_HIGH_BEGIN ... ADDRESS_RAM_HIGH_END:
		case ADDRESS_RAM_SWAP_BEGIN ... ADDRESS_RAM_SWAP_END:
		case ADDRESS_RAM_UNUSED_BEGIN ... ADDRESS_RAM_UNUSED_END:
		case ADDRESS_ROM_BEGIN ... ADDRESS_ROM_END:
		case ADDRESS_ROM_SWAP_BEGIN ... ADDRESS_ROM_SWAP_END:
			result = dmg_memory_read(&g_runtime.memory, address);
			break;

		// TODO: READ BYTE FROM SUBSYSTEM

		default:
			result = UINT8_MAX;

			TRACE_FORMAT(LEVEL_WARNING, "Unsupported read [%04x]->%02x", address, result);
			break;
	}

	return result;
}

void
dmg_runtime_write(
	__in uint16_t address,
	__in uint8_t value
	)
{

	switch(address) {
		case ADDRESS_INTERRUPT_ENABLE:
		case ADDRESS_INTERRUPT_FLAG:
			dmg_processor_write(&g_runtime.processor, address, value);
			break;
		case ADDRESS_BOOTROM_DISABLE:
		case ADDRESS_RAM_BEGIN ... ADDRESS_RAM_END:
		case ADDRESS_RAM_ECHO_BEGIN ... ADDRESS_RAM_ECHO_END:
		case ADDRESS_RAM_HIGH_BEGIN ... ADDRESS_RAM_HIGH_END:
		case ADDRESS_RAM_SWAP_BEGIN ... ADDRESS_RAM_SWAP_END:
		case ADDRESS_RAM_UNUSED_BEGIN ... ADDRESS_RAM_UNUSED_END:
		case ADDRESS_ROM_BEGIN ... ADDRESS_ROM_END:
		case ADDRESS_ROM_SWAP_BEGIN ... ADDRESS_ROM_SWAP_END:
			dmg_memory_write(&g_runtime.memory, address, value);
			break;

		// TODO: WRITE BYTE TO SUBSYSTEM

		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported write [%04x]<-%02x", address, value);
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
