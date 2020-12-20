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

#include "./syntax_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

const char *
dmg_tool_directive_string(
	__in int type
	)
{
	return DIRECTIVE_STR[type];
}

const char *
dmg_tool_header_string(
	__in int type
	)
{
	return HEADER_STR[type];
}

const dmg_tool_instruction_t *
dmg_tool_instruction(
	__in uint8_t opcode,
	__in bool extended
	)
{
	return (extended ? &INSTRUCTION_EXTENDED[opcode] : &INSTRUCTION[opcode]);
}

const char *
dmg_tool_instruction_string(
	__in uint8_t opcode,
	__in bool extended
	)
{
	return (extended ? INSTRUCTION_EXTENDED_STR[opcode] : INSTRUCTION_STR[opcode]);
}

const char *
dmg_tool_mapper_string(
	__in int type
	)
{
	return MAPPER_STR[type];
}

const char *
dmg_tool_ram_string(
	__in int type
	)
{
	return RAM_STR[type];
}

const char *
dmg_tool_rom_string(
	__in int type
	)
{
	return ROM_STR[type];
}

const char *
dmg_tool_vector_string(
	__in int type
	)
{
	return VECTOR_STR[type];
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
