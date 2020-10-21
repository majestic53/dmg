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

#include "./video_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NDEBUG

static void
dmg_video_trace(
	__in int level,
	__inout dmg_video_t *video
	)
{
	TRACE_FORMAT(level, "Video LCDC=%02x [Enable=%x (%c%c%c)]", video->lcdc.raw, video->lcdc.enable,
		video->lcdc.background ? 'B' : '-', video->lcdc.sprite ? 'S' : '-', video->lcdc.window ? 'W' : '-');
	TRACE_FORMAT(level, "Video STAT=%02x [Mode=%02x, Coin=%x, (%c%c%c%c)]", video->stat.raw, video->stat.mode,
		video->stat.coincidence, video->stat.hblank ? 'H' : '-', video->stat.vblank ? 'V' : '-',
		video->stat.search ? 'S' : '-', video->stat.lyc ? 'C' : '-');
	TRACE_FORMAT(level, "Video BGP=%02x (%02x, %02x, %02x, %02x)", video->bgp.raw, video->bgp.white, video->bgp.grey_light,
		video->bgp.grey_dark, video->bgp.black);
	TRACE_FORMAT(level, "Video OBP0=%02x (%02x, %02x, %02x, %02x)", video->obp0.raw, video->obp0.white, video->obp0.grey_light,
		video->obp0.grey_dark, video->obp0.black);
	TRACE_FORMAT(level, "Video OBP1=%02x (%02x, %02x, %02x, %02x)", video->obp1.raw, video->obp1.white, video->obp1.grey_light,
		video->obp1.grey_dark, video->obp1.black);
	TRACE_FORMAT(level, "Video LY/LYC=%02x, %02x", video->ly, video->lyc);
	TRACE_FORMAT(level, "Video SCX/SCY=%02x, %02x", video->scx, video->scy);
	TRACE_FORMAT(level, "Video WX/WY=%02x, %02x", video->wx, video->wy);
	TRACE_FORMAT(level, "Video Ram[%04x]=%p", video->ram.length, video->ram.data);
	TRACE_FORMAT(level, "Video Ram-Sprite[%04x]=%p", video->ram_sprite.length, video->ram_sprite.data);
}

static void
dmg_video_dma_trace(
	__in int level,
	__inout dmg_video_t *video
	)
{
	TRACE_FORMAT(level, "DMA enable=%x (remaining=%u)", video->dma.enable,
		(ADDRESS_VIDEO_RAM_SPRITE_END + 1) - video->dma.destination);
	TRACE_FORMAT(level, "DMA destination=%04x", video->dma.destination);
	TRACE_FORMAT(level, "DMA source=%04x", video->dma.source);
}

#endif /* NDEBUG */

static void
dmg_video_dma_start(
	__in dmg_video_t *video,
	__in uint8_t value
	)
{
	video->dma.enable = true;
	video->dma.destination = ADDRESS_VIDEO_RAM_SPRITE_BEGIN;
	video->dma.source = (DMA_SCALE * value);
	TRACE_VIDEO_DMA(LEVEL_VERBOSE, video);
}

static void
dmg_video_dma_step(
	__in dmg_video_t *video,
	__in uint32_t cycle
	)
{

	if(video->dma.enable) {

		for(uint32_t index = 0; index < (cycle / CYCLE); ++index) {

			if(!(video->dma.enable = (video->dma.destination <= ADDRESS_VIDEO_RAM_SPRITE_END))) {
				break;
			}

			dmg_runtime_write(video->dma.destination++, dmg_runtime_read(video->dma.source++));
		}

		TRACE_VIDEO_DMA(LEVEL_VERBOSE, video);
	}
}

