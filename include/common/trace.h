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

#ifndef DMG_COMMON_TRACE_H_
#define DMG_COMMON_TRACE_H_

#include "./define.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NDEBUG

void dmg_trace(
	__in FILE *stream,
	__in int level,
	__in const char *file,
	__in const char *function,
	__in size_t line,
	__in const char *format,
	...
	);

void dmg_trace_enable(
	__in bool enable,
	__in dmg_cycle_t *cycle
	);

#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_COMMON_TRACE_H_ */
