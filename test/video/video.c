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

#include "../../include/system/video.h"
#include "../../src/system/video_type.h"
#include "../include/common.h"

#define TRANSFER_BASE 0x10

typedef struct {
	dmg_t configuration;
	int export;
	FILE *export_file;
	const void *export_data;
	uint32_t export_length;
	int import;
	FILE *import_file;
	void *import_data;
	uint32_t import_length;
	dmg_video_t video;
	uint16_t address;
	uint8_t value;
	bool interrupt_lcdc;
	bool interrupt_vblank;
} dmg_video_test_t;

static dmg_video_test_t g_video = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_buffer_allocate(
	__inout dmg_buffer_t *buffer,
	__in uint32_t length,
	__in uint8_t value
	)
{
	return DMG_STATUS_SUCCESS;
}

void
dmg_buffer_free(
	__inout dmg_buffer_t *buffer
	)
{
	return;
}

void
dmg_runtime_interrupt(
	__in int type
	)
{

	switch(type) {
		case INTERRUPT_LCDC:
			g_video.interrupt_lcdc = true;
			break;
		case INTERRUPT_VBLANK:
			g_video.interrupt_vblank = true;
			break;
		default:
			break;
	}
}

uint8_t
dmg_runtime_read(
	__in uint16_t address
	)
{
	g_video.address = address;

	return g_video.value;
}

void
dmg_runtime_write(
	__in uint16_t address,
	__in uint8_t value
	)
{
	g_video.address = address;
	g_video.value = value;
}

int
dmg_service_export_data(
	__in FILE *file,
	__in const void *data,
	__in uint32_t length
	)
{
	g_video.export_file = file;
	g_video.export_data = data;
	g_video.export_length = length;

	return g_video.export;
}

int
dmg_service_import_data(
	__in FILE *file,
	__in void *data,
	__in uint32_t length
	)
{
	g_video.import_file = file;
	g_video.import_data = data;
	g_video.import_length = length;

	return g_video.import;
}

void
dmg_service_pixel(
	__in uint8_t color,
	__in uint8_t x,
	__in uint8_t y
	)
{
	return;
}

static void
dmg_test_video_initialize(void)
{
	memset(&g_video, 0, sizeof(g_video));
}

