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

#include "./video_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NDEBUG

static void
dmg_video_trace(
	__in int level,
	__inout const dmg_video_t *video
	)
{
	TRACE_FORMAT(level, "Video control=%02x [Enable=%x (%c%c%c)]", video->control.raw, video->control.enable,
		video->control.background ? 'B' : '-', video->control.sprite ? 'S' : '-', video->control.window ? 'W' : '-');
	TRACE_FORMAT(level, "Video status=%02x [Mode=%02x, Coin=%x, (%c%c%c%c)]", video->status.raw, video->status.mode,
		video->status.coincidence, video->status.hblank ? 'H' : '-', video->status.vblank ? 'V' : '-',
		video->status.search ? 'S' : '-', video->status.line_coincidence ? 'C' : '-');
	TRACE_FORMAT(level, "Video background=%02x (%02x, %02x, %02x, %02x)", video->background.raw,
		video->background.white, video->background.grey_light, video->background.grey_dark, video->background.black);
	TRACE_FORMAT(level, "Video object-0=%02x (%02x, %02x, %02x, %02x)", video->object_0.raw,
		video->object_0.white, video->object_0.grey_light, video->object_0.grey_dark, video->object_0.black);
	TRACE_FORMAT(level, "Video object-1=%02x (%02x, %02x, %02x, %02x)", video->object_1.raw,
		video->object_1.white, video->object_1.grey_light, video->object_1.grey_dark, video->object_1.black);
	TRACE_FORMAT(level, "Video line/coincidence=%02x, %02x", video->line, video->line_coincidence);
	TRACE_FORMAT(level, "Video screen-x/y=%02x, %02x", video->screen_x, video->screen_y);
	TRACE_FORMAT(level, "Video window-x/y=%02x, %02x", video->window_x, video->window_y);
	TRACE_FORMAT(level, "Video ram[%04x]=%p", video->ram.length, video->ram.data);
	TRACE_FORMAT(level, "Video ram-sprite[%04x]=%p", video->ram_sprite.length, video->ram_sprite.data);
}

static void
dmg_video_trace_transfer(
	__in int level,
	__inout dmg_video_t *video
	)
{
	TRACE_FORMAT(level, "DMA enable=%x (remaining=%u)", video->transfer.enable,
		(ADDRESS_VIDEO_RAM_SPRITE_END + 1) - video->transfer.destination);
	TRACE_FORMAT(level, "DMA destination=%04x", video->transfer.destination);
	TRACE_FORMAT(level, "DMA source=%04x", video->transfer.source);
}

#endif /* NDEBUG */

#ifndef UNITTEST

static uint8_t
dmg_video_palette_color(
	__in const dmg_video_palette_t *palette,
	__in uint8_t color
	)
{
	uint8_t result = color;

	switch(color) {
		case DMG_PALETTE_WHITE:
			result = palette->white;
			break;
		case DMG_PALETTE_GREY_LIGHT:
			result = palette->grey_light;
			break;
		case DMG_PALETTE_GREY_DARK:
			result = palette->grey_dark;
			break;
		case DMG_PALETTE_BLACK:
			result = palette->black;
			break;
		default:
			break;
	}

	return result;
}

static uint8_t
dmg_video_pixel_color(
	__in const dmg_video_tile_t *tile,
	__in uint8_t x,
	__in uint8_t y
	)
{
	return ((((tile->line[y].high >> (TILE_WIDTH - x - 1)) & 1) << 1) | ((tile->line[y].low >> (TILE_WIDTH - x - 1)) & 1));
}

static const dmg_video_tile_t *
dmg_video_tile_background(
	__in const void *ram,
	__in int map,
	__in int data,
	__in uint8_t x,
	__in uint8_t y
	)
{
	uint16_t address = (TILE_MAP[map] + (y * TILE_PITCH) + x);

	if(!data) {
		address = (TILE_DATA[data] + (sizeof(dmg_video_tile_t) * (((int8_t)((uint8_t *)ram)[address]) + (INT8_MAX + 1))));
	} else {
		address = (TILE_DATA[data] + (sizeof(dmg_video_tile_t) * ((uint8_t *)ram)[address]));
	}

	return (const dmg_video_tile_t *)&(((uint8_t *)ram)[address]);
}

