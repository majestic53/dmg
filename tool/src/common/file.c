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

#include "./file_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_tool_file_open(
	__in const char *path,
	__in bool read_only,
	__in bool allow_empty,
	__out FILE **file,
	__out int *length
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!(*file = fopen(path, read_only ? "rb" : "wb"))) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Failed to open file: %s", path);
		goto exit;
	}

	fseek(*file, 0, SEEK_END);
	*length = ftell(*file);
	fseek(*file, 0, SEEK_SET);

	if(*length < 0) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Malformed file: %s", path);
		goto exit;
	} else if(read_only && !allow_empty && !*length) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Empty file: %s", path);
		goto exit;
	}

exit:

	if(result != DMG_STATUS_SUCCESS) {
		dmg_tool_file_close(file);
		*length = 0;
	}

	return result;
}

void
dmg_tool_file_close(
	__inout FILE **file
	)
{

	if(*file) {
		fclose(*file);
		*file = NULL;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
