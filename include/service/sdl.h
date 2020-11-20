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

#ifndef DMG_SERVICE_SDL_H_
#define DMG_SERVICE_SDL_H_

#include "../common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool dmg_sdl_button(
	__in int button
	);

bool dmg_sdl_direction(
	__in int direction
	);

int dmg_sdl_load(
	__in const dmg_t *configuration,
	__in const char *title
	);

void dmg_sdl_pixel(
	__in uint8_t color,
	__in uint8_t x,
	__in uint8_t y
	);

bool dmg_sdl_poll(
	__in bool complete,
	__in float rate
	);

void dmg_sdl_sync(void);

void dmg_sdl_unload(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_SERVICE_SDL_H_ */