static const dmg_video_tile_t *
dmg_video_tile_sprite(
	__in const void *ram,
	__in uint8_t id
	)
{
	return (const dmg_video_tile_t *)&(((uint8_t *)ram)[TILE_DATA[SPRITE_DATA] + (sizeof(dmg_video_tile_t) * id)]);
}

static void
dmg_video_scanline_background(
	__in dmg_video_t *video
	)
{
	uint32_t x = video->screen_x, y = (video->screen_y + video->line), py = (y % TILE_HEIGHT);
	const dmg_video_tile_t *tile = dmg_video_tile_background(video->ram.data, video->control.background_tile_map, video->control.tile_data,
						(x / TILE_WIDTH) % TILE_PITCH, (y / TILE_HEIGHT) % TILE_PITCH);

	for(; x < (video->screen_x + VIEWPORT_WIDTH); ++x) {
		uint8_t px;

		if(!(px = (x % TILE_WIDTH))) {
			tile = dmg_video_tile_background(video->ram.data, video->control.background_tile_map, video->control.tile_data,
					(x / TILE_WIDTH) % TILE_PITCH, (y / TILE_HEIGHT) % TILE_PITCH);
		}

		video->viewport[y - video->screen_y][x - video->screen_x]
			= dmg_video_palette_color(&video->background, dmg_video_pixel_color(tile, px, py));
	}
}

static void
dmg_video_scanline_sprite(
	__in dmg_video_t *video
	)
{
	uint32_t count = 0, index;
	dmg_video_sprite_screen_list_t list = {};

	for(index = 0; index < SPRITE_MAX; ++index) {
		const dmg_video_sprite_t *sprite = &((const dmg_video_sprite_list_t *)video->ram_sprite.data)->sprite[index];

		if((video->line >= (sprite->y - SPRITE_OFFSET_Y))
				&& (video->line < ((sprite->y - SPRITE_OFFSET_Y) + SPRITE_SIZE[video->control.sprite_size]))) {
			list.sprite[count].x = (sprite->x - SPRITE_OFFSET_X);
			list.sprite[count].y = (sprite->y - SPRITE_OFFSET_Y);
			list.sprite[count].entry = sprite;
			++count;
		}

		if(count == LINE_SPRITE_MAX) {
			break;
		}
	}

	if(count) {

		for(index = 0; index < count; ++index) {
			uint32_t x, x_begin, x_end, x_off = 0, y = video->line;
			dmg_video_sprite_screen_t *sprite = &list.sprite[count - index - 1];
			const dmg_video_tile_t *tile = dmg_video_tile_sprite(video->ram.data, sprite->entry->id);

			if((sprite->x >= -TILE_WIDTH) && (sprite->x < 0)) {
				x_begin = 0;
				x_end = (TILE_WIDTH + sprite->x);
				x_off = (TILE_WIDTH - x_end);
			} else {
				x_begin = sprite->x;
				x_end = TILE_WIDTH;
			}

			for(x = x_begin; x < (x_begin + x_end); ++x) {
				uint8_t color, px = ((x + x_off) - x_begin), py = (y - sprite->y);

				if(x > VIEWPORT_WIDTH) {
					break;
				}

				if(sprite->entry->flip_x) {
					px = ((TILE_WIDTH - 1) - px);
				}

				if(sprite->entry->flip_y) {
					py = ((SPRITE_SIZE[video->control.sprite_size] - 1) - (y - sprite->y));
				}

				if(!(color = dmg_video_pixel_color(tile, px, py))
						|| (sprite->entry->priority && video->viewport[y][x])) {
					continue;
				}

				video->viewport[y][x] = dmg_video_palette_color(sprite->entry->palette ? &video->object_1 : &video->object_0,
								color);
			}
		}
	}
}

