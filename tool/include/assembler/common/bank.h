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

#ifndef DMG_TOOL_ASSEMBLER_COMMON_BANK_H_
#define DMG_TOOL_ASSEMBLER_COMMON_BANK_H_

#include "../../common.h"

typedef struct {
	uint8_t data[BANK_WIDTH];
	uint16_t offset;
} dmg_assembler_bank_t;

typedef struct {
	dmg_assembler_bank_t *bank;
	uint32_t count;
} dmg_assembler_banks_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_assembler_bank_add(
	__inout dmg_assembler_banks_t *banks,
	__in uint16_t offset
	);

int dmg_assembler_bank_set_byte(
	__inout dmg_assembler_banks_t *banks,
	__in uint32_t bank,
	__in uint16_t address,
	__in uint8_t value
	);

int dmg_assembler_bank_set_word(
	__inout dmg_assembler_banks_t *banks,
	__in uint32_t bank,
	__in uint16_t address,
	__in uint16_t value
	);

int dmg_assembler_banks_allocate(
	__inout dmg_assembler_banks_t *banks
	);

void dmg_assembler_banks_free(
	__inout dmg_assembler_banks_t *banks
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TOOL_ASSEMBLER_COMMON_BANK_H_ */
