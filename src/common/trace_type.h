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

#ifndef DMG_COMMON_TRACE_TYPE_H_
#define DMG_COMMON_TRACE_TYPE_H_

#include "../../include/common.h"

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

#define MS_PER_CYCLE 0.000238f

#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
#define TIMESTAMP_LENGTH_MAX 32
#define TIMESTAMP_MALFORMED "Malformed timestamp"

#define TRACE_LENGTH_MAX 256
#define TRACE_MALFORMED "Malformed trace"

typedef struct {
	bool enable;
	uint32_t *cycle;
} dmg_trace_t;

#endif /* NDEBUG */

#endif /* DMG_COMMON_TRACE_TYPE_H_ */
