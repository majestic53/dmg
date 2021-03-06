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

#ifndef DMG_COMMON_BANK_H_
#define DMG_COMMON_BANK_H_

#include "./buffer.h"

typedef struct {
	dmg_buffer_t *buffer;
	uint32_t count;
} __attribute__((packed)) dmg_bank_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_bank_allocate(
	__inout dmg_bank_t *bank,
	__in uint32_t count
	);

void dmg_bank_free(
	__inout dmg_bank_t *bank
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_COMMON_BANK_H_ */
