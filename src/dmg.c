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

int
dmg_load(
	__in const dmg_t *configuration
	)
{

	if(!g_initialized) {
		g_initialized = (dmg_runtime_load(configuration) == ERROR_SUCCESS);
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
	}

	return (int)result;
}

unsigned
dmg_serial_in(
	__in unsigned in
	)
{
	return (g_initialized ? dmg_runtime_serial_in(in) : 1);
}

int
dmg_step(void)
{
	bool result = false;

	if(g_initialized) {
		result = dmg_runtime_step();
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
