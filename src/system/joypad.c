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

#include "./joypad_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NDEBUG

static void
dmg_joypad_trace(
	__in int level,
	__inout const dmg_joypad_t *joypad
	)
{
	TRACE_FORMAT(level, "Joypad P1=%02x [%i%i%i%i%i%i]", joypad->state.raw, joypad->state.button, joypad->state.direction,
		joypad->state.p10, joypad->state.p11, joypad->state.p12, joypad->state.p13);
	TRACE_FORMAT(level, "Joypad Button=(%c%c%c%c)", joypad->button[DMG_BUTTON_A] ? 'A' : '-',
		joypad->button[DMG_BUTTON_B] ? 'B' : '-', joypad->button[DMG_BUTTON_SELECT] ? 'L' : '-',
		joypad->button[DMG_BUTTON_START] ? 'S' : '-');
	TRACE_FORMAT(level, "Joypad Direction=(%c%c%c%c)", joypad->direction[DMG_DIRECTION_RIGHT] ? 'R' : '-',
		joypad->direction[DMG_DIRECTION_LEFT] ? 'L' : '-', joypad->direction[DMG_DIRECTION_UP] ? 'U' : '-',
		joypad->direction[DMG_DIRECTION_DOWN] ? 'D' : '-');
}

#endif /* NDEBUG */

static void
dmg_joypad_poll(
	__inout dmg_joypad_t *joypad
	)
{
	bool interrupt = false;

	for(int index = 0; index < DMG_BUTTON_MAX; ++index) {

		if(joypad->button[index] != dmg_service_button(index)) {
			joypad->button[index] = !joypad->button[index];

			if(!joypad->state.button) {
				interrupt = true;
			}
		}
	}

	for(int index = 0; index < DMG_DIRECTION_MAX; ++index) {

		if(joypad->direction[index] != dmg_service_direction(index)) {
			joypad->direction[index] = !joypad->direction[index];

			if(!joypad->state.direction) {
				interrupt = true;
			}
		}
	}

	if(interrupt) {
		dmg_runtime_interrupt(INTERRUPT_JOYPAD);

		TRACE_JOYPAD(LEVEL_VERBOSE, joypad);
	}
}

int
dmg_joypad_export(
	__in const dmg_joypad_t *joypad,
	__in FILE *file
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Joypad exporting");
	TRACE_JOYPAD(LEVEL_VERBOSE, joypad);

	if((result = dmg_service_export_data(file, &joypad->cycle, sizeof(joypad->cycle))) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &joypad->state, sizeof(dmg_joypad_state_t))) != ERROR_SUCCESS) {
		goto exit;
	}

	TRACE(LEVEL_INFORMATION, "Joypad exported");

exit:
	return result;
}

int
dmg_joypad_import(
	__inout dmg_joypad_t *joypad,
	__in FILE *file
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Joypad importing");

	if((result = dmg_service_import_data(file, &joypad->cycle, sizeof(joypad->cycle))) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &joypad->state, sizeof(dmg_joypad_state_t))) != ERROR_SUCCESS) {
		goto exit;
	}

	TRACE_JOYPAD(LEVEL_VERBOSE, joypad);
	TRACE(LEVEL_INFORMATION, "Joypad imported");

exit:
	return result;
}

int
dmg_joypad_load(
	__inout dmg_joypad_t *joypad,
	__in const dmg_t *configuration
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Joypad loading");
	joypad->state.raw = POST_STATE;
	dmg_joypad_poll(joypad);
	TRACE(LEVEL_INFORMATION, "Joypad loaded");

	return result;
}

uint8_t
dmg_joypad_read(
	__in const dmg_joypad_t *joypad,
	__in uint16_t address
	)
{
	uint8_t result = 0;

	switch(address) {
		case ADDRESS_JOYPAD_STATE:
			result = (joypad->state.raw | STATE_READ);
			break;
		default:
			result = UINT8_MAX;
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported joypad read [%04x]->%02x", address, result);
			break;
	}

	return result;
}

void
dmg_joypad_step(
	__inout dmg_joypad_t *joypad,
	__in uint32_t cycle
	)
{

	for(uint32_t tick = 0; tick < cycle; tick += CYCLE) {

		if((joypad->cycle += CYCLE) >= STATE_CYCLE) {
			joypad->cycle %= STATE_CYCLE;
			dmg_joypad_poll(joypad);
		}
	}
}

void
dmg_joypad_unload(
	__inout dmg_joypad_t *joypad
	)
{
	TRACE(LEVEL_INFORMATION, "Joypad unloading");
	memset(joypad, 0, sizeof(*joypad));
	TRACE(LEVEL_INFORMATION, "Joypad unloaded");
}

void
dmg_joypad_write(
	__inout dmg_joypad_t *joypad,
	__in uint16_t address,
	__in uint8_t value
	)
{

	switch(address) {
		case ADDRESS_JOYPAD_STATE:
			joypad->state.raw = ((value & STATE_MASK) | STATE_WRITE);
			TRACE_FORMAT(LEVEL_VERBOSE, "Joypad button=%x, direction=%x", joypad->state.button, joypad->state.direction);
			dmg_joypad_poll(joypad);

			if(!joypad->state.button) {

				for(int index = 0; index < DMG_BUTTON_MAX; ++index) {

					if(joypad->button[index]) {
						joypad->state.raw &= ~(1 << index);
					}
				}
			}

			if(!joypad->state.direction) {

				for(int index = 0; index < DMG_DIRECTION_MAX; ++index) {

					if(joypad->direction[index]) {
						joypad->state.raw &= ~(1 << index);
					}
				}
			}
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported joypad write [%04x]<-%02x", address, value);
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
