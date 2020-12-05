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

#include "./error_type.h"

static dmg_error_t g_error = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

const char *
dmg_error_get(void)
{
	return g_error.str;
}

int
dmg_error_set(
	__in int error,
	__in const char *file,
	__in const char *function,
	__in size_t line,
	__in const char *format,
	...
	)
{
	int result = error;

	memset(&g_error, 0, sizeof(g_error));

	if(error != DMG_STATUS_SUCCESS) {
		char message[ERROR_LENGTH_MAX / 2] = {};

		if(format) {
			va_list arguments;

			va_start(arguments, format);

			if(vsnprintf(message, ERROR_LENGTH_MAX / 2, format, arguments) < 0) {
				memcpy(message, ERROR_MALFORMED, strlen(ERROR_MALFORMED));
			}

			va_end(arguments);
		}

#ifndef NDEBUG
		snprintf(g_error.str, ERROR_LENGTH_MAX, "%s (%s:%s@%zu)", message, function, file, line);
#else
		snprintf(g_error.str, ERROR_LENGTH_MAX, "%s", message);
#endif /* NDEBUG */
		TRACE_ERROR(message, file, function, line);
	}

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
