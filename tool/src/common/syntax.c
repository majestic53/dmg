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

static bool
dmg_tool_is_string(
	__in const char **strings,
	__in int count,
	__in const char *str,
	__inout int *type
	)
{

	for(*type = 0; *type < count; ++*type) {

		if(!strcmp(strings[*type], str)) {
			break;
		}
	}

	return (*type < count);
}

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

bool
dmg_tool_is_directive_string(
	__in const char *str,
	__inout int *type
	)
{
	return dmg_tool_is_string(DIRECTIVE_STR, DIRECTIVE_MAX, str, type);
}

bool
dmg_tool_is_macro_string(
	__in const char *str,
	__inout int *type
	)
{
	return dmg_tool_is_string(MACRO_STR, MACRO_MAX, str, type);
}

bool
dmg_tool_is_opcode_string(
	__in const char *str,
	__inout int *type
	)
{
	return dmg_tool_is_string(OPCODE_STR, OPCODE_MAX, str, type);
}

bool
dmg_tool_is_operator_string(
	__in const char *str,
	__inout int *type
	)
{
	return dmg_tool_is_string(OPERATOR_STR, OPERATOR_MAX, str, type);
}

bool
dmg_tool_is_register_string(
	__in const char *str,
	__inout int *type
	)
{
	return dmg_tool_is_string(REGISTER_STR, REGISTER_MAX, str, type);
}

bool
dmg_tool_is_symbol_string(
	__in const char *str,
	__inout int *type
	)
{
	return dmg_tool_is_string(SYMBOL_STR, SYMBOL_MAX, str, type);
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
