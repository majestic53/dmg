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

#include "./runtime_type.h"

static dmg_error_t g_error = {};

static dmg_runtime_t g_runtime = {};

static dmg_version_t g_version = { VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH };

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_runtime_load(
	__in const dmg_t *configuration
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Runtime loading");

	if(!configuration) {
		result = SET_ERROR(ERROR_INVALID, "Configuration is NULL");
		goto exit;
	}

	g_runtime.configuration = configuration;

	if(SDL_Init(SDL_INIT_VIDEO)) {
		result = SET_ERROR_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
	}

	// TODO: LOAD SUBSYSTEMS

	TRACE(LEVEL_INFORMATION, "Runtime loaded");

exit:
	return result;
}

static int
dmg_runtime_loop(void)
{
	int result = EXIT_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Runtime loop entry");

	// TODO: LOOP SUBSYSTEMS

	TRACE(LEVEL_INFORMATION, "Runtime loop exit");

	return result;
}

static void
dmg_runtime_unload(void)
{
	TRACE(LEVEL_INFORMATION, "Runtime unloading");

	// TODO: UNLOAD SUBSYSTEMS

	SDL_Quit();

	TRACE(LEVEL_INFORMATION, "Runtime unloaded");

	memset(&g_runtime, 0, sizeof(g_runtime));
}

int
dmg_runtime(
	__in const dmg_t *configuration
	)
{
	int result;

	CLEAR_ERROR();

	if((result = dmg_runtime_load(configuration)) == ERROR_SUCCESS) {
		result = dmg_runtime_loop();
	}

	dmg_runtime_unload();

	return result;
}

const char *
dmg_runtime_error(void)
{
	return g_error.str;
}

int
dmg_runtime_error_set(
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

	if(error != ERROR_SUCCESS) {
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

void
dmg_runtime_interrupt(
	__in int type
	)
{
	// TODO: SEND INTERRUPT TO PROCESSOR SUBSYSTEM
}

uint8_t
dmg_runtime_read(
	__in uint16_t address
	)
{
	uint8_t result = 0;

	switch(address) {

		// TODO: READ BYTE FROM SUBSYSTEM

		default:
			result = UINT8_MAX;
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported read [%04x]->%02x", address, result);
			break;
	}

	return result;
}

#ifndef NDEBUG

void
dmg_runtime_trace(
	__in FILE *stream,
	__in int level,
	__in const char *file,
	__in const char *function,
	__in size_t line,
	__in const char *format,
	...
	)
{
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

	// TODO: CALCULATE ELAPSED

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
	fprintf(stream, "[%s] {%u (%0.2f ms)} %s (%s:%s@%zu)", timestamp, g_runtime.cycle, g_runtime.cycle * MS_PER_CYCLE,
		message, function, file, line);
#ifdef COLOR
	fprintf(stream, "%s", LEVEL_STR[LEVEL_NONE]);
#endif /* COLOR */
	fprintf(stream, "\n");
}

#endif /* NDEBUG */

const dmg_version_t *
dmg_runtime_version(void)
{
	return &g_version;
}

void
dmg_runtime_write(
	__in uint16_t address,
	__in uint8_t value
	)
{

	switch(address) {

		// TODO: WRITE BYTE TO SUBSYSTEM

		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported write [%04x]<-%02x", address, value);
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
