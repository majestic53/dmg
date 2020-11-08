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

#include "../include/file.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_file_load(
	__inout dmg_buffer_t *buffer,
	__in const char *path
	)
{
	FILE *file = NULL;
	int length, result = EXIT_SUCCESS;

	if(!(file = fopen(path, "rb"))) {
		result = EXIT_FAILURE;
		goto exit;
	}

	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	if(length <= 0) {
		result = EXIT_FAILURE;
		goto exit;
	}

	if(!(buffer->data = (void *)malloc(length))) {
		result = EXIT_FAILURE;
		goto exit;
	}

	if(fread(buffer->data, sizeof(uint8_t), length, file) != length) {
		result = EXIT_FAILURE;
		goto exit;
	}

	buffer->length = length;

exit:

	if(file) {
		fclose(file);
		file = NULL;
	}

	return result;
}

void
dmg_file_unload(
	__inout dmg_buffer_t *buffer
	)
{

	if(buffer->data) {
		free(buffer->data);
	}

	memset(buffer, 0, sizeof(*buffer));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
