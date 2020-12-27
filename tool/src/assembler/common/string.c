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

#include "./string_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_assembler_string_reallocate(
	__inout dmg_assembler_string_t *string
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((string->str = (char *)realloc(string->str, sizeof(char) * string->capacity * STRING_CAPACITY_SCALE)) == NULL) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to reallocate string buffer");
		goto exit;
	}

	memset(&(string->str[string->capacity]), 0, sizeof(char) * ((string->capacity * STRING_CAPACITY_SCALE) - string->capacity));
	string->capacity *= STRING_CAPACITY_SCALE;

exit:
	return result;
}

int
dmg_assembler_string_allocate(
	__inout dmg_assembler_string_t *string
	)
{
	int result = DMG_STATUS_SUCCESS;

	dmg_assembler_string_free(string);

	if((string->str = (char *)calloc(STRING_CAPACITY_INIT, sizeof(char))) == NULL) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to allocate string buffer");
		goto exit;
	}

	string->capacity = STRING_CAPACITY_INIT;
	string->length = 0;

exit:
	return result;
}

int
dmg_assembler_string_append(
	__inout dmg_assembler_string_t *string,
	__in char value
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(((string->length + 1) == string->capacity)
			&& ((result = dmg_assembler_string_reallocate(string)) != DMG_STATUS_SUCCESS)) {
		goto exit;
	}

	string->str[string->length++] = value;

exit:
	return result;
}

void
dmg_assembler_string_free(
	__inout dmg_assembler_string_t *string
	)
{

	if(string->str) {
		free(string->str);
	}

	memset(string, 0, sizeof(*string));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
