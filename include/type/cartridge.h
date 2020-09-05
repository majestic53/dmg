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

#ifndef DMG_TYPE_CARTRIDGE_H_
#define DMG_TYPE_CARTRIDGE_H_

#include "./bank.h"

typedef struct {
	uint8_t magic[4];
	uint8_t logo[48];
	char title[11];
	char manufacturer[4];
	uint8_t cgb;
	char licensee[2];
	uint8_t sgb;
	uint8_t mapper;
	uint8_t rom;
	uint8_t ram;
	uint8_t destination;
	uint8_t licensee_old;
	uint8_t version;
	uint8_t checksum;
	uint16_t checksum_global;
} dmg_header_t;

typedef struct {
	const dmg_header_t *header;
	dmg_bank_t ram;
	dmg_bank_t rom;
} dmg_cartridge_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_cartridge_load(
	__inout dmg_cartridge_t *cartridge,
	__in const dmg_buffer_t *buffer
	);

uint8_t dmg_cartridge_read_ram(
	__in const dmg_cartridge_t *cartridge,
	__in uint32_t bank,
	__in uint16_t address
	);

uint8_t dmg_cartridge_read_rom(
	__in const dmg_cartridge_t *cartridge,
	__in uint32_t bank,
	__in uint16_t address
	);

void dmg_cartridge_unload(
	__inout dmg_cartridge_t *cartridge
	);

void dmg_cartridge_write_ram(
	__in dmg_cartridge_t *cartridge,
	__in uint32_t bank,
	__in uint16_t address,
	__in uint8_t value
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TYPE_CARTRIDGE_H_ */
