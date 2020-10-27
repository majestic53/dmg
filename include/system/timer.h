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

#ifndef DMG_SYSTEM_TIMER_H_
#define DMG_SYSTEM_TIMER_H_

#include "../common.h"

typedef union {

	struct {
		uint8_t select : 2;
		uint8_t enable : 1;
		uint8_t unused : 4;
		uint8_t overflow : 1;
	};

	uint8_t raw;
} dmg_timer_control_t;

typedef union {

	struct {
		uint8_t unused;
		uint8_t counter;
	};

	uint16_t raw;
} dmg_timer_divider_t;

typedef struct {
	dmg_timer_control_t control;
	uint8_t counter;
	uint32_t cycle;
	dmg_timer_divider_t divider;
	uint8_t modulo;
} dmg_timer_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_timer_load(
	__inout dmg_timer_t *timer,
	__in const dmg_t *configuration
	);

uint8_t dmg_timer_read(
	__in const dmg_timer_t *timer,
	__in uint16_t address
	);

void dmg_timer_step(
	__inout dmg_timer_t *timer,
	__in uint32_t cycle
	);

void dmg_timer_unload(
	__inout dmg_timer_t *timer
	);

void dmg_timer_write(
	__inout dmg_timer_t *timer,
	__in uint16_t address,
	__in uint8_t value
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_SYSTEM_TIMER_H_ */
