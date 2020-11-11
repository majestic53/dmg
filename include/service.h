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

#ifndef DMG_SERVICE_H_
#define DMG_SERVICE_H_

#include "./common.h"

typedef int (*dmg_service_handler)(FILE *);

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool dmg_service_button(
	__in int button
	);

void dmg_service_close(
	__in FILE *file
	);

bool dmg_service_direction(
	__in int direction
	);

int dmg_service_export(
	__in dmg_service_handler handler,
	__in const char *path
	);

int dmg_service_export_data(
	__in FILE *file,
	__in const void *data,
	__in uint32_t length
	);

int dmg_service_import(
	__in dmg_service_handler handler,
	__in const char *path
	);

int dmg_service_import_data(
	__in FILE *file,
	__in void *data,
	__in uint32_t length
	);

int dmg_service_load(
	__in const dmg_t *configuration,
	__in const char *title
	);

FILE *dmg_service_open(
	__in const char *path,
	__in const char *mode
	);

void dmg_service_pixel(
	__in uint8_t color,
	__in uint8_t x,
	__in uint8_t y
	);

bool dmg_service_poll(void);

void dmg_service_sync(void);

void dmg_service_unload(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_SERVICE_H_ */
