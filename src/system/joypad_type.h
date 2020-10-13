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

#ifndef DMG_SYSTEM_JOYPAD_TYPE_H_
#define DMG_SYSTEM_JOYPAD_TYPE_H_

#include "../../include/system/joypad.h"
#include "../../include/runtime.h"
#include "../../include/service.h"

#define POST_STATE 0xff

#define STATE_CYCLE 1024
#define STATE_MASK 0x30
#define STATE_RESET 0xcf

#ifndef NDEBUG
#define TRACE_JOYPAD(_LEVEL_, _JOYPAD_) \
	if((_LEVEL_) <= (LEVEL)) { \
		dmg_joypad_trace(_LEVEL_, _JOYPAD_); \
	}
#else
#define TRACE_JOYPAD(_LEVEL_, _JOYPAD_)
#endif /* NDEBUG */

#endif /* DMG_SYSTEM_JOYPAD_TYPE_H_ */
