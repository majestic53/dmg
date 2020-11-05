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

#ifndef DMG_SYSTEM_JOYPAD_H_
#define DMG_SYSTEM_JOYPAD_H_

#include "../common.h"

typedef union {

	struct {
		uint8_t p10 : 1;
		uint8_t p11 : 1;
		uint8_t p12 : 1;
		uint8_t p13 : 1;
		uint8_t direction : 1;
		uint8_t button : 1;
		uint8_t unused : 2;
	};

	uint8_t raw;
} dmg_joypad_state_t;

typedef struct {
	uint32_t cycle;
	bool button[DMG_BUTTON_MAX];
	bool direction[DMG_DIRECTION_MAX];
	dmg_joypad_state_t state;
} dmg_joypad_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_joypad_export(
	__in const dmg_joypad_t *joypad,
	__in FILE *file
	);

int dmg_joypad_import(
	__inout dmg_joypad_t *joypad,
	__in FILE *file
	);

int dmg_joypad_load(
	__inout dmg_joypad_t *joypad,
	__in const dmg_t *configuration
	);

uint8_t dmg_joypad_read(
	__in const dmg_joypad_t *joypad,
	__in uint16_t address
	);

void dmg_joypad_step(
	__inout dmg_joypad_t *joypad,
	__in uint32_t cycle
	);

void dmg_joypad_unload(
	__inout dmg_joypad_t *joypad
	);

void dmg_joypad_write(
	__inout dmg_joypad_t *joypad,
	__in uint16_t address,
	__in uint8_t value
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_SYSTEM_JOYPAD_H_ */
