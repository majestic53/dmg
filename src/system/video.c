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
	// TODO
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

	// TODO

	TRACE_VIDEO(LEVEL_VERBOSE, video);
	TRACE(LEVEL_INFORMATION, "Video loaded");

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
	// TODO
	return true;
	// ---
}

void
dmg_video_unload(
	__inout dmg_video_t *video
	)
{
	TRACE(LEVEL_INFORMATION, "Video unloading");

	// TODO

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
