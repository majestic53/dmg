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

#ifndef DMG_TOOL_SYNTAX_H_
#define DMG_TOOL_SYNTAX_H_

#include "../../include/system/processor/instruction.h"

#define CHARACTER_EOF '\0'
#define CHARACTER_FILL '.'
#define CHARACTER_NEWLINE '\n'

#define DELIMITER_BINARY "@"
#define DELIMITER_COMMENT ';'
#define DELIMITER_DIRECTIVE "."
#define DELIMITER_HEXIDECIMAL "$"
#define DELIMITER_LABEL ":"

#define LITERAL_MAX 32

enum {
	DIRECTIVE_BANK = 0,
	DIRECTIVE_DATA,
	DIRECTIVE_ORIGIN,
	DIRECTIVE_MAX,
};

enum {
	HEADER_ENTRY = 0,
	HEADER_LOGO,
	HEADER_TITLE,
	HEADER_MANUFACTURER,
	HEADER_COLOR_GAMEBOY,
	HEADER_LICENSEE,
	HEADER_SUPER_GAMEBOY,
	HEADER_MAPPER,
	HEADER_ROM,
	HEADER_RAM,
	HEADER_DESTINATION,
	HEADER_LICENSEE_OLD,
	HEADER_VERSION,
	HEADER_CHECKSUM,
	HEADER_CHECKSUM_GLOBAL,
	HEADER_MAX,
};

enum {
	VECTOR_RST_00 = 0,
	VECTOR_RST_08,
	VECTOR_RST_10,
	VECTOR_RST_18,
	VECTOR_RST_20,
	VECTOR_RST_28,
	VECTOR_RST_30,
	VECTOR_RST_38,
	VECTOR_VBLANK,
	VECTOR_LCDC,
	VECTOR_TIMER,
	VECTOR_SERIAL,
	VECTOR_JOYPAD,
	VECTOR_MAX,
};

typedef struct {
	uint8_t opcode;
	uint8_t operand;
} dmg_tool_instruction_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

const char *dmg_tool_directive_string(
	__in int type
	);

const char *dmg_tool_header_string(
	__in int type
	);

const dmg_tool_instruction_t *dmg_tool_instruction(
	__in uint8_t opcode,
	__in bool extended
	);

const char *dmg_tool_instruction_string(
	__in uint8_t opcode,
	__in bool extended
	);

const char *dmg_tool_mapper_string(
	__in int type
	);

const char *dmg_tool_ram_string(
	__in int type
	);

const char *dmg_tool_rom_string(
	__in int type
	);

const char *dmg_tool_vector_string(
	__in int type
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TOOL_SYNTAX_H_ */
