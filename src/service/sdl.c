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

static int
dmg_service_display_show(void)
{
	int result = ERROR_SUCCESS;

	if(SDL_UpdateTexture(g_sdl.texture, NULL, (dmg_bgra_t *)g_sdl.pixel, DISPLAY_WIDTH * sizeof(dmg_bgra_t))) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_RenderClear(g_sdl.renderer)) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_RenderCopy(g_sdl.renderer, g_sdl.texture, NULL, NULL)) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	SDL_RenderPresent(g_sdl.renderer);

exit:
	return result;
}

static int
dmg_service_display_load(
	__in const dmg_t *configuration,
	__in const char *title
	)
{
	int result;

	for(size_t index = 0; index < DMG_PALETTE_MAX; ++index) {
		g_sdl.palette[index].red = ((configuration->palette[index] & PALETTE_MASK_RED) >> PALETTE_SHIFT_RED);
		g_sdl.palette[index].green = ((configuration->palette[index] & PALETTE_MASK_GREEN) >> PALETTE_SHIFT_GREEN);
		g_sdl.palette[index].blue = ((configuration->palette[index] & PALETTE_MASK_BLUE) >> PALETTE_SHIFT_BLUE);
		g_sdl.palette[index].alpha = UINT8_MAX;

		TRACE_FORMAT(LEVEL_VERBOSE, "Palette[%zu]=%08x (%02x, %02x, %02x)", index, g_sdl.palette[index].raw,
			g_sdl.palette[index].red, g_sdl.palette[index].green, g_sdl.palette[index].blue);
	}

	for(uint32_t y = 0; y < DISPLAY_HEIGHT; ++y) {

		for(uint32_t x = 0; x < DISPLAY_WIDTH; ++x) {
			g_sdl.pixel[x][y].raw = g_sdl.palette[DMG_PALETTE_WHITE].raw;
		}
	}

	g_sdl.scale = configuration->scale;
	if(g_sdl.scale < DISPLAY_SCALE_MIN) {
		g_sdl.scale = DISPLAY_SCALE_MIN;

		TRACE_FORMAT(LEVEL_WARNING, "Scale is too small: %u (setting to %u)", configuration->scale, g_sdl.scale);
	} else if(g_sdl.scale > DISPLAY_SCALE_MAX) {
		g_sdl.scale = DISPLAY_SCALE_MAX;

		TRACE_FORMAT(LEVEL_WARNING, "Scale is too large: %u (setting to %u)", configuration->scale, g_sdl.scale);
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "Scale=%u", g_sdl.scale);

	if(snprintf(g_sdl.title, sizeof(g_sdl.title), "%s -- %s", strlen(title) ? title : DISPLAY_UNTITLED, DISPLAY_TITLE) < 0) {
		memcpy(g_sdl.title, DISPLAY_UNTITLED, strlen(DISPLAY_UNTITLED));
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "Title[%u]=\"%s\"", strlen(g_sdl.title), g_sdl.title);

	if(!(g_sdl.window = SDL_CreateWindow(g_sdl.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH * g_sdl.scale,
			DISPLAY_HEIGHT * g_sdl.scale, SDL_WINDOW_RESIZABLE))) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(!(g_sdl.renderer = SDL_CreateRenderer(g_sdl.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC))) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_RenderSetLogicalSize(g_sdl.renderer, DISPLAY_WIDTH, DISPLAY_HEIGHT)) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_SetRenderDrawColor(g_sdl.renderer, 0, 0, 0, 0)) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0") == SDL_FALSE) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(!(g_sdl.texture = SDL_CreateTexture(g_sdl.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
			DISPLAY_WIDTH, DISPLAY_HEIGHT))) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	result = dmg_service_display_show();

exit:
	return result;
}

static void
dmg_service_display_unload(void)
{

	SDL_DestroyTexture(g_sdl.texture);
	SDL_DestroyRenderer(g_sdl.renderer);
	SDL_DestroyWindow(g_sdl.window);
}

static int
dmg_service_input_load(
	__in const dmg_t *configuration
	)
{
	int result = ERROR_SUCCESS;

	for(size_t index = 0; index < DMG_BUTTON_MAX; ++index) {
		g_sdl.button[index] = configuration->button[index];

		TRACE_FORMAT(LEVEL_VERBOSE, "Button[%zu]=%u", index, g_sdl.button[index]);
	}

	memset(g_sdl.button_state, false, sizeof(bool) * DMG_BUTTON_MAX);

	for(size_t index = 0; index < DMG_DIRECTION_MAX; ++index) {
		g_sdl.direction[index] = configuration->direction[index];

		TRACE_FORMAT(LEVEL_VERBOSE, "Direction[%zu]=%u", index, g_sdl.direction[index]);
	}

	memset(g_sdl.direction_state, false, sizeof(bool) * DMG_DIRECTION_MAX);

	return result;
}