static bool
dmg_video_hblank(
	__in dmg_video_t *video
	)
{
	bool result;

	if((result = (++video->ly == LINE_HBLANK_MAX))) {
		video->stat.mode = MODE_VBLANK;

		if(video->lcdc.enable) {
			dmg_runtime_interrupt(INTERRUPT_VBLANK);
		}

		if(video->stat.vblank) {
			dmg_runtime_interrupt(INTERRUPT_LCDC);
		}
	} else {
		video->stat.mode = MODE_SEARCH;

		if(video->stat.search) {
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
	video->stat.mode = MODE_TRANSFER;

	return false;
}

static bool
dmg_video_transfer(
	__in dmg_video_t *video
	)
{
	video->stat.mode = MODE_HBLANK;

	if(video->stat.hblank) {
		dmg_runtime_interrupt(INTERRUPT_LCDC);
	}

	if(video->lcdc.enable) {

		if(video->lcdc.background || video->lcdc.window) {

			// TODO: RENDER BACKGROUND/WINDOW SCANLINE

		}

		if(video->lcdc.sprite) {

			// TOOD: RENDER SPRITE SCANLINE

		}
	}

	return false;
}

static bool
dmg_video_vblank(
	__in dmg_video_t *video
	)
{

	if(++video->ly > LINE_VBLANK_MAX) {
		video->ly = 0;
		video->stat.mode = MODE_SEARCH;

		if(video->stat.search) {
			dmg_runtime_interrupt(INTERRUPT_LCDC);
		}
	}

	return false;
}

static const dmg_mode_cb MODE_HANDLER[] = {
	dmg_video_hblank, /* MODE_HBLANK */
	dmg_video_vblank, /* MODE_VBLANK */
	dmg_video_search, /* MODE_SEARCH */
	dmg_video_transfer, /* MODE_TRANSFER */
	};

int
dmg_video_load(
	__inout dmg_video_t *video,
	__in const dmg_t *configuration
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Video loading");

	if(!configuration->bootrom.data) {
		video->bgp.raw = POST_BGP;
		video->lcdc.raw = POST_LCDC;
		video->lyc = POST_LYC;
		video->obp0.raw = POST_OBP0;
		video->obp1.raw = POST_OBP1;
		video->scx = POST_SCX;
		video->scy = POST_SCY;
		video->wx = POST_WX;
		video->wy = POST_WY;
	}

	video->stat.mode = MODE_SEARCH;

	if((result = dmg_buffer_allocate(&video->ram, RAM_WIDTH, 0)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_buffer_allocate(&video->ram_sprite, RAM_SPRITE_WIDTH, 0)) != ERROR_SUCCESS) {
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
		case ADDRESS_VIDEO_BGP:
			result = video->bgp.raw;
			break;
		case ADDRESS_VIDEO_LCDC:
			result = video->lcdc.raw;
			break;
		case ADDRESS_VIDEO_LY:
			result = video->ly;
			break;
		case ADDRESS_VIDEO_LYC:
			result = video->lyc;
			break;
		case ADDRESS_VIDEO_OBP0:
			result = video->obp0.raw;
			break;
		case ADDRESS_VIDEO_OBP1:
			result = video->obp1.raw;
			break;
		case ADDRESS_VIDEO_RAM_BEGIN ... ADDRESS_VIDEO_RAM_END:

			if(video->lcdc.enable) {

				switch(video->stat.mode) {
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

			if(video->lcdc.enable) {

				switch(video->stat.mode) {
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
		case ADDRESS_VIDEO_SCX:
			result = video->scx;
			break;
		case ADDRESS_VIDEO_SCY:
			result = video->scy;
			break;
		case ADDRESS_VIDEO_STAT:
			result = video->stat.raw;
			break;
		case ADDRESS_VIDEO_WX:
			result = video->wx;
			break;
		case ADDRESS_VIDEO_WY:
			result = video->wy;
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

	dmg_video_dma_step(video, cycle);

	for(uint32_t tick = 0; tick < cycle; tick += CYCLE) {

		if((video->cycle += CYCLE) >= MODE_CYC[video->stat.mode]) {
			video->cycle %= MODE_CYC[video->stat.mode];
			result = MODE_HANDLER[video->stat.mode](video);

			if((video->stat.coincidence = (video->ly == video->lyc)) && video->stat.lyc) {
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
		case ADDRESS_VIDEO_BGP:
			video->bgp.raw = value;
			break;
		case ADDRESS_VIDEO_DMA:
			dmg_video_dma_start(video, value);
			break;
		case ADDRESS_VIDEO_LCDC:
			video->lcdc.raw = value;
			dmg_service_window(video->lcdc.window, video->wx, video->wy);

			if(!video->lcdc.enable) {
				video->ly = 0;
			}
			break;
		case ADDRESS_VIDEO_LYC:
			video->lyc = value;
			break;
		case ADDRESS_VIDEO_OBP0:
			video->obp0.raw = value;
			break;
		case ADDRESS_VIDEO_OBP1:
			video->obp1.raw = value;
			break;
		case ADDRESS_VIDEO_RAM_BEGIN ... ADDRESS_VIDEO_RAM_END:

			if(video->lcdc.enable) {

				switch(video->stat.mode) {
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

			if(video->lcdc.enable) {

				switch(video->stat.mode) {
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
		case ADDRESS_VIDEO_SCX:
			video->scx = value;
			dmg_service_viewport(video->scx, video->scy);
			break;
		case ADDRESS_VIDEO_SCY:
			video->scy = value;
			dmg_service_viewport(video->scx, video->scy);
			break;
		case ADDRESS_VIDEO_STAT:
			video->stat.raw = value;
			break;
		case ADDRESS_VIDEO_WX:
			video->wx = value;
			dmg_service_window(video->lcdc.window, video->wx, video->wy);
			break;
		case ADDRESS_VIDEO_WY:
			video->wy = value;
			dmg_service_window(video->lcdc.window, video->wx, video->wy);
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported video write [%04x]<-%02x", address, value);
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
