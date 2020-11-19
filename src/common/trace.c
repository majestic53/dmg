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

#include "./trace_type.h"

#ifndef NDEBUG
static dmg_trace_t g_trace = {};
#endif /* NDEBUG */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NDEBUG

void
dmg_trace(
	__in FILE *stream,
	__in int level,
	__in const char *file,
	__in const char *function,
	__in size_t line,
	__in const char *format,
	...
	)
{

	if(g_trace.enable) {
		time_t current = time(NULL);
		char message[TRACE_LENGTH_MAX] = {}, timestamp[TIMESTAMP_LENGTH_MAX] = {};

		if(!stream) {

			switch(level) {
				case LEVEL_ERROR:
				case LEVEL_WARNING:
					stream = stderr;
					break;
				default:
					stream = stdout;
					break;
			}
		}

		if(!strftime(timestamp, TIMESTAMP_LENGTH_MAX, TIMESTAMP_FORMAT, localtime(&current))) {
			memcpy(timestamp, TIMESTAMP_MALFORMED, strlen(TIMESTAMP_MALFORMED));
		}

		if(format) {
			va_list arguments;

			va_start(arguments, format);

			if(vsnprintf(message, TRACE_LENGTH_MAX, format, arguments) < 0) {
				memcpy(message, TRACE_MALFORMED, strlen(TRACE_MALFORMED));
			}

			va_end(arguments);
		}

#ifdef COLOR
		fprintf(stream, "%s", LEVEL_STR[level]);
#endif /* COLOR */
		if(g_trace.cycle) {
			fprintf(stream, "[%s] {%u (%0.2f ms)} %s (%s:%s@%zu)", timestamp, *g_trace.cycle, *g_trace.cycle * MS_PER_CYCLE,
				message, function, file, line);
		} else {
			fprintf(stream, "[%s] %s (%s:%s@%zu)", timestamp, message, function, file, line);
		}
#ifdef COLOR
		fprintf(stream, "%s", LEVEL_STR[LEVEL_NONE]);
#endif /* COLOR */
		fprintf(stream, "\n");
	}
}

void
dmg_trace_enable(
	__in bool enable,
	__in uint32_t *cycle
	)
{
	g_trace.enable = enable;
	g_trace.cycle = cycle;
}

#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif /* __cplusplus */
