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

#ifndef DMG_COMMON_H_
#define DMG_COMMON_H_

#include <SDL2/SDL.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "./dmg.h"

#ifndef __in
#define __in
#endif /* __in */
#ifndef __inout
#define __inout
#endif /* __inout */
#ifndef __out
#define __out
#endif /* __out */

enum {
	ERROR_SUCCESS = 0,
	ERROR_FAILURE,
	ERROR_INVALID,
	ERROR_MAX,
};

#define CLEAR_ERROR() \
	SET_ERROR(ERROR_SUCCESS, "")

#define SET_ERROR(_ERROR_, _FORMAT_) \
	SET_ERROR_FORMAT(_ERROR_, _FORMAT_, "")
#define SET_ERROR_FORMAT(_ERROR_, _FORMAT_, ...) \
	dmg_runtime_error_set(_ERROR_, __FILE__, __FUNCTION__, __LINE__, _FORMAT_, __VA_ARGS__)

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
#define TRACE_ERROR(_FORMAT_, _FILE_, _FUNCTION_, _LINE_) \
	if((LEVEL_ERROR) <= (LEVEL)) { \
		dmg_runtime_trace(NULL, LEVEL_ERROR, _FILE_, _FUNCTION_, _LINE_, _FORMAT_, ""); \
	}
#define TRACE_FORMAT(_LEVEL_, _FORMAT_, ...) \
	if((_LEVEL_) <= (LEVEL)) { \
		dmg_runtime_trace(NULL, _LEVEL_, __FILE__, __FUNCTION__, __LINE__, _FORMAT_, __VA_ARGS__); \
	}
#else
#define TRACE_ERROR(_FORMAT_, _FILE_, _FUNCTION_, _LINE_)
#define TRACE_FORMAT(_LEVEL_, _FORMAT_, ...)
#endif /* NDEBUG */

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 1

#endif /* DMG_COMMON_H_ */