bool
dmg_service_button(
	__in int button
	)
{
	return g_sdl.button_state[button];
}

bool
dmg_service_button_change(
	__in const SDL_Event *event
	)
{
	bool result = false;

	for(int button = 0; button < DMG_BUTTON_MAX; ++button) {

		if((result = (event->key.keysym.scancode == g_sdl.button[button]))) {
			g_sdl.button_state[button] = (event->type == SDL_KEYDOWN);
			break;
		}
	}

	return result;
}

bool
dmg_service_direction(
	__in int direction
	)
{
	return g_sdl.direction_state[direction];
}

bool
dmg_service_direction_change(
	__in const SDL_Event *event
	)
{
	bool result = false;

	for(int direction = 0; direction < DMG_DIRECTION_MAX; ++direction) {

		if((result = (event->key.keysym.scancode == g_sdl.direction[direction]))) {
			g_sdl.direction_state[direction] = (event->type == SDL_KEYDOWN);
			break;
		}
	}

	return result;
}

int
dmg_service_load(
	__in const dmg_t *configuration,
	__in const char *title
	)
{
	int result;
	SDL_version version = {};

	SDL_GetVersion(&version);

	TRACE_FORMAT(LEVEL_INFORMATION, "SDL loading ver.%u.%u.%u", version.major, version.minor, version.patch);

	if(SDL_Init(SDL_INIT_VIDEO)) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if((result = dmg_service_display_load(configuration, title)) != ERROR_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_input_load(configuration)) != ERROR_SUCCESS) {
		goto exit;
	}

	TRACE(LEVEL_INFORMATION, "SDL loaded");

exit:
	return result;
}

void
dmg_service_pixel(
	__in int color,
	__in uint8_t x,
	__in uint8_t y
	)
{
	g_sdl.pixel[x][y].raw = g_sdl.palette[color].raw;
}

int
dmg_service_poll(void)
{
	SDL_Event event = {};
	int result = EVENT_NONE;

	g_sdl.end = SDL_GetTicks();

	if((g_sdl.framerate = (g_sdl.end - g_sdl.begin)) >= MS_PER_SEC) {
		g_sdl.framerate = (g_sdl.frame - ((g_sdl.framerate - MS_PER_SEC) / (float)FRAME_PER_SEC));
		g_sdl.framerate = ((g_sdl.framerate > 0.f) ? g_sdl.framerate : 0.f);
#ifndef NDEBUG
		char title[TITLE_LENGTH_MAX] = {};

		if(snprintf(title, TITLE_LENGTH_MAX, "%s [%.01f fps]", g_sdl.title, g_sdl.framerate) > 0) {
			SDL_SetWindowTitle(g_sdl.window, title);
		}

		TRACE_FORMAT(LEVEL_VERBOSE, "Framerate=%.01f", g_sdl.framerate);
#endif /* NDEBUG */
		g_sdl.begin = g_sdl.end;
		g_sdl.frame = 0;
	}

	while(SDL_PollEvent(&event)) {

		switch(event.type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:

				if(!event.key.repeat) {

					if(dmg_service_button_change(&event)
							|| dmg_service_direction_change(&event)) {
						result |= EVENT_KEY;

						TRACE_FORMAT(LEVEL_VERBOSE, "SDL key %s event=%u", (event.type == SDL_KEYUP) ? "up" : "down",
							event.key.keysym.scancode);
					}
				}
				break;
			case SDL_QUIT:
				result |= EVENT_QUIT;

				TRACE(LEVEL_VERBOSE, "SDL quit event");
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
	dmg_service_display_show();

	if((g_sdl.frequency = (SDL_GetTicks() - g_sdl.end)) < FRAME_RATE) {
		SDL_Delay(FRAME_RATE - g_sdl.frequency);
	}

	++g_sdl.frame;
}

void
dmg_service_unload(void)
{
	TRACE(LEVEL_INFORMATION, "SDL unloading");

	dmg_service_display_unload();
	SDL_Quit();

	TRACE(LEVEL_INFORMATION, "SDL unloaded");

	memset(&g_sdl, 0, sizeof(g_sdl));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
