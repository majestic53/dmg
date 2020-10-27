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

#ifndef DMG_SYSTEM_VIDEO_TYPE_H_
#define DMG_SYSTEM_VIDEO_TYPE_H_

#include "../../include/system/video.h"
#include "../../include/runtime.h"
#include "../../include/service.h"

#define LINE_HBLANK_MAX 144
#define LINE_VBLANK_MAX 153

#define POST_BACKGROUND_PALETTE 0xfc
#define POST_CONTROL 0x91
#define POST_LINE_COINCIDENCE 0x00
#define POST_OBJECT_PALETTE_0 0xff
#define POST_OBJECT_PALETTE_1 0xff
#define POST_SCREEN_X 0x00
#define POST_SCREEN_Y 0x00
#define POST_WINDOW_X 0x00
#define POST_WINDOW_Y 0x00

#define RAM_WIDTH ADDRESS_WIDTH(ADDRESS_VIDEO_RAM_BEGIN, ADDRESS_VIDEO_RAM_END)
#define RAM_SPRITE_WIDTH ADDRESS_WIDTH(ADDRESS_VIDEO_RAM_SPRITE_BEGIN, ADDRESS_VIDEO_RAM_SPRITE_END)

#define TRANSFER_SCALE 0x0100

enum {
	MODE_HBLANK = 0,
	MODE_VBLANK,
	MODE_SEARCH,
	MODE_TRANSFER,
	MODE_MAX,
};

static const uint32_t MODE_CYC[] = {
	204, /* MODE_HBLANK */
	456, /* MODE_VBLANK */
	80, /* MODE_SEARCH */
	172, /* MODE_TRANSFER */
	};

typedef bool (*dmg_mode)(
	__in dmg_video_t *video
	);

#ifndef NDEBUG
#define TRACE_VIDEO(_LEVEL_, _VIDEO_) \
	if((_LEVEL_) <= (LEVEL)) { \
		dmg_video_trace(_LEVEL_, _VIDEO_); \
	}
#define TRACE_VIDEO_TRANSFER(_LEVEL_, _VIDEO_) \
	if((_LEVEL_) <= (LEVEL)) { \
		dmg_video_trace_transfer(_LEVEL_, _VIDEO_); \
	}
#else
#define TRACE_VIDEO(_LEVEL_, _VIDEO_)
#define TRACE_VIDEO_TRANSFER(_LEVEL_, _VIDEO_)
#endif /* NDEBUG */

#endif /* DMG_SYSTEM_VIDEO_TYPE_H_ */
