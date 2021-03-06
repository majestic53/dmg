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

#ifndef DMG_RUNTIME_H_
#define DMG_RUNTIME_H_

#include "./common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_runtime_action(
	__in const dmg_action_t *request,
	__in dmg_action_t *response
	);

void dmg_runtime_interrupt(
	__in int type
	);

int dmg_runtime_load(
	__in const dmg_t *configuration
	);

uint8_t dmg_runtime_read(
	__in uint16_t address
	);

int dmg_runtime_run(
	__in const uint16_t *breakpoint,
	__in uint32_t count
	);

unsigned dmg_runtime_serial_in(
	__in unsigned in
	);

int dmg_runtime_step(
	__in uint32_t instructions,
	__in const uint16_t *breakpoint,
	__in uint32_t count
	);

void dmg_runtime_unload(void);

void dmg_runtime_write(
	__in uint16_t address,
	__in uint8_t value
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_RUNTIME_H_ */
