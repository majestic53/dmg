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

#ifndef DMG_RUNTIME_TYPE_H_
#define DMG_RUNTIME_TYPE_H_

#include "../include/runtime.h"

typedef struct {
	dmg_cycle_t cycle;
	const dmg_t *configuration;

	// TODO: ADD SUBSYSTEMS

} dmg_runtime_t;

#endif /* DMG_RUNTIME_TYPE_H_ */
