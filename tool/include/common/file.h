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

#ifndef DMG_TOOL_COMMON_FILE_H_
#define DMG_TOOL_COMMON_FILE_H_

#include "./define.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_tool_file_open(
	__in const char *path,
	__in bool read_only,
	__in bool allow_empty,
	__out FILE **file,
	__out int *length
	);

void dmg_tool_file_close(
	__inout FILE **file
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TOOL_COMMON_FILE_H_ */
