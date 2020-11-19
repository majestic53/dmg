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

#ifndef DMG_TYPE_CARTRIDGE_H_
#define DMG_TYPE_CARTRIDGE_H_

#include "./bank.h"

#define CARTRIDGE_HEADER_ENTRY_LENGTH 4
#define CARTRIDGE_HEADER_LICENSEE_LENGTH 2
#define CARTRIDGE_HEADER_LOGO_LENGTH 48
#define CARTRIDGE_HEADER_MANUFACTURER_LENGTH 4
#define CARTRIDGE_HEADER_TITLE_LENGTH 11

typedef struct {
	uint8_t entry[CARTRIDGE_HEADER_ENTRY_LENGTH];
	uint8_t logo[CARTRIDGE_HEADER_LOGO_LENGTH];
	char title[CARTRIDGE_HEADER_TITLE_LENGTH];
	char manufacturer[CARTRIDGE_HEADER_MANUFACTURER_LENGTH];
	uint8_t cgb;
	char licensee[CARTRIDGE_HEADER_LICENSEE_LENGTH];
	uint8_t sgb;
	uint8_t mapper;
	uint8_t rom;
	uint8_t ram;
	uint8_t destination;
	uint8_t licensee_old;
	uint8_t version;
	uint8_t checksum;
	uint16_t checksum_global;
} __attribute__((packed)) dmg_cartridge_header_t;

typedef struct {
	const dmg_cartridge_header_t *header;
	bool enable;
	dmg_bank_t ram;
	dmg_bank_t rom;
} __attribute__((packed)) dmg_cartridge_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_cartridge_export(
	__in const dmg_cartridge_t *cartridge,
	__in FILE *file
	);

int dmg_cartridge_import(
	__inout dmg_cartridge_t *cartridge,
	__in FILE *file
	);

int dmg_cartridge_load(
	__inout dmg_cartridge_t *cartridge,
	__in const dmg_buffer_t *buffer
	);

void dmg_cartridge_ram_enable(
	__inout dmg_cartridge_t *cartridge,
	__in bool enable
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
	__inout dmg_cartridge_t *cartridge,
	__in uint32_t bank,
	__in uint16_t address,
	__in uint8_t value
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TYPE_CARTRIDGE_H_ */
