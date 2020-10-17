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

#ifndef DMG_SERVICE_SDL_TYPE_H_
#define DMG_SERVICE_SDL_TYPE_H_

#include <SDL2/SDL.h>
#include "../../include/service.h"

#define FRAME_PER_SEC 60
#define FRAME_RATE (MS_PER_SEC / (float)FRAME_PER_SEC)

#define MS_PER_SEC 1000

#define PALETTE_MASK_BLUE 0x000000ff
#define PALETTE_MASK_GREEN 0x0000ff00
#define PALETTE_MASK_RED 0x00ff0000
#define PALETTE_SHIFT_BLUE 0
#define PALETTE_SHIFT_GREEN 8
#define PALETTE_SHIFT_RED 16

#define SCALE_MAX 4
#define SCALE_MIN 1

#define TILE_HEIGHT 8
#define TILE_WIDTH 8

#define TITLE "DMG"
#define TITLE_LENGTH_MAX 64
#define TITLE_UNTITLED "Untitled"

#define VIEWPORT_HEIGHT 144
#define VIEWPORT_WIDTH 160

#define WINDOW_HEIGHT 256
#define WINDOW_WIDTH 256

typedef union {

	struct {
		uint8_t blue;
		uint8_t green;
		uint8_t red;
		uint8_t alpha;
	};

	uint32_t raw;
} dmg_bgra_t;

typedef struct {
	bool redraw;
	uint32_t scale;
	uint8_t viewport_x;
	uint8_t viewport_y;
	bool window_enable;
	uint8_t window_x;
	uint8_t window_y;
	char title[TITLE_LENGTH_MAX];
	dmg_bgra_t palette[DMG_PALETTE_MAX];
	dmg_bgra_t pixel[WINDOW_HEIGHT][WINDOW_WIDTH];
	dmg_bgra_t pixel_viewport[WINDOW_HEIGHT][WINDOW_WIDTH];
	dmg_bgra_t pixel_window[WINDOW_HEIGHT][WINDOW_WIDTH];
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	SDL_Texture *texture_viewport;
	SDL_Texture *texture_window;
	SDL_Window *window;
} dmg_display_t;

typedef struct {
	uint32_t begin;
	uint32_t count;
	uint32_t end;
	float frequency;
	float rate;
} dmg_frame_t;

typedef struct {
	uint32_t button[DMG_BUTTON_MAX];
	uint32_t direction[DMG_DIRECTION_MAX];
} dmg_input_t;

typedef struct {
	dmg_frame_t frame;
	dmg_input_t input;
	dmg_display_t display;
} dmg_sdl_t;

static const dmg_bgra_t COLOR_BACKGROUND = {{ 0x00, 0x00, 0x00, 0x00 }};
static const dmg_bgra_t COLOR_VIEWPORT = {{ 0x00, 0x00, 0xff, 0xff }};
static const dmg_bgra_t COLOR_WINDOW = {{ 0x00, 0xff, 0xff, 0xff }};

#endif /* DMG_SERVICE_SDL_TYPE_H_ */
