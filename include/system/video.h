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

#ifndef DMG_SYSTEM_VIDEO_H_
#define DMG_SYSTEM_VIDEO_H_

#include "../common/buffer.h"

typedef union {

	struct {
		uint8_t background : 1;
		uint8_t sprite : 1;
		uint8_t sprite_size : 1;
		uint8_t background_tile_map : 1;
		uint8_t tile_data : 1;
		uint8_t window : 1;
		uint8_t window_tile_map : 1;
		uint8_t enable : 1;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_video_control_t;

typedef union {

	struct {
		uint8_t white : 2;
		uint8_t grey_light : 2;
		uint8_t grey_dark : 2;
		uint8_t black : 2;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_video_palette_t;

typedef union {

	struct {
		uint8_t mode : 2;
		uint8_t coincidence : 1;
		uint8_t hblank : 1;
		uint8_t vblank : 1;
		uint8_t search : 1;
		uint8_t line_coincidence : 1;
		uint8_t unused : 1;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_video_status_t;

typedef struct {
	bool enable;
	uint16_t destination;
	uint16_t source;
} __attribute__((packed)) dmg_video_transfer_t;

typedef struct {
	dmg_video_palette_t background;
	uint32_t cycle;
	dmg_video_control_t control;
	uint8_t line;
	uint8_t line_coincidence;
	dmg_video_palette_t object_0;
	dmg_video_palette_t object_1;
	dmg_buffer_t ram;
	dmg_buffer_t ram_sprite;
	uint8_t screen_x;
	uint8_t screen_y;
	dmg_video_status_t status;
	dmg_video_transfer_t transfer;
	uint8_t viewport[VIEWPORT_HEIGHT][VIEWPORT_WIDTH];
	uint8_t window_x;
	uint8_t window_y;
} __attribute__((packed)) dmg_video_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_video_export(
	__in const dmg_video_t *video,
	__in FILE *file
	);

int dmg_video_import(
	__inout dmg_video_t *video,
	__in FILE *file
	);

int dmg_video_load(
	__inout dmg_video_t *video,
	__in const dmg_t *configuration
	);

uint8_t dmg_video_read(
	__in const dmg_video_t *video,
	__in uint16_t address
	);

bool dmg_video_step(
	__inout dmg_video_t *video,
	__in uint32_t cycle
	);

void dmg_video_unload(
	__inout dmg_video_t *video
	);

void dmg_video_write(
	__inout dmg_video_t *video,
	__in uint16_t address,
	__in uint8_t value
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_SYSTEM_VIDEO_H_ */
