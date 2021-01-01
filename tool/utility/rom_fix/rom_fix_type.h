/* DMG_TOOL_UTILITY_ROM_INFO_TYPE_H_ */
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

#ifndef DMG_TOOL_UTILITY_ROM_FIX_TYPE_H_
#define DMG_TOOL_UTILITY_ROM_FIX_TYPE_H_

#include "../../../include/common/cartridge.h"
#include "../../include/common.h"

#define DMG_USAGE "dmg-rom-fix [args]"

#define OPTION_HELP 'h'
#define OPTION_ROM 'r'
#define OPTION_VERSION 'v'
#define OPTIONS "hr:v"

enum {
	FLAG_HELP = 0,
	FLAG_ROM,
	FLAG_VERSION,
	FLAG_MAX,
};

static const char *FLAG_STR[] = {
	"-h", /* FLAG_HELP */
	"-r", /* FLAG_ROM */
	"-v", /* FLAG_VERSION */
	"", /* FLAG_MAX */
	};

static const char *FLAG_DESCRIPTION_STR[] = {
	"Display help information", /* FLAG_HELP */
	"Specify rom file", /* FLAG_ROM */
	"Display version information", /* FLAG_VERSION */
	"", /* FLAG_MAX */
	};

typedef struct {
	const char *rom;
	dmg_buffer_t buffer;
	FILE *file;
	bool help;
	bool version;
} dmg_rom_fix_t;

#endif /* DMG_TOOL_UTILITY_ROM_FIX_TYPE_H_ */
