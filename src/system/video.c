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

#endif /* NDEBUG */

/*static bool
dmg_video_hblank(
	__in dmg_video_t *video
	)
{
	// TODO
	return false;
	// ---
}

static bool
dmg_video_search(
	__in dmg_video_t *video
	)
{
	// TODO
	return false;
	// ---
}

static bool
dmg_video_transfer(
	__in dmg_video_t *video
	)
{
	// TODO
	return false;
	// ---
}

static bool
dmg_video_vblank(
	__in dmg_video_t *video
	)
{
	// TODO
	return false;
	// ---
}*/

//static const dmg_mode_cb MODE_HANDLER[] = {
	//dmg_video_hblank, /* MODE_HBLANK */
	//dmg_video_vblank, /* MODE_VBLANK */
	//dmg_video_search, /* MODE_SEARCH */
	//dmg_video_transfer, /* MODE_TRANSFER */
	//};

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
	uint8_t result = 0;

	switch(address) {
		case ADDRESS_VIDEO_BGP:

			// TODO

			break;
		case ADDRESS_VIDEO_LCDC:

			// TODO

			break;
		case ADDRESS_VIDEO_LY:

			// TODO

			break;
		case ADDRESS_VIDEO_LYC:

			// TODO

			break;
		case ADDRESS_VIDEO_OBP0:

			// TODO

			break;
		case ADDRESS_VIDEO_OBP1:

			// TODO

			break;
		case ADDRESS_VIDEO_RAM_BEGIN ... ADDRESS_VIDEO_RAM_END:

			// TODO

			break;
		case ADDRESS_VIDEO_RAM_SPRITE_BEGIN ... ADDRESS_VIDEO_RAM_SPRITE_END:

			// TODO

			break;
		case ADDRESS_VIDEO_SCX:

			// TODO

			break;
		case ADDRESS_VIDEO_SCY:

			// TODO

			break;
		case ADDRESS_VIDEO_STAT:

			// TODO

			break;
		case ADDRESS_VIDEO_WX:

			// TODO

			break;
		case ADDRESS_VIDEO_WY:

			// TODO

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
	bool result;

	// TODO
	if((result = (video->cycle > CYCLE_PER_FRAME))) {
		video->cycle %= CYCLE_PER_FRAME;
	} else {
		video->cycle += cycle;
	}
	// ---

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

	switch(address) {
		case ADDRESS_VIDEO_BGP:

			// TODO

			break;
		case ADDRESS_VIDEO_DMA:

			// TODO

			break;
		case ADDRESS_VIDEO_LCDC:

			// TODO

			break;
		case ADDRESS_VIDEO_LYC:

			// TODO

			break;
		case ADDRESS_VIDEO_OBP0:

			// TODO

			break;
		case ADDRESS_VIDEO_OBP1:

			// TODO

			break;
		case ADDRESS_VIDEO_RAM_BEGIN ... ADDRESS_VIDEO_RAM_END:

			// TODO

			break;
		case ADDRESS_VIDEO_RAM_SPRITE_BEGIN ... ADDRESS_VIDEO_RAM_SPRITE_END:

			// TODO

			break;
		case ADDRESS_VIDEO_SCX:

			// TODO

			break;
		case ADDRESS_VIDEO_SCY:

			// TODO

			break;
		case ADDRESS_VIDEO_STAT:

			// TODO

			break;
		case ADDRESS_VIDEO_WX:

			// TODO

			break;
		case ADDRESS_VIDEO_WY:

			// TODO

			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported video write [%04x]<-%02x", address, value);
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
