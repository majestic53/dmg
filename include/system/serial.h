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

#ifndef DMG_SYSTEM_SERIAL_H_
#define DMG_SYSTEM_SERIAL_H_

#include "../common.h"

typedef union {

	struct {
		uint8_t select : 1;
		uint8_t unused : 6;
		uint8_t enable : 1;
	};

	uint8_t raw;
} dmg_serial_control_t;

typedef union {

	struct {
		uint8_t lsb : 1;
		uint8_t unused : 6;
		uint8_t msb : 1;
	};

	uint8_t raw;
} dmg_serial_data_t;

typedef struct {
	dmg_serial_control_t control;
	uint32_t cycle;
	dmg_serial_data_t data;
	uint8_t remaining;
	dmg_serial_transfer transfer;
} dmg_serial_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_serial_export(
	__in const dmg_serial_t *serial,
	__in FILE *file
	);

int dmg_serial_import(
	__inout dmg_serial_t *serial,
	__in FILE *file
	);

int dmg_serial_load(
	__inout dmg_serial_t *serial,
	__in const dmg_t *configuration
	);

uint8_t dmg_serial_read(
	__in const dmg_serial_t *serial,
	__in uint16_t address
	);

void dmg_serial_step(
	__inout dmg_serial_t *serial,
	__in uint32_t cycle
	);

void dmg_serial_unload(
	__inout dmg_serial_t *serial
	);

void dmg_serial_write(
	__inout dmg_serial_t *serial,
	__in uint16_t address,
	__in uint8_t value
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_SYSTEM_SERIAL_H_ */
