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

#ifndef DMG_TOOL_UTILITY_ROM_INFO_TYPE_H_
#define DMG_TOOL_UTILITY_ROM_INFO_TYPE_H_

#include "../../../src/common/cartridge_type.h"
#include "../../../src/common/mapper_type.h"
#include "../../../include/common/cartridge.h"
#include "../../include/common.h"

#define DMG "DMG-ROM-INFO"
#define DMG_NOTICE "Copyright (C) 2020 David Jolly"
#define DMG_USAGE "dmg-rom-info [args]"

#define OPTION_HELP 'h'
#define OPTION_ROM 'r'
#define OPTION_VERSION 'v'
#define OPTIONS "hr:v"

#define CGB_SUPPORT 0x80
#define CGB_SUPPORT_ONLY 0xc0

#define SGB_SUPPORT 0x03

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
	"",
	"MBC2", /* MAPPER_MBC2 */
	"MBC2-BATTERY", /* MAPPER_MBC2_BATTERY */
	"",
	"ROM-RAM", /* MAPPER_ROM_RAM */
	"ROM-RAM-BATTERY", /* MAPPER_ROM_RAM_BATTERY */
	"", "", "", "", "",
	"MBC3-TIMER-BATTERY", /* MAPPER_MBC3_TIMER_BATTERY */
	"MBC3-TIMER-RAM-BATTERY", /* MAPPER_MBC3_TIMER_RAM_BATTERY */
	"MBC3", /* MAPPER_MBC3 */
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
	"1 bank (2 KB)", /* RAM_2KB */
	"1 bank (8 KB)", /* RAM_8KB */
	"4 banks (32 KB)", /* RAM_32KB */
	"16 banks (128 KB)", /* RAM_128KB */
	};

static const char *ROM_STR[] = {
	"2 banks (32 KB)", /* ROM_32KB */
	"4 banks (64 KB)", /* ROM_64KB */
	"8 banks (128 KB)", /* ROM_128KB */
	"16 banks (256 KB)", /* ROM_256KB */
	"32 banks (512 KB)", /* ROM_512KB */
	"64 banks (1 MB)", /* ROM_1MB */
	"128 banks (2 MB)", /* ROM_2MB */
	"256 banks (4 MB)", /* ROM_4MB */
	"512 banks (8 MB)", /* ROM_8MB */
	};

typedef struct {
	const char *rom;
	dmg_buffer_t buffer;
	bool help;
	bool version;
} __attribute__((packed)) dmg_rom_info_t;

#endif /* DMG_TOOL_UTILITY_ROM_INFO_TYPE_H_ */
