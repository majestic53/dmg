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

static bool
dmg_action_serial_in(
	__in const dmg_request_t *request,
	__in dmg_response_t *response
	)
{
	response->data.u8 = dmg_runtime_serial_in(request->data.u8);
	response->action.length = sizeof(request->data.u8);

	return true;
}

static const dmg_action_hdlr ACTION_HANDLER[] = {
	dmg_action_serial_in, /* DMG_ACTION_SERIAL_IN */
	};

int
dmg_action(
	__in const dmg_request_t *request,
	__in dmg_response_t *response
	)
{
	bool result = true;

	if(g_initialized) {

		if(request->action.type >= DMG_ACTION_MAX) {
			ERROR_SET_FORMAT(ERROR_INVALID, "Unsupported action [%u]->%u", request->action.id, request->action.type);
			result = false;
		}

		memset(response, 0, sizeof(*response));
		result = ACTION_HANDLER[request->action.type](request, response);
	} else {
		ERROR_SET(ERROR_INVALID, "DMG uninitialized");
		result = false;
	}

	if(result) {
		response->action.id = request->action.id;
		response->action.type = request->action.type;
	}

	return (int)result;
}

int
dmg_load(
	__in const dmg_t *configuration
	)
{

	if(!g_initialized) {
		g_initialized = (dmg_runtime_load(configuration) == ERROR_SUCCESS);
	} else {
		ERROR_SET(ERROR_INVALID, "DMG reinitialized");
	}

	return (int)g_initialized;
}

const char *
dmg_error(void)
{
	return dmg_error_get();
}

int
dmg_run(void)
{
	bool result = false;

	if(g_initialized) {
		result = dmg_runtime_run();
	} else {
		ERROR_SET(ERROR_INVALID, "DMG uninitialized");
	}

	return (int)result;
}

int
dmg_step(void)
{
	bool result = false;

	if(g_initialized) {
		result = dmg_runtime_step();
	} else {
		ERROR_SET(ERROR_INVALID, "DMG uninitialized");
	}

	return (int)result;
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
