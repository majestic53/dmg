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

#ifndef DMG_SYSTEM_MEMORY_H_
#define DMG_SYSTEM_MEMORY_H_

#include "../common/bootrom.h"
#include "../common/mapper.h"

typedef struct {
	dmg_bootrom_t bootrom;
	dmg_mapper_t mapper;
	dmg_buffer_t ram;
	dmg_buffer_t ram_high;
} __attribute__((packed)) dmg_memory_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_memory_export(
	__in const dmg_memory_t *memory,
	__in FILE *file
	);

int dmg_memory_import(
	__inout dmg_memory_t *memory,
	__in FILE *file
	);

int dmg_memory_load(
	__inout dmg_memory_t *memory,
	__in const dmg_t *configuration
	);

uint8_t dmg_memory_read(
	__in const dmg_memory_t *memory,
	__in uint16_t address
	);

void dmg_memory_unload(
	__inout dmg_memory_t *memory
	);

void dmg_memory_write(
	__inout dmg_memory_t *memory,
	__in uint16_t address,
	__in uint8_t value
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_SYSTEM_MEMORY_H_ */
