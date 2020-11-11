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

#include "./file_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void dmg_service_close(
	__in FILE *file
	)
{

	if(file) {
		fclose(file);
		TRACE_FORMAT(LEVEL_INFORMATION, "File closed: %p", file);
	}
}

FILE *
dmg_service_open(
	__in const char *path,
	__in const char *mode
	)
{
	FILE *result;

	TRACE_FORMAT(LEVEL_INFORMATION, "File opening: %s", path);

	if((result = fopen(path, mode))) {
		TRACE_FORMAT(LEVEL_INFORMATION, "File opened: %p", result);
	}

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
