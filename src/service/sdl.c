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

#include "./sdl_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_service_load(void)
{
	SDL_version version = {};
	int result = ERROR_SUCCESS;

	SDL_GetVersion(&version);
	TRACE_FORMAT(LEVEL_INFORMATION, "SDL loading ver.%u.%u.%u",
		version.major, version.minor, version.patch);

	if(SDL_Init(SDL_INIT_VIDEO)) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	TRACE(LEVEL_INFORMATION, "SDL loaded");

exit:
	return result;
}

bool
dmg_service_poll(void)
{
	// TODO
	return false;
	// ---
}

void
dmg_service_unload(void)
{
	TRACE(LEVEL_INFORMATION, "SDL unloading");

	SDL_Quit();

	TRACE(LEVEL_INFORMATION, "SDL unloaded");
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
