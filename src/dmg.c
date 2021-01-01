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

#include "./dmg_type.h"

static bool g_initialized = false;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_action(
	__in const dmg_action_t *request,
	__in dmg_action_t *response
	)
{
	int result;

	if(!g_initialized) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Uninitialized");
		goto exit;
	}

	result = dmg_runtime_action(request, response);

exit:
	return result;
}

int
dmg_load(
	__in const dmg_t *configuration
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(g_initialized) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Reinitialized");
		goto exit;
	}

	g_initialized = ((result = dmg_runtime_load(configuration)) == DMG_STATUS_SUCCESS);

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
	int result = DMG_STATUS_SUCCESS;

	if(!g_initialized) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Uninitialized");
		goto exit;
	}

	result = dmg_runtime_run(breakpoint, count);

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
	int result = DMG_STATUS_SUCCESS;

	if(!g_initialized) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Uninitialized");
		goto exit;
	}

	result = dmg_runtime_step(instructions, breakpoint, count);

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
