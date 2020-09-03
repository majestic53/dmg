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

#define ERROR_MALFORMED "Malformed error"
#define ERROR_LENGTH_MAX 256

#ifndef NDEBUG
#ifdef COLOR

static const char *LEVEL_STR[] = {
	"\x1b[0m", /* LEVEL_NONE */
	"\x1b[91m", /* LEVEL_ERROR */
	"\x1b[93m", /* LEVEL_WARNING */
	"\x1b[94m", /* LEVEL_INFORMATION */
	"\x1b[90m", /* LEVEL_VERBOSE */
	"", /* LEVEL_MAX */
	};

#endif /* COLOR */

#define MS_PER_CYCLE 0.000238f /* 238 ns/cycle */

#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
#define TIMESTAMP_LENGTH_MAX 32
#define TIMESTAMP_MALFORMED "Malformed timestamp"

#define TRACE_MALFORMED "Malformed trace"
#define TRACE_LENGTH_MAX 256

#endif /* NDEBUG */

typedef struct {
	int error;
	char str[ERROR_LENGTH_MAX];
} dmg_error_t;

typedef struct {
	uint32_t cycle;
	const dmg_t *configuration;

	// TODO: ADD SUBSYSTEMS

} dmg_runtime_t;

#endif /* DMG_RUNTIME_TYPE_H_ */
