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

#include "./dmg_type.h"

static bool g_initialized = false;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_action_serial_in(
	__in const dmg_action_t *request,
	__in dmg_action_t *response
	)
{
	response->data.u8 = dmg_runtime_serial_in(request->data.u8);
	response->length = sizeof(request->data.u8);

	return DMG_SUCCESS;
}

static const dmg_action_hdlr ACTION_HANDLER[] = {
	dmg_action_serial_in, /* DMG_ACTION_SERIAL_IN */
	};

int
dmg_action(
	__in const dmg_action_t *request,
	__in dmg_action_t *response
	)
{
	int result;

	if(!g_initialized) {
		ERROR_SET(ERROR_FAILURE, "Uninitialized");
		result = DMG_FAILURE;
		goto exit;
	} else if(request->type >= DMG_ACTION_MAX) {
		ERROR_SET_FORMAT(ERROR_INVALID, "Unsupported action [%u]->%u", request->id, request->type);
		result = DMG_INVALID;
		goto exit;
	}

	memset(response, 0, sizeof(*response));

	if((result = ACTION_HANDLER[request->type](request, response)) == DMG_SUCCESS) {
		response->id = request->id;
		response->type = request->type;
	}

exit:
	return result;
}

int
dmg_load(
	__in const dmg_t *configuration
	)
{
	int result = DMG_SUCCESS;

	if(g_initialized) {
		ERROR_SET(ERROR_FAILURE, "Reinitialized");
		result = DMG_FAILURE;
		goto exit;
	}

	g_initialized = (dmg_runtime_load(configuration) == ERROR_SUCCESS);

exit:
	return result;
}

const char *
dmg_error(void)
{
	return dmg_error_get();
}

int
dmg_run(
	__in const unsigned short *breakpoint,
	__in unsigned count
	)
{
	int result = DMG_SUCCESS;

	if(!g_initialized) {
		ERROR_SET(ERROR_FAILURE, "Uninitialized");
		result = DMG_FAILURE;
		goto exit;
	} else if(!breakpoint && count) {
		ERROR_SET_FORMAT(ERROR_FAILURE, "Invalid parameter: breakpoint[%u]=%p", count, breakpoint);
		result = DMG_INVALID;
		goto exit;
	}

	result = ((dmg_runtime_run(breakpoint, count) == ERROR_SUCCESS) ? DMG_SUCCESS : DMG_BREAKPOINT);

exit:
	return result;
}

int
dmg_step(
	__in unsigned instructions,
	__in const unsigned short *breakpoint,
	__in unsigned count
	)
{
	int result = DMG_SUCCESS;

	if(!g_initialized) {
		ERROR_SET(ERROR_FAILURE, "Uninitialized");
		result = DMG_FAILURE;
		goto exit;
	} else if(!instructions) {
		ERROR_SET_FORMAT(ERROR_FAILURE, "Invalid parameter: instructions=%u", instructions);
		result = DMG_INVALID;
		goto exit;
	} else if(!breakpoint && count) {
		ERROR_SET_FORMAT(ERROR_FAILURE, "Invalid parameter: breakpoint[%u]=%p", count, breakpoint);
		result = DMG_INVALID;
		goto exit;
	}

	result = ((dmg_runtime_step(instructions, breakpoint, count) == ERROR_SUCCESS) ? DMG_SUCCESS : DMG_BREAKPOINT);

exit:
	return result;
}

void
dmg_unload(void)
{

	if(g_initialized) {
		g_initialized = false;
		dmg_runtime_unload();
	}
}

const dmg_version_t *
dmg_version(void)
{
	return dmg_version_get();
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
