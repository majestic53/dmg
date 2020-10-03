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

#include "./timer_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_timer_load(
	__inout dmg_timer_t *timer,
	__in const dmg_buffer_t *bootrom
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Timer loading");

	if(!bootrom->data) {
		timer->control.raw = POST_CONTROL;
		timer->counter = POST_COUNTER;
		timer->divider = POST_DIVIDER;
		timer->modulo = POST_MODULO;
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "Timer TAC=%02x [Select=%u, Enable=%u]", timer->control.raw,
		timer->control.select, timer->control.enable);
	TRACE_FORMAT(LEVEL_VERBOSE, "Timer TIMA=%02x", timer->counter);
	TRACE_FORMAT(LEVEL_VERBOSE, "Timer TMA=%02x", timer->modulo);
	TRACE_FORMAT(LEVEL_VERBOSE, "Timer DIV=%04x", timer->divider);
	TRACE(LEVEL_INFORMATION, "Timer loaded");

	return result;
}

uint8_t
dmg_timer_read(
	__in const dmg_timer_t *timer,
	__in uint16_t address
	)
{
	uint8_t result = 0;

	switch(address) {
		case ADDRESS_TIMER_CONTROL:
			result = timer->control.raw;
			break;
		case ADDRESS_TIMER_COUNTER:
			result = timer->counter;
			break;
		case ADDRESS_TIMER_DIVIDER:
			result = (timer->divider >> CHAR_BIT);
			break;
		case ADDRESS_TIMER_MODULO:
			result = timer->modulo;
			break;
		default:
			result = UINT8_MAX;

			TRACE_FORMAT(LEVEL_WARNING, "Unsupported timer read [%04x]->%02x", address, result);
			break;
	}

	return result;
}

void
dmg_timer_step(
	__inout dmg_timer_t *timer,
	__in uint32_t cycle
	)
{
	timer->divider += cycle;

	if(timer->control.enable) {

		// TODO: COUNTER/MODULO
	}
}

void
dmg_timer_unload(
	__inout dmg_timer_t *timer
	)
{
	TRACE(LEVEL_INFORMATION, "Timer unloading");

	memset(timer, 0, sizeof(*timer));

	TRACE(LEVEL_INFORMATION, "Timer unloaded");
}

void
dmg_timer_write(
	__inout dmg_timer_t *timer,
	__in uint16_t address,
	__in uint8_t value
	)
{

	switch(address) {
		case ADDRESS_TIMER_CONTROL:
			timer->control.raw = (value & CONTROL_MASK);

			TRACE_FORMAT(LEVEL_VERBOSE, "Timer select=%u, enable=%x", timer->control.select, timer->control.enable);
			break;
		case ADDRESS_TIMER_COUNTER:
			timer->counter = value;
			break;
		case ADDRESS_TIMER_DIVIDER:
			timer->divider = 0;
			break;
		case ADDRESS_TIMER_MODULO:
			timer->modulo = value;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported timer write [%04x]<-%02x", address, value);
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
