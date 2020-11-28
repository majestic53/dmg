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

#include "./serial_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NDEBUG

static void
dmg_serial_trace(
	__in int level,
	__inout const dmg_serial_t *serial
	)
{
	TRACE_FORMAT(level, "Serial control=%02x [Select=%u, Enable=%x]", serial->control.raw,
		serial->control.select, serial->control.enable);
	TRACE_FORMAT(level, "Serial data=%02x", serial->data.raw);
}

#endif /* NDEBUG */

int
dmg_serial_export(
	__in const dmg_serial_t *serial,
	__in FILE *file
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Serial exporting");
	TRACE_SERIAL(LEVEL_VERBOSE, serial);

	if((result = dmg_service_export_data(file, &serial->control, sizeof(serial->control))) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &serial->cycle, sizeof(serial->cycle))) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &serial->data, sizeof(serial->data))) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &serial->remaining, sizeof(serial->remaining))) != ERROR_SUCCESS) {
		goto exit;
	}

	TRACE(LEVEL_INFORMATION, "Serial exported");

exit:
	return result;
}

int
dmg_serial_import(
	__inout dmg_serial_t *serial,
	__in FILE *file
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Serial importing");

	if((result = dmg_service_import_data(file, &serial->control, sizeof(serial->control))) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &serial->cycle, sizeof(serial->cycle))) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &serial->data, sizeof(serial->data))) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &serial->remaining, sizeof(serial->remaining))) != ERROR_SUCCESS) {
		goto exit;
	}

	TRACE_SERIAL(LEVEL_VERBOSE, serial);
	TRACE(LEVEL_INFORMATION, "Serial imported");

exit:
	return result;
}

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

	serial->out = configuration->out;
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

	if(serial->control.enable && (serial->control.select == SELECT_INTERNAL)) {

		for(uint32_t tick = 0; tick < cycle; tick += CYCLE) {
			TRACE_SERIAL(LEVEL_VERBOSE, serial);

			if((serial->cycle += CYCLE) >= SELECT_CYC[serial->control.select]) {
				dmg_serial_data_t in = {};

				serial->cycle %= SELECT_CYC[serial->control.select];
				in.lsb = true;

				if(serial->out) {
					in.lsb = serial->out(serial->data.msb);
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

dmg_serial_data_t
dmg_serial_transfer(
	__inout dmg_serial_t *serial,
	__in dmg_serial_data_t in
	)
{
	dmg_serial_data_t out = {};

	if(serial->control.enable && (serial->control.select == SELECT_EXTERNAL)) {
		out.lsb = serial->data.msb;
		serial->data.raw = ((serial->data.raw << 1) | in.lsb);

		if(!--serial->remaining) {
			serial->control.enable = false;
			dmg_runtime_interrupt(INTERRUPT_SERIAL);
			TRACE(LEVEL_VERBOSE, "Serial transfer complete");
		}
	}

	return out;
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
