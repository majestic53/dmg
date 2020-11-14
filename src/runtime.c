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

static uint32_t g_cycle = 0;
static uint32_t g_cycle_last = 0;
static const dmg_t *g_configuration = NULL;
static dmg_runtime_t g_runtime = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_runtime_export(
	__in FILE *file
	)
{
	int result;

	TRACE(LEVEL_INFORMATION, "Runtime exporting");

	if((result = dmg_joypad_export(&g_runtime.joypad, file)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_memory_export(&g_runtime.memory, file)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_processor_export(&g_runtime.processor, file)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_serial_export(&g_runtime.serial, file)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_timer_export(&g_runtime.timer, file)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_video_export(&g_runtime.video, file)) != ERROR_SUCCESS) {
		goto exit;
	}

	// TODO: EXPORT SUBSYSTEMS

	TRACE(LEVEL_INFORMATION, "Runtime exported");

exit:
	return result;
}

static int
dmg_runtime_import(
	__in FILE *file
	)
{
	int result;

	TRACE(LEVEL_INFORMATION, "Runtime importing");

	if((result = dmg_joypad_import(&g_runtime.joypad, file)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_memory_import(&g_runtime.memory, file)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_processor_import(&g_runtime.processor, file)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_serial_import(&g_runtime.serial, file)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_timer_import(&g_runtime.timer, file)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_video_import(&g_runtime.video, file)) != ERROR_SUCCESS) {
		goto exit;
	}

	// TODO: IMPORT SUBSYSTEMS

	TRACE(LEVEL_INFORMATION, "Runtime imported");

exit:
	return result;
}

static int
dmg_runtime_load(
	__in const dmg_t *configuration
	)
{
	int result = ERROR_SUCCESS;
	char title[CARTRIDGE_HEADER_TITLE_LENGTH + 1] = {};

	TRACE_FORMAT(LEVEL_INFORMATION, "Runtime loading ver.%u.%u.%u",
		dmg_version_get()->major, dmg_version_get()->minor, dmg_version_get()->patch);

	if(!configuration) {
		result = ERROR_SET(ERROR_INVALID, "Configuration is NULL");
		goto exit;
	}

	g_configuration = configuration;

	if((result = dmg_joypad_load(&g_runtime.joypad, g_configuration)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_memory_load(&g_runtime.memory, g_configuration)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_processor_load(&g_runtime.processor, g_configuration)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_serial_load(&g_runtime.serial, g_configuration)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_timer_load(&g_runtime.timer, g_configuration)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_video_load(&g_runtime.video, g_configuration)) != ERROR_SUCCESS) {
		goto exit;
	}

	// TODO: LOAD SUBSYSTEMS

	for(uint32_t address = 0; address < CARTRIDGE_HEADER_TITLE_LENGTH; ++address) {
		char value = g_runtime.memory.mapper.cartridge.header->title[address];

		if(!value) {
			break;
		}

		title[address] = ((isprint(value) || isspace(value)) ? value : '?');
	}

	if((result = dmg_service_load(g_configuration, title)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import(dmg_runtime_import, g_configuration->save_in)) != ERROR_SUCCESS) {
		goto exit;
	}

	TRACE(LEVEL_INFORMATION, "Runtime loaded");

exit:
	return result;
}

static int
dmg_runtime_loop(void)
{
	int result = EXIT_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Runtime loop entry");
	g_cycle = 0;
	g_cycle_last = 0;

	for(;;) {

		if(!dmg_service_poll()) {
			TRACE(LEVEL_INFORMATION, "Runtime loop exiting");
			break;
		}

		do {
			g_cycle_last = dmg_processor_step(&g_runtime.processor);
			dmg_joypad_step(&g_runtime.joypad, g_cycle_last);
			dmg_serial_step(&g_runtime.serial, g_cycle_last);
			dmg_timer_step(&g_runtime.timer, g_cycle_last);

			// TODO: LOOP SUBSYSTEMS

			g_cycle += g_cycle_last;
		} while(!dmg_video_step(&g_runtime.video, g_cycle_last));

		dmg_service_sync();
	}

	TRACE(LEVEL_INFORMATION, "Runtime loop exit");

	return result;
}

static void
dmg_runtime_unload(void)
{
	TRACE(LEVEL_INFORMATION, "Runtime unloading");
	dmg_service_export(dmg_runtime_export, g_configuration->save_out);
	dmg_service_unload();

	// TODO: UNLOAD SUBSYSTEMS

	dmg_video_unload(&g_runtime.video);
	dmg_timer_unload(&g_runtime.timer);
	dmg_serial_unload(&g_runtime.serial);
	dmg_processor_unload(&g_runtime.processor);
	dmg_memory_unload(&g_runtime.memory);
	dmg_joypad_unload(&g_runtime.joypad);
	TRACE(LEVEL_INFORMATION, "Runtime unloaded");
	memset(&g_runtime, 0, sizeof(g_runtime));
}

