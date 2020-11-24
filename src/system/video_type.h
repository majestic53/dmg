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

#ifndef DMG_SYSTEM_VIDEO_TYPE_H_
#define DMG_SYSTEM_VIDEO_TYPE_H_

#include "../../include/system/video.h"
#include "../../include/runtime.h"
#include "../../include/service.h"

#define LINE_HBLANK_MAX 144
#define LINE_SPRITE_MAX 10
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

#define SPRITE_DATA 1
#define SPRITE_MAP 0
#define SPRITE_MAX 40
#define SPRITE_OFFSET_X 8
#define SPRITE_OFFSET_Y 16

#define TILE_HEIGHT 8
#define TILE_PITCH 32
#define TILE_WIDTH 8

#define TRANSFER_SCALE 0x0100

#define WINDOW_OFFSET_X 7

enum {
	MODE_HBLANK = 0,
	MODE_VBLANK,
	MODE_SEARCH,
	MODE_TRANSFER,
	MODE_MAX,
};

enum {
	SPRITE_SIZE_8 = 0,
	SPRITE_SIZE_16,
	SPRITE_SIZE_MAX,
};

static const uint32_t MODE_CYC[] = {
	204, /* MODE_HBLANK */
	456, /* MODE_VBLANK */
	80, /* MODE_SEARCH */
	172, /* MODE_TRANSFER */
	};

static const uint8_t SPRITE_SIZE[] = {
	8, /* SPRITE_SIZE_8 */
	16,  /* SPRITE_SIZE_16 */
	};

static const uint16_t TILE_DATA[] = {
	0x0800, /* 0x8800 - 0x97ff */
	0x0000, /* 0x8000 - 0x8fff */
	};

static const uint16_t TILE_MAP[] = {
	0x1800, /* 0x9800 - 0x9bff */
	0x1c00, /* 0x9c00 - 0x9fff */
	};

typedef union {

	struct {
		uint8_t y;
		uint8_t x;
		uint8_t id;

		union {

			struct {
				uint8_t unused : 4;
				uint8_t palette : 1;
				uint8_t flip_x : 1;
				uint8_t flip_y : 1;
				uint8_t priority : 1;
			};

			uint8_t flag;
		};
	};

	uint32_t raw;
} __attribute__((packed)) dmg_video_sprite_t;

typedef struct {
	dmg_video_sprite_t sprite[SPRITE_MAX];
} __attribute__((packed)) dmg_video_sprite_list_t;

typedef struct {
	int32_t x;
	int32_t y;
	const dmg_video_sprite_t *entry;
} __attribute__((packed)) dmg_video_sprite_screen_t;

typedef struct {
	dmg_video_sprite_screen_t sprite[SPRITE_MAX];
} __attribute__((packed)) dmg_video_sprite_screen_list_t;

typedef union {

	struct {
		uint8_t low;
		uint8_t high;
	};

	uint16_t raw;
} __attribute__((packed)) dmg_video_tile_line_t;

typedef struct {
	dmg_video_tile_line_t line[TILE_HEIGHT];
} __attribute__((packed)) dmg_video_tile_t;

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
