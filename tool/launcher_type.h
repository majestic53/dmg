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

#define DMG "DMG"
#define DMG_NOTICE "Copyright (C) 2020 David Jolly"
#define DMG_USAGE "dmg [args]"

#define OPTION_BOOTROM 'b'
#define OPTION_HELP 'h'
#define OPTION_ROM 'r'
#define OPTION_VERSION 'v'
#define OPTIONS "b:hr:v"

enum {
	FLAG_BOOTROM = 0,
	FLAG_HELP,
	FLAG_ROM,
	FLAG_VERSION,
	FLAG_MAX,
};

static const char *FLAG_STR[] = {
	"-b", /* FLAG_BOOTROM */
	"-h", /* FLAG_HELP */
	"-r", /* FLAG_ROM */
	"-v", /* FLAG_VERSION */
	"", /* FLAG_MAX */
	};

static const char *FLAG_DESCRIPTION_STR[] = {
	"Specify bootrom binary", /* FLAG_BOOTROM */
	"Display help information", /* FLAG_HELP */
	"Specify rom binary", /* FLAG_ROM */
	"Display version information", /* FLAG_VERSION */
	"", /* FLAG_MAX */
	};

typedef struct {
	dmg_t configuration;
	const char *bootrom;
	const char *rom;
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

static const dmg_color_t PALETTE[] = {
	{{ 0xb9, 0xe5, 0xbb, 0xff }}, /* DMG_PALETTE_WHITE */
	{{ 0xa8, 0xb9, 0x5a, 0xff }}, /* DMG_PALETTE_GREY_LIGHT */
	{{ 0x6e, 0x60, 0x1e, 0xff }}, /* DMG_PALETTE_GREY_DARK */
	{{ 0x00, 0x1b, 0x2d, 0xff }}, /* DMG_PALETTE_BLACK */
	};

#define SCALE 1

#define TRANSFER NULL

#endif /* DMG_TOOL_LAUNCHER_TYPE_H_ */