int
dmg_runtime(
	__in const dmg_t *configuration
	)
{
	int result;

	TRACE_ENABLE(&g_cycle);
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
	dmg_runtime_write(ADDRESS_PROCESSOR_INTERRUPT_FLAG, dmg_runtime_read(ADDRESS_PROCESSOR_INTERRUPT_FLAG) | (1 << type));
}

uint8_t
dmg_runtime_read(
	__in uint16_t address
	)
{
	uint8_t result = 0;

	switch(address) {
		case ADDRESS_JOYPAD_STATE:
			result = dmg_joypad_read(&g_runtime.joypad, address);
			break;
		case ADDRESS_PROCESSOR_INTERRUPT_ENABLE:
		case ADDRESS_PROCESSOR_INTERRUPT_FLAG:
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
		case ADDRESS_SERIAL_CONTROL:
		case ADDRESS_SERIAL_DATA:
			result = dmg_serial_read(&g_runtime.serial, address);
			break;
		case ADDRESS_TIMER_CONTROL:
		case ADDRESS_TIMER_COUNTER:
		case ADDRESS_TIMER_DIVIDER:
		case ADDRESS_TIMER_MODULO:
			result = dmg_timer_read(&g_runtime.timer, address);
			break;
		case ADDRESS_VIDEO_BACKGROUND_PALETTE:
		case ADDRESS_VIDEO_CONTROL:
		case ADDRESS_VIDEO_LINE:
		case ADDRESS_VIDEO_LINE_COINCIDENCE:
		case ADDRESS_VIDEO_OBJECT_PALETTE_0:
		case ADDRESS_VIDEO_OBJECT_PALETTE_1:
		case ADDRESS_VIDEO_RAM_BEGIN ... ADDRESS_VIDEO_RAM_END:
		case ADDRESS_VIDEO_RAM_SPRITE_BEGIN ... ADDRESS_VIDEO_RAM_SPRITE_END:
		case ADDRESS_VIDEO_SCREEN_X:
		case ADDRESS_VIDEO_SCREEN_Y:
		case ADDRESS_VIDEO_STATUS:
		case ADDRESS_VIDEO_WINDOW_X:
		case ADDRESS_VIDEO_WINDOW_Y:
			result = dmg_video_read(&g_runtime.video, address);
			break;

		// TODO: READ BYTE FROM SUBSYSTEMS

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
		case ADDRESS_JOYPAD_STATE:
			dmg_joypad_write(&g_runtime.joypad, address, value);
			break;
		case ADDRESS_PROCESSOR_INTERRUPT_ENABLE:
		case ADDRESS_PROCESSOR_INTERRUPT_FLAG:
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
		case ADDRESS_SERIAL_CONTROL:
		case ADDRESS_SERIAL_DATA:
			dmg_serial_write(&g_runtime.serial, address, value);
			break;
		case ADDRESS_TIMER_CONTROL:
		case ADDRESS_TIMER_COUNTER:
		case ADDRESS_TIMER_DIVIDER:
		case ADDRESS_TIMER_MODULO:
			dmg_timer_write(&g_runtime.timer, address, value);
			break;
		case ADDRESS_VIDEO_BACKGROUND_PALETTE:
		case ADDRESS_VIDEO_CONTROL:
		case ADDRESS_VIDEO_LINE_COINCIDENCE:
		case ADDRESS_VIDEO_OBJECT_PALETTE_0:
		case ADDRESS_VIDEO_OBJECT_PALETTE_1:
		case ADDRESS_VIDEO_RAM_BEGIN ... ADDRESS_VIDEO_RAM_END:
		case ADDRESS_VIDEO_RAM_SPRITE_BEGIN ... ADDRESS_VIDEO_RAM_SPRITE_END:
		case ADDRESS_VIDEO_SCREEN_X:
		case ADDRESS_VIDEO_SCREEN_Y:
		case ADDRESS_VIDEO_STATUS:
		case ADDRESS_VIDEO_TRANSFER:
		case ADDRESS_VIDEO_WINDOW_X:
		case ADDRESS_VIDEO_WINDOW_Y:
			dmg_video_write(&g_runtime.video, address, value);
			break;

		// TODO: WRITE BYTE TO SUBSYSTEMS

		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported write [%04x]<-%02x", address, value);
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
