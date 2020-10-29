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

#ifndef DMG_TOOL_LAUNCHER_TYPE_H_
#define DMG_TOOL_LAUNCHER_TYPE_H_

#include <SDL2/SDL.h>
#include <getopt.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/dmg.h"

#ifndef __in
#define __in
#endif /* __in */
#ifndef __inout
#define __inout
#endif /* __inout */
#ifndef __out
#define __out
#endif /* __out */

#define DATA_MASK 1
#define DATA_SHIFT 1

#define DMG "DMG"
#define DMG_NOTICE "Copyright (C) 2020 David Jolly"
#define DMG_USAGE "dmg [args]"

#define OPTION_BOOTROM 'b'
#define OPTION_CAPTURE 'c'
#define OPTION_HELP 'h'
#define OPTION_ROM 'r'
#define OPTION_SCALE 's'
#define OPTION_VERSION 'v'
#define OPTIONS "b:chr:s:v"

enum {
	FLAG_BOOTROM = 0,
	FLAG_CAPTURE,
	FLAG_HELP,
	FLAG_ROM,
	FLAG_SCALE,
	FLAG_VERSION,
	FLAG_MAX,
};

static const char *FLAG_STR[] = {
	"-b", /* FLAG_BOOTROM */
	"-c", /* FLAG_CAPTURE */
	"-h", /* FLAG_HELP */
	"-r", /* FLAG_ROM */
	"-s", /* FLAG_SCALE */
	"-v", /* FLAG_VERSION */
	"", /* FLAG_MAX */
	};

static const char *FLAG_DESCRIPTION_STR[] = {
	"Specify bootrom binary", /* FLAG_BOOTROM */
	"Enable serial capture", /* FLAG_CAPTURE */
	"Display help information", /* FLAG_HELP */
	"Specify rom binary", /* FLAG_ROM */
	"Specify display scale", /* FLAG_SCALE */
	"Display version information", /* FLAG_VERSION */
	"", /* FLAG_MAX */
	};

typedef struct {
	uint8_t data;
	uint8_t length;
} dmg_launcher_capture_t;

typedef struct {
	dmg_t configuration;
	const char *bootrom;
	const char *rom;
	dmg_launcher_capture_t capture;
	bool help;
	bool version;
} dmg_launcher_t;

static const uint32_t BUTTON[] = {
	SDL_SCANCODE_X, /* DMG_BUTTON_A */
	SDL_SCANCODE_Z, /* DMG_BUTTON_B */
	SDL_SCANCODE_C, /* DMG_BUTTON_SELECT */
	SDL_SCANCODE_SPACE, /* DMG_BUTTON_START */
	};

static const uint32_t DIRECTION[] = {
	SDL_SCANCODE_RIGHT, /* DMG_DIRECTION_RIGHT */
	SDL_SCANCODE_LEFT, /* DMG_DIRECTION_LEFT */
	SDL_SCANCODE_UP, /* DMG_DIRECTION_UP */
	SDL_SCANCODE_DOWN, /* DMG_DIRECTION_DOWN */
	};

static const uint32_t PALETTE[] = {
	0xffffff, //0xbbe5b9, /* DMG_PALETTE_WHITE */
	0x949494, //0x5ab9a8, /* DMG_PALETTE_GREY_LIGHT */
	0x525252, //0x1e606e, /* DMG_PALETTE_GREY_DARK */
	0x000000, //0x2d1b00, /* DMG_PALETTE_BLACK */
	};

#define SCALE 1

#define TRANSFER NULL

#endif /* DMG_TOOL_LAUNCHER_TYPE_H_ */