static void
dmg_video_scanline_window(
	__in dmg_video_t *video
	)
{
	uint32_t x = 0, y = (video->line - video->window_y), py = (y % TILE_HEIGHT);
	const dmg_video_tile_t *tile = dmg_video_tile_background(video->ram.data, video->control.window_tile_map, video->control.tile_data,
						(x / TILE_WIDTH) % TILE_PITCH, (y / TILE_HEIGHT) % TILE_PITCH);

	for(; x < (VIEWPORT_WIDTH - (video->window_x - WINDOW_OFFSET_X)); ++x) {
		uint8_t px;

		if(!(px = (x % TILE_WIDTH))) {
			tile = dmg_video_tile_background(video->ram.data, video->control.window_tile_map, video->control.tile_data,
					(x / TILE_WIDTH) % TILE_PITCH, (y / TILE_HEIGHT) % TILE_PITCH);
		}

		if((y + video->window_y) < LINE_HBLANK_MAX) {
			video->viewport[y + video->window_y][x + (video->window_x - WINDOW_OFFSET_X)]
				= dmg_video_palette_color(&video->background, dmg_video_pixel_color(tile, px, py));
		}
	}
}

#endif /* UNITTEST */

static bool
dmg_video_hblank(
	__in dmg_video_t *video
	)
{
	bool result;

	if((result = (++video->line == LINE_HBLANK_MAX))) {
		video->status.mode = MODE_VBLANK;

		if(video->control.enable) {
			dmg_runtime_interrupt(INTERRUPT_VBLANK);
		}

		if(video->status.vblank) {
			dmg_runtime_interrupt(INTERRUPT_LCDC);
		}
	} else {
		video->status.mode = MODE_SEARCH;

		if(video->status.search) {
			dmg_runtime_interrupt(INTERRUPT_LCDC);
		}
	}

	return result;
}

static bool
dmg_video_search(
	__in dmg_video_t *video
	)
{
	video->status.mode = MODE_TRANSFER;

	return false;
}

static bool
dmg_video_transfer(
	__in dmg_video_t *video
	)
{
	video->status.mode = MODE_HBLANK;

	if(video->status.hblank) {
		dmg_runtime_interrupt(INTERRUPT_LCDC);
	}

#ifndef UNITTEST
	if(video->control.enable) {

		if(video->control.background) {
			dmg_video_scanline_background(video);

			if(video->control.window && (video->window_x >= WINDOW_OFFSET_X) && (video->window_x < VIEWPORT_WIDTH)
					&& (video->window_y < LINE_HBLANK_MAX) && (video->line >= video->window_y)) {
				dmg_video_scanline_window(video);
			}
		}

		if(video->control.sprite) {
			dmg_video_scanline_sprite(video);
		}
	}
#endif /* UNITTEST */

	return false;
}

static bool
dmg_video_vblank(
	__in dmg_video_t *video
	)
{

	if(++video->line > LINE_VBLANK_MAX) {
		video->line = 0;
		video->status.mode = MODE_SEARCH;

		if(video->status.search) {
			dmg_runtime_interrupt(INTERRUPT_LCDC);
		}

#ifndef UNITTEST

		for(uint8_t y = 0; y < VIEWPORT_HEIGHT; ++y) {

			for(uint8_t x = 0; x < VIEWPORT_WIDTH; ++x) {
				dmg_service_pixel(video->viewport[y][x], x, y);
			}
		}

		memset(video->viewport, 0, sizeof(uint8_t) * VIEWPORT_WIDTH * VIEWPORT_HEIGHT);
#endif /* UNITTEST */
	}

	return false;
}