int
dmg_test_video_load(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_video_initialize();

	if(ASSERT_SUCCESS(dmg_video_load(&g_video.video, &g_video.configuration))) {
		result = EXIT_FAILURE;
	}

	if(ASSERT(g_video.video.background.raw == POST_BACKGROUND_PALETTE)
			|| ASSERT(g_video.video.control.raw == POST_CONTROL)
			|| ASSERT(g_video.video.line == 0)
			|| ASSERT(g_video.video.line_coincidence == POST_LINE_COINCIDENCE)
			|| ASSERT(g_video.video.object_0.raw == POST_OBJECT_PALETTE_0)
			|| ASSERT(g_video.video.object_1.raw == POST_OBJECT_PALETTE_1)
			|| ASSERT(g_video.video.screen_x == POST_SCREEN_X)
			|| ASSERT(g_video.video.screen_y == POST_SCREEN_Y)
			|| ASSERT(g_video.video.status.mode == MODE_SEARCH)
			|| ASSERT(g_video.video.window_x == POST_WINDOW_X)
			|| ASSERT(g_video.video.window_y == POST_WINDOW_Y)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	g_video.configuration.bootrom.data = (void *)1;

	if(ASSERT_SUCCESS(dmg_video_load(&g_video.video, &g_video.configuration))) {
		result = EXIT_FAILURE;
	}

	if(ASSERT(g_video.video.background.raw == 0)
			|| ASSERT(g_video.video.control.raw == 0)
			|| ASSERT(g_video.video.line == 0)
			|| ASSERT(g_video.video.line_coincidence == 0)
			|| ASSERT(g_video.video.object_0.raw == 0)
			|| ASSERT(g_video.video.object_1.raw == 0)
			|| ASSERT(g_video.video.screen_x == 0)
			|| ASSERT(g_video.video.screen_y == 0)
			|| ASSERT(g_video.video.status.mode == MODE_SEARCH)
			|| ASSERT(g_video.video.window_x == 0)
			|| ASSERT(g_video.video.window_y == 0)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_video_read(void)
{
	uint8_t value = rand();
	int result = EXIT_SUCCESS;

	dmg_test_video_initialize();

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_BACKGROUND_PALETTE - 1) == UINT8_MAX)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	g_video.video.background.raw = rand();

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_BACKGROUND_PALETTE) == g_video.video.background.raw)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	g_video.video.control.raw = rand();

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_CONTROL) == g_video.video.control.raw)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	g_video.video.line = rand();

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_LINE) == g_video.video.line)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	g_video.video.line_coincidence = rand();

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_LINE_COINCIDENCE) == g_video.video.line_coincidence)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	g_video.video.object_0.raw = rand();

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_OBJECT_PALETTE_0) == g_video.video.object_0.raw)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	g_video.video.object_1.raw = rand();

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_OBJECT_PALETTE_1) == g_video.video.object_1.raw)) {
		result = EXIT_FAILURE;
	}

	for(int mode = 0; mode < MODE_MAX; ++mode) {
		dmg_test_video_initialize();
		g_video.video.control.enable = true;
		g_video.video.ram.data = &value;

		switch(g_video.video.status.mode) {
			case MODE_HBLANK:
			case MODE_SEARCH:
			case MODE_VBLANK:

				if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_RAM_BEGIN) == value)) {
					result = EXIT_FAILURE;
				}
				break;
			default:

				if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_RAM_BEGIN) == 0)) {
					result = EXIT_FAILURE;
				}
				break;
		}

		dmg_test_video_initialize();
		g_video.video.control.enable = true;
		g_video.video.ram_sprite.data = &value;

		switch(g_video.video.status.mode) {
			case MODE_HBLANK:
			case MODE_VBLANK:

				if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_RAM_SPRITE_BEGIN) == value)) {
					result = EXIT_FAILURE;
				}
				break;
			default:

				if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_RAM_SPRITE_BEGIN) == 0)) {
					result = EXIT_FAILURE;
				}
				break;
		}
	}

	dmg_test_video_initialize();
	g_video.video.screen_x = rand();

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_SCREEN_X) == g_video.video.screen_x)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	g_video.video.screen_y = rand();

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_SCREEN_Y) == g_video.video.screen_y)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	g_video.video.status.raw = rand();

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_STATUS) == g_video.video.status.raw)) {
		result = EXIT_FAILURE;
	}

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_TRANSFER) == UINT8_MAX)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	g_video.video.window_x = rand();

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_WINDOW_X) == g_video.video.window_x)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	g_video.video.window_y = rand();

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_WINDOW_Y) == g_video.video.window_y)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_video_step(void)
{
	int result = EXIT_SUCCESS;
	uint16_t destination, source;

	dmg_test_video_initialize();
	dmg_video_load(&g_video.video, &g_video.configuration);
	destination = ADDRESS_VIDEO_RAM_SPRITE_BEGIN;
	source = (TRANSFER_SCALE * TRANSFER_BASE);
	g_video.video.transfer.enable = true;
	g_video.video.transfer.destination = destination;
	g_video.video.transfer.source = source;
	g_video.value = rand();

	for(uint32_t cycle = 0; cycle < (CYCLE * RAM_SPRITE_WIDTH); cycle += CYCLE) {

		if(ASSERT(g_video.video.transfer.enable == true)
				|| ASSERT(g_video.video.transfer.destination == destination++)
				|| ASSERT(g_video.video.transfer.source == source++)) {
			result = EXIT_FAILURE;
		}

		dmg_video_step(&g_video.video, CYCLE);
	}

	dmg_video_step(&g_video.video, CYCLE);

	if(ASSERT(g_video.video.transfer.enable == false)
			|| ASSERT(g_video.video.transfer.destination == (ADDRESS_VIDEO_RAM_SPRITE_END + 1))
			|| ASSERT(g_video.video.transfer.source == ((TRANSFER_SCALE * TRANSFER_BASE) + RAM_SPRITE_WIDTH))) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	dmg_video_load(&g_video.video, &g_video.configuration);
	g_video.video.status.hblank = true;
	g_video.video.status.search = true;
	g_video.video.status.vblank = true;

	for(uint32_t line = 0; line < (LINE_HBLANK_MAX * 3); ++line) {
		int mode = g_video.video.status.mode;

		if(ASSERT(g_video.interrupt_vblank == false)) {
			result = EXIT_FAILURE;
		}

		for(uint32_t cycle = 0; cycle < (MODE_CYC[mode] - CYCLE); cycle += CYCLE) {
			dmg_video_step(&g_video.video, CYCLE);
		}

		dmg_video_step(&g_video.video, CYCLE);

		if(ASSERT(g_video.video.status.mode != mode)) {
			result = EXIT_FAILURE;
		}

		switch(g_video.video.status.mode) {
			case MODE_HBLANK:
			case MODE_SEARCH:
			case MODE_VBLANK:

				if(ASSERT(g_video.interrupt_lcdc == true)) {
					result = EXIT_FAILURE;
				}

				g_video.interrupt_lcdc = false;
				break;
			default:
				break;
		}
	}

	if(ASSERT(g_video.video.status.mode == MODE_VBLANK)
			|| ASSERT(g_video.interrupt_vblank == true)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_video_unload(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_video_initialize();
	dmg_video_load(&g_video.video, &g_video.configuration);
	dmg_video_unload(&g_video.video);

	if(ASSERT(g_video.video.background.raw == 0)
			|| ASSERT(g_video.video.control.raw == 0)
			|| ASSERT(g_video.video.line == 0)
			|| ASSERT(g_video.video.line_coincidence == 0)
			|| ASSERT(g_video.video.object_0.raw == 0)
			|| ASSERT(g_video.video.object_1.raw == 0)
			|| ASSERT(g_video.video.screen_x == 0)
			|| ASSERT(g_video.video.screen_y == 0)
			|| ASSERT(g_video.video.status.mode == 0)
			|| ASSERT(g_video.video.window_x == 0)
			|| ASSERT(g_video.video.window_y == 0)
			|| ASSERT(g_video.video.ram.data == NULL)
			|| ASSERT(g_video.video.ram_sprite.data == NULL)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_video_write(void)
{
	uint8_t value = rand();
	int result = EXIT_SUCCESS;

	dmg_test_video_initialize();
	dmg_video_write(&g_video.video, ADDRESS_VIDEO_BACKGROUND_PALETTE, value);

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_BACKGROUND_PALETTE) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	g_video.video.window_x = rand();
	g_video.video.window_y = rand();
	dmg_video_write(&g_video.video, ADDRESS_VIDEO_CONTROL, value);

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_CONTROL) == value)) {
		result = EXIT_FAILURE;
	}

	if(g_video.video.control.enable && ASSERT(g_video.video.line == 0)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	g_video.video.line = (value + 1);
	dmg_video_write(&g_video.video, ADDRESS_VIDEO_LINE, value);

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_LINE) == (uint8_t)(value + 1))) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	dmg_video_write(&g_video.video, ADDRESS_VIDEO_LINE_COINCIDENCE, value);

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_LINE_COINCIDENCE) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	dmg_video_write(&g_video.video, ADDRESS_VIDEO_OBJECT_PALETTE_0, value);

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_OBJECT_PALETTE_0) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	dmg_video_write(&g_video.video, ADDRESS_VIDEO_OBJECT_PALETTE_1, value);

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_OBJECT_PALETTE_1) == value)) {
		result = EXIT_FAILURE;
	}

	for(int mode = 0; mode < MODE_MAX; ++mode) {
		uint8_t data;

		dmg_test_video_initialize();
		g_video.video.control.enable = true;
		g_video.video.ram.data = &data;
		data = (value + 1);
		dmg_video_write(&g_video.video, ADDRESS_VIDEO_RAM_BEGIN, value);

		switch(g_video.video.status.mode) {
			case MODE_HBLANK:
			case MODE_SEARCH:
			case MODE_VBLANK:

				if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_RAM_BEGIN) == value)) {
					result = EXIT_FAILURE;
				}
				break;
			default:

				if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_RAM_BEGIN) == 0)) {
					result = EXIT_FAILURE;
				}
				break;
		}

		dmg_test_video_initialize();
		g_video.video.control.enable = true;
		g_video.video.ram_sprite.data = &data;
		data = (value + 1);
		dmg_video_write(&g_video.video, ADDRESS_VIDEO_RAM_SPRITE_BEGIN, value);

		switch(g_video.video.status.mode) {
			case MODE_HBLANK:
			case MODE_VBLANK:

				if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_RAM_SPRITE_BEGIN) == value)) {
					result = EXIT_FAILURE;
				}
				break;
			default:

				if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_RAM_SPRITE_BEGIN) == 0)) {
					result = EXIT_FAILURE;
				}
				break;
		}
	}

	dmg_test_video_initialize();
	dmg_video_write(&g_video.video, ADDRESS_VIDEO_SCREEN_X, value);

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_SCREEN_X) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	dmg_video_write(&g_video.video, ADDRESS_VIDEO_SCREEN_Y, value);

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_SCREEN_Y) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	dmg_video_write(&g_video.video, ADDRESS_VIDEO_STATUS, value);

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_STATUS) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	dmg_video_write(&g_video.video, ADDRESS_VIDEO_TRANSFER, value & UINT8_MAX);

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_TRANSFER) == UINT8_MAX)
			|| ASSERT(g_video.video.transfer.enable == true)
			|| ASSERT(g_video.video.transfer.destination == ADDRESS_VIDEO_RAM_SPRITE_BEGIN)
			|| ASSERT(g_video.video.transfer.source == (TRANSFER_SCALE * (value & UINT8_MAX)))) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	dmg_video_write(&g_video.video, ADDRESS_VIDEO_WINDOW_X, value);

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_WINDOW_X) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_video_initialize();
	dmg_video_write(&g_video.video, ADDRESS_VIDEO_WINDOW_Y, value);

	if(ASSERT(dmg_video_read(&g_video.video, ADDRESS_VIDEO_WINDOW_Y) == value)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

static const dmg_test TEST[] = {
	dmg_test_video_load,
	dmg_test_video_read,
	dmg_test_video_step,
	dmg_test_video_unload,
	dmg_test_video_write,
	};

int
main(
	__in int argc,
	__in char *argv[]
	)
{
	int result = EXIT_SUCCESS;

	if(argc > 1) {
		TEST_SEED(strtol(argv[1], NULL, 16));
	} else {
		TEST_SEED(time(NULL));
	}

	for(size_t trial = 0; trial < TEST_TRIALS; ++trial) {
		TRACE_TEST_TRIAL(trial);

		for(size_t test = 0; test < TEST_COUNT(TEST); ++test) {

			if(TEST[test]() != EXIT_SUCCESS) {
				result = EXIT_FAILURE;
			}
		}
	}

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
