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

typedef struct {
	uint32_t begin;
	uint32_t end;
	uint32_t frame;
	float framerate;
	float frequency;
} dmg_sdl_t;

#endif /* DMG_SERVICE_SDL_TYPE_H_ */
