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

#ifndef DMG_COMMON_ERROR_H_
#define DMG_COMMON_ERROR_H_

#include "./define.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

const char *dmg_error_get(void);

int dmg_error_set(
	__in int error,
	__in const char *file,
	__in const char *function,
	__in size_t line,
	__in const char *format,
	...
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_COMMON_ERROR_H_ */
