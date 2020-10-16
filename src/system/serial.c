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

#include "./serial_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NDEBUG

static void
dmg_serial_trace(
	__in int level,
	__inout dmg_serial_t *serial
	)
{
	TRACE_FORMAT(level, "Serial SC=%02x [Select=%u, Enable=%x]", serial->control.raw,
		serial->control.select, serial->control.enable);
	TRACE_FORMAT(level, "Serial SD=%02x", serial->data);
}

#endif /* NDEBUG */

int
dmg_serial_load(
	__inout dmg_serial_t *serial,
	__in const dmg_t *configuration
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Serial loading");

	if(!configuration->bootrom.data) {
		serial->control.raw = POST_CONTROL;
		serial->data.raw = POST_DATA;
	}

	serial->transfer = configuration->transfer;

	TRACE_SERIAL(LEVEL_VERBOSE, serial);
	TRACE(LEVEL_INFORMATION, "Serial loaded");

	return result;
}

uint8_t
dmg_serial_read(
	__in const dmg_serial_t *serial,
	__in uint16_t address
	)
{
	uint8_t result = 0;

	switch(address) {
		case ADDRESS_SERIAL_CONTROL:
			result = (serial->control.raw & CONTROL_MASK);
			break;
		case ADDRESS_SERIAL_DATA:
			result = serial->data.raw;
			break;
		default:
			result = UINT8_MAX;

			TRACE_FORMAT(LEVEL_WARNING, "Unsupported serial read [%04x]->%02x", address, result);
			break;
	}

	return result;
}

void
dmg_serial_step(
	__inout dmg_serial_t *serial,
	__in uint32_t cycle
	)
{

	if(serial->control.enable) {

		for(uint32_t tick = 0; tick < cycle; tick += CYCLE) {
			TRACE_SERIAL(LEVEL_VERBOSE, serial);

			serial->cycle += CYCLE;
			if(serial->cycle >= SELECT_CYC[serial->control.select]) {
				dmg_sb_t in = {};

				serial->cycle %= SELECT_CYC[serial->control.select];
				in.lsb = true;

				if(serial->transfer) {
					in.lsb = serial->transfer(serial->data.msb);
				}

				serial->data.raw = ((serial->data.raw << 1) | in.lsb);

				if(!--serial->remaining) {
					serial->control.enable = false;
					dmg_runtime_interrupt(INTERRUPT_SERIAL);

					TRACE(LEVEL_VERBOSE, "Serial transfer complete");
					break;
				}
			}
		}
	}
}

void
dmg_serial_unload(
	__inout dmg_serial_t *serial
	)
{
	TRACE(LEVEL_INFORMATION, "Serial unloading");

	memset(serial, 0, sizeof(*serial));

	TRACE(LEVEL_INFORMATION, "Serial unloaded");
}

void
dmg_serial_write(
	__inout dmg_serial_t *serial,
	__in uint16_t address,
	__in uint8_t value
	)
{

	switch(address) {
		case ADDRESS_SERIAL_CONTROL:
			serial->control.raw = (value & CONTROL_MASK);
			serial->cycle = 0;
			serial->remaining = CHAR_BIT;

			TRACE_FORMAT(LEVEL_VERBOSE, "Serial select=%u, enable=%x", serial->control.select, serial->control.enable);
			break;
		case ADDRESS_SERIAL_DATA:
			serial->data.raw = value;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported serial write [%04x]<-%02x", address, value);
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
