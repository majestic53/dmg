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

#include "./instruction_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

const dmg_processor_instruction_t *
dmg_processor_instruction(
	__in uint8_t opcode,
	__in bool extended
	)
{
	return (extended ? &INSTRUCTION_EXTENDED[opcode] : &INSTRUCTION[opcode]);
}

const char *
dmg_processor_instruction_string(
	__in uint8_t opcode,
	__in bool extended
	)
{
	return (extended ? INSTRUCTION_EXTENDED_STR[opcode] : INSTRUCTION_STR[opcode]);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
