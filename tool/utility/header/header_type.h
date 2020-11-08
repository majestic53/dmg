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

#ifndef DMG_TOOL_UTILITY_HEADER_TYPE_H_
#define DMG_TOOL_UTILITY_HEADER_TYPE_H_

#include "../../../src/type/cartridge_type.h"
#include "../../../src/type/mapper_type.h"
#include "../../../include/type/cartridge.h"
#include "../../include/file.h"

#define DMG "DMG-ROM-INFO"
#define DMG_NOTICE "Copyright (C) 2020 David Jolly"
#define DMG_USAGE "dmg-rom-info [args]"

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

static const char *MAPPER_STR[] = {
	"ROM", /* MAPPER_ROM_ONLY */
	"MBC1", /* MAPPER_MBC1 */
	"MBC1-RAM", /* MAPPER_MBC1_RAM */
	"MBC1-RAM-BATTERY", /* MAPPER_MBC1_RAM_BATTERY */
	"", "", "", "", "", "", "", "", "", "", "", "", "", "",
	"MBC3-RAM", /* MAPPER_MBC3_RAM */
	"MBC3-RAM-BATTERY", /* MAPPER_MBC3_RAM_BATTERY */
	"", "", "", "", "",
	"MBC5", /* MAPPER_MBC5 */
	"MBC5-RAM", /* MAPPER_MBC5_RAM */
	"MBC5-RAM-BATTERY", /* MAPPER_MBC5_RAM_BATTERY */
	"MBC5-RUMBLE", /* MAPPER_MBC5_RUMBLE */
	"MBC5-RUMBLE-RAM", /* MAPPER_MBC5_RUMBLE_RAM */
	"MBC5-RUMBLE-RAM-BATTERY", /* MAPPER_MBC5_RUMBLE_RAM_BATTERY */
	};

static const char *RAM_STR[] = {
	"None", /* RAM_NONE */
	"2 KB, 1 bank", /* RAM_2KB */
	"8 KB, 1 bank", /* RAM_8KB */
	"32 KB, 4 banks", /* RAM_32KB */
	"128 KB, 16 banks", /* RAM_128KB */
	};

static const char *ROM_STR[] = {
	"32 KB, 2 banks", /* ROM_32KB */
	"64 KB, 4 banks", /* ROM_64KB */
	"128 KB, 8 banks", /* ROM_128KB */
	"256 KB, 16 banks", /* ROM_256KB */
	"512 KB, 32 banks", /* ROM_512KB */
	"1 MB, 64 banks", /* ROM_1MB */
	"2 MB, 128 banks", /* ROM_2MB */
	"4 MB, 256 banks", /* ROM_4MB */
	"8 MB, 512 banks", /* ROM_8MB */
	};

typedef struct {
	const char *rom;
	dmg_buffer_t buffer;
	bool help;
	bool version;
} dmg_utility_header_t;

#endif /* DMG_TOOL_UTILITY_HEADER_TYPE_H_ */
