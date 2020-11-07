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

#ifndef DMG_TYPE_BOOTROM_H_
#define DMG_TYPE_BOOTROM_H_

#include "../common.h"

typedef struct {
	const dmg_buffer_t *buffer;
	bool enable;
} __attribute__((packed)) dmg_bootrom_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_bootrom_export(
	__in const dmg_bootrom_t *bootrom,
	__in FILE *file
	);

int dmg_bootrom_import(
	__inout dmg_bootrom_t *bootrom,
	__in FILE *file
	);

int dmg_bootrom_load(
	__inout dmg_bootrom_t *bootrom,
	__in const dmg_buffer_t *buffer
	);

uint8_t dmg_bootrom_read(
	__in const dmg_bootrom_t *bootrom,
	__in uint16_t address
	);

void dmg_bootrom_unload(
	__inout dmg_bootrom_t *bootrom
	);

void dmg_bootrom_write(
	__inout dmg_bootrom_t *bootrom,
	__in uint16_t address,
	__in uint8_t value
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TYPE_BOOTROM_H_ */
