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

#include "./buffer_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_buffer_allocate(
	__inout dmg_buffer_t *buffer,
	__in uint32_t length,
	__in uint8_t value
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!(buffer->data = (void *)malloc(length))) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to allocate buffer");
		goto exit;
	}

	memset(buffer->data, value, length);
	buffer->length = length;

exit:
	return result;
}

void
dmg_buffer_free(
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
