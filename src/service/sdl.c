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

static dmg_sdl_t g_sdl = {};

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
	bool result = true;
	SDL_Event event = {};

	g_sdl.end = SDL_GetTicks();

	if((g_sdl.framerate = (g_sdl.end - g_sdl.begin)) >= MS_PER_SEC) {
		g_sdl.framerate = (g_sdl.frame - ((g_sdl.framerate - MS_PER_SEC) / (float)FRAMES));
		g_sdl.framerate = ((g_sdl.framerate > 0.f) ? g_sdl.framerate : 0.f);
#ifndef NDEBUG

		// TODO: DISPLAY FRAMERATE IN DISPLAY

#endif /* NDEBUG */
		g_sdl.begin = g_sdl.end;
		g_sdl.frame = 0;
	}

	while(SDL_PollEvent(&event)) {

		switch(event.type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:

				if(!event.key.repeat) {

					// TODO: HANDLE JOYPAD INPUT
				}
				break;
			case SDL_QUIT:
				result = false;

				TRACE(LEVEL_INFORMATION, "SDL quit event");
				goto exit;
			default:
				break;
		}
	}

exit:
	return result;
}

void
dmg_service_sync(void)
{

	if((g_sdl.frequency = (SDL_GetTicks() - g_sdl.end)) < FRAME_RATE) {
		SDL_Delay(FRAME_RATE - g_sdl.frequency);
	}

	++g_sdl.frame;
}

void
dmg_service_unload(void)
{
	TRACE(LEVEL_INFORMATION, "SDL unloading");

	SDL_Quit();

	TRACE(LEVEL_INFORMATION, "SDL unloaded");

	memset(&g_sdl, 0, sizeof(g_sdl));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
