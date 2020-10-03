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

#ifndef DMG_SYSTEM_TIMER_TYPE_H_
#define DMG_SYSTEM_TIMER_TYPE_H_

#include "../../include/system/timer.h"
#include "../../include/runtime.h"

#define CONTROL_MASK 0x3

#define POST_CONTROL 0x00
#define POST_COUNTER 0x00
#define POST_DIVIDER 0xabcc
#define POST_MODULO 0x00

enum {
	SELECT_4096 = 0,
	SELECT_262144,
	SELECT_65536,
	SELECT_16386,
	SELECT_MAX,
};

// TODO

#endif /* DMG_SYSTEM_TIMER_TYPE_H_ */
