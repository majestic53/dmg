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

#ifndef DMG_TOOL_UTILITY_DASM_TYPE_H_
#define DMG_TOOL_UTILITY_DASM_TYPE_H_

#include "../../../src/common/cartridge_type.h"
#include "../../../src/common/mapper_type.h"
#include "../../../include/common/cartridge.h"
#include "../../src/common/version_type.h"
#include "../../include/common.h"

#define DMG_USAGE "dmg-dasm [args]"

#define OPTION_HELP 'h'
#define OPTION_OUTPUT 'o'
#define OPTION_ROM 'r'
#define OPTION_VERSION 'v'
#define OPTIONS "ho:r:v"

#define PATH_OUTPUT "output.s"

#define HEADER_WIDTH 16

#define SUBROUTINE_PREFIX "sub_"
#define SUBROUTINE_MAX 10000

#define VECTOR_WIDTH 8

enum {
	FLAG_HELP = 0,
	FLAG_OUTPUT,
	FLAG_ROM,
	FLAG_VERSION,
	FLAG_MAX,
};

static const char *FLAG_STR[] = {
	"-h", /* FLAG_HELP */
	"-o", /* FLAG_OUTPUT */
	"-r", /* FLAG_ROM */
	"-v", /* FLAG_VERSION */
	"", /* FLAG_MAX */
	};

static const char *FLAG_DESCRIPTION_STR[] = {
	"Display help information", /* FLAG_HELP */
	"Specify output file", /* FLAG_OUTPUT */
	"Specify rom file", /* FLAG_ROM */
	"Display version information", /* FLAG_VERSION */
	"", /* FLAG_MAX */
	};

static const uint32_t HEADER_LEN[] = {
	CARTRIDGE_HEADER_ENTRY_LENGTH, /* HEADER_ENTRY */
	CARTRIDGE_HEADER_LOGO_LENGTH, /* HEADER_LOGO */
	CARTRIDGE_HEADER_TITLE_LENGTH, /* HEADER_TITLE */
	CARTRIDGE_HEADER_MANUFACTURER_LENGTH, /* HEADER_MANUFACTURER */
	sizeof(uint8_t), /* HEADER_COLOR_GAMEBOY */
	CARTRIDGE_HEADER_LICENSEE_LENGTH, /* HEADER_LICENSEE */
	sizeof(uint8_t), /* HEADER_SUPER_GAMEBOY */
	sizeof(uint8_t), /* HEADER_MAPPER */
	sizeof(uint8_t), /* HEADER_ROM */
	sizeof(uint8_t), /* HEADER_RAM */
	sizeof(uint8_t), /* HEADER_DESTINATION */
	sizeof(uint8_t), /* HEADER_LICENSEE_OLD */
	sizeof(uint8_t), /* HEADER_VERSION */
	sizeof(uint8_t), /* HEADER_CHECKSUM */
	sizeof(uint16_t), /* HEADER_CHECKSUM_GLOBAL */
	0, /* HEADER_MAX */
	};

typedef struct {
	const char *rom;
	const char *output;
	dmg_buffer_t buffer;
	uint16_t subroutine[SUBROUTINE_MAX];
	uint32_t subroutine_count;
	FILE *file;
	bool help;
	bool version;
} dmg_dasm_t;

#endif /* DMG_TOOL_UTILITY_DASM_TYPE_H_ */