static const dmg_video_mode_hdlr MODE_HANDLER[] = {
	dmg_video_hblank, /* MODE_HBLANK */
	dmg_video_vblank, /* MODE_VBLANK */
	dmg_video_search, /* MODE_SEARCH */
	dmg_video_transfer, /* MODE_TRANSFER */
	};

int
dmg_video_export(
	__in const dmg_video_t *video,
	__in FILE *file
	)
{
	int result = DMG_STATUS_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Video exporting");
	TRACE_VIDEO(LEVEL_VERBOSE, video);

	if((result = dmg_service_export_data(file, &video->background, sizeof(video->background))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &video->cycle, sizeof(video->cycle))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &video->control, sizeof(video->control))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &video->line, sizeof(video->line))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &video->line_coincidence, sizeof(video->line_coincidence))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &video->object_0, sizeof(video->object_0))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &video->object_1, sizeof(video->object_1))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &video->screen_x, sizeof(video->screen_x))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &video->screen_y, sizeof(video->screen_y))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &video->status, sizeof(video->status))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &video->window_x, sizeof(video->window_x))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &video->window_y, sizeof(video->window_y))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	for(uint32_t address = 0; address < video->ram.length; ++address) {

		if((result = dmg_service_export_data(file, &((uint8_t *)video->ram.data)[address], sizeof(uint8_t))) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	for(uint32_t address = 0; address < video->ram_sprite.length; ++address) {

		if((result = dmg_service_export_data(file, &((uint8_t *)video->ram_sprite.data)[address], sizeof(uint8_t))) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	for(uint32_t y = 0; y < VIEWPORT_HEIGHT; ++y) {

		for(uint32_t x = 0; x < VIEWPORT_WIDTH; ++x) {

			if((result = dmg_service_export_data(file, &video->viewport[y][x], sizeof(video->viewport[y][x]))) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}
	}

	TRACE(LEVEL_INFORMATION, "Video exported");

exit:
	return result;
}

int
dmg_video_import(
	__inout dmg_video_t *video,
	__in FILE *file
	)
{
	int result = DMG_STATUS_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Video importing");

	if((result = dmg_service_import_data(file, &video->background, sizeof(video->background))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &video->cycle, sizeof(video->cycle))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &video->control, sizeof(video->control))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &video->line, sizeof(video->line))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &video->line_coincidence, sizeof(video->line_coincidence))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &video->object_0, sizeof(video->object_0))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &video->object_1, sizeof(video->object_1))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &video->screen_x, sizeof(video->screen_x))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &video->screen_y, sizeof(video->screen_y))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &video->status, sizeof(video->status))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &video->window_x, sizeof(video->window_x))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &video->window_y, sizeof(video->window_y))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	for(uint32_t address = 0; address < video->ram.length; ++address) {

		if((result = dmg_service_import_data(file, &((uint8_t *)video->ram.data)[address], sizeof(uint8_t))) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	for(uint32_t address = 0; address < video->ram_sprite.length; ++address) {

		if((result = dmg_service_import_data(file, &((uint8_t *)video->ram_sprite.data)[address], sizeof(uint8_t))) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	for(uint32_t y = 0; y < VIEWPORT_HEIGHT; ++y) {

		for(uint32_t x = 0; x < VIEWPORT_WIDTH; ++x) {

			if((result = dmg_service_import_data(file, &video->viewport[y][x], sizeof(video->viewport[y][x]))) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}
	}

	TRACE_VIDEO(LEVEL_VERBOSE, video);
	TRACE(LEVEL_INFORMATION, "Video imported");

exit:
	return result;
}

int
dmg_video_load(
	__inout dmg_video_t *video,
	__in const dmg_t *configuration
	)
{
	int result = DMG_STATUS_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Video loading");

	if(!configuration->bootrom.data) {
		video->background.raw = POST_BACKGROUND_PALETTE;
		video->control.raw = POST_CONTROL;
		video->line_coincidence = POST_LINE_COINCIDENCE;
		video->object_0.raw = POST_OBJECT_PALETTE_0;
		video->object_1.raw = POST_OBJECT_PALETTE_1;
		video->screen_x = POST_SCREEN_X;
		video->screen_y = POST_SCREEN_Y;
		video->window_x = POST_WINDOW_X;
		video->window_y = POST_WINDOW_Y;
	}

	video->status.mode = MODE_SEARCH;

	if((result = dmg_buffer_allocate(&video->ram, RAM_WIDTH, 0)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_buffer_allocate(&video->ram_sprite, RAM_SPRITE_WIDTH, 0)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE_VIDEO(LEVEL_VERBOSE, video);
	TRACE(LEVEL_INFORMATION, "Video loaded");

exit:
	return result;
}

uint8_t
dmg_video_read(
	__in const dmg_video_t *video,
	__in uint16_t address
	)
{
	bool read = true;
	uint8_t result = 0;

	switch(address) {
		case ADDRESS_VIDEO_BACKGROUND_PALETTE:
			result = video->background.raw;
			break;
		case ADDRESS_VIDEO_CONTROL:
			result = video->control.raw;
			break;
		case ADDRESS_VIDEO_LINE:
			result = video->line;
			break;
		case ADDRESS_VIDEO_LINE_COINCIDENCE:
			result = video->line_coincidence;
			break;
		case ADDRESS_VIDEO_OBJECT_PALETTE_0:
			result = video->object_0.raw;
			break;
		case ADDRESS_VIDEO_OBJECT_PALETTE_1:
			result = video->object_1.raw;
			break;
		case ADDRESS_VIDEO_RAM_BANK:
			result = UINT8_MAX;
			TRACE_FORMAT(LEVEL_VERBOSE, "CGB VRAM bank read [%04x]->%02x", address, result);
			break;
		case ADDRESS_VIDEO_RAM_BEGIN ... ADDRESS_VIDEO_RAM_END:

			if(video->control.enable) {

				switch(video->status.mode) {
					case MODE_HBLANK:
					case MODE_SEARCH:
					case MODE_VBLANK:
						break;
					default:
						read = false;
						break;
				}
			}

			result = (read ? ((uint8_t *)video->ram.data)[address - ADDRESS_VIDEO_RAM_BEGIN] : UINT8_MAX);
			break;
		case ADDRESS_VIDEO_RAM_SPRITE_BEGIN ... ADDRESS_VIDEO_RAM_SPRITE_END:

			if(video->control.enable) {

				switch(video->status.mode) {
					case MODE_HBLANK:
					case MODE_VBLANK:
						break;
					default:
						read = false;
						break;
				}
			}

			result = (read ? ((uint8_t *)video->ram_sprite.data)[address - ADDRESS_VIDEO_RAM_SPRITE_BEGIN] : UINT8_MAX);
			break;
		case ADDRESS_VIDEO_SCREEN_X:
			result = video->screen_x;
			break;
		case ADDRESS_VIDEO_SCREEN_Y:
			result = video->screen_y;
			break;
		case ADDRESS_VIDEO_STATUS:
			result = video->status.raw;
			break;
		case ADDRESS_VIDEO_WINDOW_X:
			result = video->window_x;
			break;
		case ADDRESS_VIDEO_WINDOW_Y:
			result = video->window_y;
			break;
		default:
			result = UINT8_MAX;
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported video read [%04x]->%02x", address, result);
			break;
	}

	return result;
}

bool
dmg_video_step(
	__inout dmg_video_t *video,
	__in uint32_t cycle
	)
{
	bool result = false;

	if(video->transfer.enable) {

		for(uint32_t tick = 0; tick < cycle; tick += CYCLE) {

			if(!(video->transfer.enable = (video->transfer.destination <= ADDRESS_VIDEO_RAM_SPRITE_END))) {
				break;
			}

			dmg_runtime_write(video->transfer.destination++, dmg_runtime_read(video->transfer.source++));
			TRACE_VIDEO_TRANSFER(LEVEL_VERBOSE, video);
		}
	}

	for(uint32_t tick = 0; tick < cycle; tick += CYCLE) {

		if((video->cycle += CYCLE) >= MODE_CYC[video->status.mode]) {
			video->cycle %= MODE_CYC[video->status.mode];
			result = MODE_HANDLER[video->status.mode](video);

			if((video->status.coincidence = (video->line == video->line_coincidence)) && video->status.line_coincidence) {
				dmg_runtime_interrupt(INTERRUPT_LCDC);
			}
		}
	}

	return result;
}

void
dmg_video_unload(
	__inout dmg_video_t *video
	)
{
	TRACE(LEVEL_INFORMATION, "Video unloading");
	dmg_buffer_free(&video->ram_sprite);
	dmg_buffer_free(&video->ram);
	memset(video, 0, sizeof(*video));
	TRACE(LEVEL_INFORMATION, "Video unloaded");
}

void
dmg_video_write(
	__inout dmg_video_t *video,
	__in uint16_t address,
	__in uint8_t value
	)
{
	bool write = true;

	switch(address) {
		case ADDRESS_VIDEO_BACKGROUND_PALETTE:
			video->background.raw = value;
			break;
		case ADDRESS_VIDEO_CONTROL:
			video->control.raw = value;

			if(!video->control.enable) {
				video->line = 0;
			}
			break;
		case ADDRESS_VIDEO_LINE_COINCIDENCE:
			video->line_coincidence = value;
			break;
		case ADDRESS_VIDEO_OBJECT_PALETTE_0:
			video->object_0.raw = value;
			break;
		case ADDRESS_VIDEO_OBJECT_PALETTE_1:
			video->object_1.raw = value;
			break;
		case ADDRESS_VIDEO_RAM_BANK:
			TRACE_FORMAT(LEVEL_VERBOSE, "CGB VRAM bank write [%04x]<-%02x", address, value);
			break;
		case ADDRESS_VIDEO_RAM_BEGIN ... ADDRESS_VIDEO_RAM_END:

			if(video->control.enable) {

				switch(video->status.mode) {
					case MODE_HBLANK:
					case MODE_SEARCH:
					case MODE_VBLANK:
						break;
					default:
						write = false;
						break;
				}
			}

			if(write) {
				((uint8_t *)video->ram.data)[address - ADDRESS_VIDEO_RAM_BEGIN] = value;
			}
			break;
		case ADDRESS_VIDEO_RAM_SPRITE_BEGIN ... ADDRESS_VIDEO_RAM_SPRITE_END:

			if(video->control.enable) {

				switch(video->status.mode) {
					case MODE_HBLANK:
					case MODE_VBLANK:
						break;
					default:
						write = false;
						break;
				}
			}

			if(write) {
				((uint8_t *)video->ram_sprite.data)[address - ADDRESS_VIDEO_RAM_SPRITE_BEGIN] = value;
			}
			break;
		case ADDRESS_VIDEO_SCREEN_X:
			video->screen_x = value;
			break;
		case ADDRESS_VIDEO_SCREEN_Y:
			video->screen_y = value;
			break;
		case ADDRESS_VIDEO_STATUS:
			video->status.raw = value;
			break;
		case ADDRESS_VIDEO_TRANSFER:
			video->transfer.enable = true;
			video->transfer.destination = ADDRESS_VIDEO_RAM_SPRITE_BEGIN;
			video->transfer.source = (TRANSFER_SCALE * value);
			TRACE_VIDEO_TRANSFER(LEVEL_VERBOSE, video);
			break;
		case ADDRESS_VIDEO_WINDOW_X:
			video->window_x = value;
			break;
		case ADDRESS_VIDEO_WINDOW_Y:
			video->window_y = value;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported video write [%04x]<-%02x", address, value);
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
