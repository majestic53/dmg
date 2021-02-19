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

#ifndef DMG_SERVICE_SDL_TYPE_H_
#define DMG_SERVICE_SDL_TYPE_H_

#ifdef SDL

#include <SDL2/SDL.h>
#include "../../include/service/sdl.h"

#define AUDIO_CHANNELS 1
#define AUDIO_DIRECTION 0
#define AUDIO_FREQUENCY 15360
#define AUDIO_FORMAT AUDIO_U8
#define AUDIO_PAUSE 1
#define AUDIO_PLAY 0
#define AUDIO_SAMPLES 256

#define FRAMES_PER_SEC 60

#define KEY_FULLSCREEN SDL_SCANCODE_F11

#define MS_PER_SEC 1000

#define PALETTE_CHANNEL_MASK 0xff
#define PALETTE_MASK_BLUE (PALETTE_CHANNEL_MASK << PALETTE_SHIFT_BLUE)
#define PALETTE_MASK_GREEN (PALETTE_CHANNEL_MASK << PALETTE_SHIFT_GREEN)
#define PALETTE_MASK_RED (PALETTE_CHANNEL_MASK << PALETTE_SHIFT_RED)
#define PALETTE_SHIFT_BLUE 0
#define PALETTE_SHIFT_GREEN 8
#define PALETTE_SHIFT_RED 16

#define SCALE_MAX 4
#define SCALE_MIN 1

#define TITLE "DMG"
#define TITLE_LENGTH_MAX 64
#define TITLE_UNTITLED "Untitled"

#define WINDOW_HEIGHT 144
#define WINDOW_WIDTH 160

typedef union {

	struct {
		uint8_t blue;
		uint8_t green;
		uint8_t red;
		uint8_t alpha;
	};

	uint32_t raw;
} dmg_sdl_bgra_t;

typedef struct {
	SDL_AudioDeviceID device;
	SDL_AudioSpec specification;
} dmg_sdl_audio_t;

typedef struct {
	dmg_sdl_bgra_t palette[DMG_PALETTE_MAX];
	dmg_sdl_bgra_t pixel[WINDOW_HEIGHT][WINDOW_WIDTH];
	bool fullscreen;
	bool redraw;
	SDL_Renderer *renderer;
	uint32_t scale;
	SDL_Texture *texture;
	char title[TITLE_LENGTH_MAX];
	SDL_Window *window;
	uint64_t frame_begin;
	float frame_elapsed;
#ifndef NDEBUG
	uint32_t frame_count;
	float frame_rate;
#endif /* NDEBUG */
} dmg_sdl_video_t;

typedef struct {
	dmg_sdl_audio_t audio;
	dmg_sdl_video_t video;
} dmg_sdl_t;

static const dmg_sdl_bgra_t COLOR_BACKGROUND = {{ 0x00, 0x00, 0x00, 0x00 }};

#endif /* SDL */

#endif /* DMG_SERVICE_SDL_TYPE_H_ */
