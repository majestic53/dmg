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

#ifndef DMG_SYSTEM_SERIAL_TYPE_H_
#define DMG_SYSTEM_SERIAL_TYPE_H_

#include "../../include/system/serial.h"
#include "../../include/runtime.h"
#include "../../include/service.h"

#define CONTROL_MASK 0x81

#define POST_CONTROL 0x00
#define POST_DATA 0x00

enum {
	SELECT_EXTERNAL = 0,
	SELECT_INTERNAL,
	SELECT_MAX,
};

static const uint32_t SELECT_CYC[] = {
	0, /* SELECT_EXTERNAL */
	512, /* SELECT_INTERNAL */
	};

#ifndef NDEBUG
#define TRACE_SERIAL(_LEVEL_, _SERIAL_) \
	if((_LEVEL_) <= (LEVEL)) { \
		dmg_serial_trace(_LEVEL_, _SERIAL_); \
	}
#else
#define TRACE_SERIAL(_LEVEL_, _SERIAL_)
#endif /* NDEBUG */

#endif /* DMG_SYSTEM_SERIAL_TYPE_H_ */
