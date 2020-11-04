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

#ifndef DMG_TYPE_SAVE_TYPE_H_
#define DMG_TYPE_SAVE_TYPE_H_

#include "../../include/type/save.h"
#include "../runtime_type.h"

#define SAVE_VERSION_1 1

#define SAVE_MAGIC 0x444d4700
#define SAVE_VERSION SAVE_VERSION_1

#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
#define TIMESTAMP_LENGTH_MAX 32
#define TIMESTAMP_MALFORMED "Malformed timestamp"

typedef struct {
	uint32_t magic;
	uint8_t version;
	uint32_t timestamp;
	uint32_t length;
} __attribute__((packed)) dmg_save_header_t;

#ifndef NDEBUG
#define TRACE_SAVE(_LEVEL_, _SAVE_) \
	if((_LEVEL_) <= (LEVEL)) { \
		dmg_save_trace(_LEVEL_, _SAVE_); \
	}
#else
#define TRACE_SAVE(_LEVEL_, _SAVE_)
#endif /* NDEBUG */

#endif /* DMG_TYPE_SAVE_TYPE_H_ */
