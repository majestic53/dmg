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

#ifndef DMG_TYPE_MAPPER_H_
#define DMG_TYPE_MAPPER_H_

#include "./cartridge.h"

typedef struct {

	union {

		struct {
			uint8_t lower : 5;
			uint8_t upper : 2;
			uint8_t unused : 1;
		};

		uint8_t raw;
	};

	int mode;
} dmg_mbc1_t;

typedef struct {
	dmg_cartridge_t cartridge;
	dmg_mbc1_t mbc1;
	uint32_t ram;
	bool ram_enable;
	uint32_t rom;
	uint32_t rom_swap;
} dmg_mapper_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_mapper_load(
	__inout dmg_mapper_t *mapper,
	__in const dmg_buffer_t *buffer
	);

uint8_t dmg_mapper_read_ram(
	__in const dmg_mapper_t *mapper,
	__in uint16_t address
	);

uint8_t dmg_mapper_read_rom(
	__in const dmg_mapper_t *mapper,
	__in uint16_t address
	);

void dmg_mapper_unload(
	__inout dmg_mapper_t *mapper
	);

void dmg_mapper_write_ram(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	);

void dmg_mapper_write_rom(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TYPE_MAPPER_H_ */
