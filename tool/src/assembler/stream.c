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

#include "./stream_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_assembler_stream_load(
	__inout dmg_assembler_stream_t *stream,
	__in const dmg_buffer_t *buffer,
	__in const char *path
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!buffer || !buffer->data) {
		result = ERROR_SET(DMG_STATUS_INVALID, "Invalid buffer");
		goto exit;
	} else if(!path) {
		result = ERROR_SET(DMG_STATUS_INVALID, "Invalid path");
		goto exit;
	}

	stream->buffer = buffer;
	stream->path = path;

exit:
	return result;
}

char
dmg_assembler_stream_character(
	__inout dmg_assembler_stream_t *stream,
	__out int *type
	)
{
	char result = ((char *)stream->buffer->data)[stream->index];

	if(result == CHARACTER_EOF) {
		*type |= CHARACTER_END;
	} else if(isalpha(result)) {
		*type |= CHARACTER_ALPHA;

		if(isxdigit(result)) {
			*type |= CHARACTER_HEXIDECIMAL;
		}
	} else if(isdigit(result)) {
		*type |= CHARACTER_DECIMAL;

		if(isxdigit(result)) {
			*type |= CHARACTER_HEXIDECIMAL;
		}
	} else if(isspace(result)) {
		*type |= CHARACTER_SPACE;
	} else {
		*type |= CHARACTER_SYMBOL;
	}

	return result;
}

int
dmg_assembler_stream_next(
	__inout dmg_assembler_stream_t *stream
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(stream->index == stream->buffer->length) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "No next character %u", stream->index);
		goto exit;
	}

	if(((char *)stream->buffer->data)[stream->index++] == CHARACTER_NEWLINE) {
		++stream->line;
	}

exit:
	return result;
}

int
dmg_assembler_stream_previous(
	__inout dmg_assembler_stream_t *stream
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!stream->index) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "No previous character %u", stream->index);
		goto exit;
	}

	if(((char *)stream->buffer->data)[--stream->index] == CHARACTER_NEWLINE) {
		--stream->line;
	}

exit:
	return result;
}

void
dmg_assembler_stream_unload(
	__inout dmg_assembler_stream_t *stream
	)
{
	memset(stream, 0, sizeof(*stream));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
