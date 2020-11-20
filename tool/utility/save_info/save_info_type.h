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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DMG_TOOL_UTILITY_SAVE_INFO_TYPE_H_
#define DMG_TOOL_UTILITY_SAVE_INFO_TYPE_H_

#include "../../../src/service_type.h"
#include "../../include/common.h"

#define DMG "DMG-SAVE-INFO"
#define DMG_NOTICE "Copyright (C) 2020 David Jolly"
#define DMG_USAGE "dmg-save-info [args]"

#define OPTION_HELP 'h'
#define OPTION_SAVE 's'
#define OPTION_VERSION 'v'
#define OPTIONS "hs:v"

#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
#define TIMESTAMP_LENGTH_MAX 32
#define TIMESTAMP_MALFORMED "Malformed timestamp"

enum {
	FLAG_HELP = 0,
	FLAG_SAVE,
	FLAG_VERSION,
	FLAG_MAX,
};

static const char *FLAG_STR[] = {
	"-h", /* FLAG_HELP */
	"-s", /* FLAG_SAVE */
	"-v", /* FLAG_VERSION */
	"", /* FLAG_MAX */
	};

static const char *FLAG_DESCRIPTION_STR[] = {
	"Display help information", /* FLAG_HELP */
	"Specify save file", /* FLAG_SAVE */
	"Display version information", /* FLAG_VERSION */
	"", /* FLAG_MAX */
	};

typedef struct {
	const char *save;
	dmg_buffer_t buffer;
	bool help;
	bool version;
} __attribute__((packed)) dmg_save_info_t;

#endif /* DMG_TOOL_UTILITY_SAVE_INFO_TYPE_H_ */
