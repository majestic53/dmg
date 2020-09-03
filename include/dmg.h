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

#ifndef DMG_H_
#define DMG_H_

#include <stdint.h>

enum {
	DMG_BUTTON_A = 0,
	DMG_BUTTON_B,
	DMG_BUTTON_SELECT,
	DMG_BUTTON_START,
	DMG_BUTTON_MAX,
};

enum {
	DMG_DIRECTION_RIGHT = 0,
	DMG_DIRECTION_LEFT,
	DMG_DIRECTION_UP,
	DMG_DIRECTION_DOWN,
	DMG_DIRECTION_MAX,
};

enum {
	DMG_PALETTE_WHITE = 0,
	DMG_PALETTE_GREY_LIGHT,
	DMG_PALETTE_GREY_DARK,
	DMG_PALETTE_BLACK,
	DMG_PALETTE_MAX,
};

typedef uint8_t (*dmg_serial_cb)(uint8_t);

typedef struct {
	uint8_t *data;
	uint32_t length;
} dmg_buffer_t;

typedef union {

	struct {
		uint8_t blue;
		uint8_t green;
		uint8_t red;
		uint8_t alpha;
	};

	uint32_t raw;
} dmg_color_t;

typedef struct {
	uint32_t button[DMG_BUTTON_MAX];
	uint32_t direction[DMG_DIRECTION_MAX];
} dmg_input_t;

typedef struct {
	dmg_color_t palette[DMG_PALETTE_MAX];
	uint8_t scale;
	dmg_serial_cb transfer;
} dmg_output_t;

typedef struct {
	uint32_t major;
	uint32_t minor;
	uint32_t patch;
} dmg_version_t;

typedef struct {
	dmg_buffer_t bootrom;
	dmg_buffer_t rom;
	dmg_input_t input;
	dmg_output_t output;
} dmg_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg(const dmg_t *);

const char *dmg_error(void);

const dmg_version_t *dmg_version(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_H_ */
