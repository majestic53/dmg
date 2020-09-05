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

#ifndef DMG_COMMON_DEFINE_H_
#define DMG_COMMON_DEFINE_H_

#include "../dmg.h"

#ifndef __in
#define __in
#endif /* __in */
#ifndef __inout
#define __inout
#endif /* __inout */
#ifndef __out
#define __out
#endif /* __out */

#define ADDRESS_WIDTH(_BEGIN_, _END_) \
	(((_END_) + 1) - (_BEGIN_))

#define ADDRESS_BOOTROM_BEGIN 0x0000
#define ADDRESS_BOOTROM_END 0x00ff

#define ADDRESS_RAM_BEGIN 0xc000
#define ADDRESS_RAM_END 0xdfff
#define ADDRESS_RAM_SWAP_BEGIN 0xa000
#define ADDRESS_RAM_SWAP_END 0xbfff

#define ADDRESS_ROM_BEGIN 0x0000
#define ADDRESS_ROM_END 0x3fff
#define ADDRESS_ROM_SWAP_BEGIN 0x4000
#define ADDRESS_ROM_SWAP_END 0x7fff

enum {
	ERROR_SUCCESS = 0,
	ERROR_FAILURE,
	ERROR_INVALID,
	ERROR_MAX,
};

#define ERROR_CLEAR() \
	ERROR_SET(ERROR_SUCCESS, "")

#define ERROR_SET(_ERROR_, _FORMAT_) \
	ERROR_SET_FORMAT(_ERROR_, _FORMAT_, "")
#define ERROR_SET_FORMAT(_ERROR_, _FORMAT_, ...) \
	dmg_error_set(_ERROR_, __FILE__, __FUNCTION__, __LINE__, _FORMAT_, __VA_ARGS__)

enum {
	LEVEL_NONE = 0,
	LEVEL_ERROR,
	LEVEL_WARNING,
	LEVEL_INFORMATION,
	LEVEL_VERBOSE,
	LEVEL_MAX,
};

#define TRACE(_LEVEL_, _FORMAT_) \
	TRACE_FORMAT(_LEVEL_, _FORMAT_, "")
#ifndef NDEBUG
#define TRACE_ENABLE(_CYCLE_) \
	dmg_trace_enable(true, _CYCLE_)
#define TRACE_ERROR(_FORMAT_, _FILE_, _FUNCTION_, _LINE_) \
	if((LEVEL_ERROR) <= (LEVEL)) { \
		dmg_trace(NULL, LEVEL_ERROR, _FILE_, _FUNCTION_, _LINE_, _FORMAT_, ""); \
	}
#define TRACE_FORMAT(_LEVEL_, _FORMAT_, ...) \
	if((_LEVEL_) <= (LEVEL)) { \
		dmg_trace(NULL, _LEVEL_, __FILE__, __FUNCTION__, __LINE__, _FORMAT_, __VA_ARGS__); \
	}
#else
#define TRACE_ENABLE(_CYCLE_)
#define TRACE_ERROR(_FORMAT_, _FILE_, _FUNCTION_, _LINE_)
#define TRACE_FORMAT(_LEVEL_, _FORMAT_, ...)
#endif /* NDEBUG */

typedef uint32_t dmg_cycle_t;

#endif /* DMG_COMMON_DEFINE_H_ */
