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

#include "./sdl_type.h"

static dmg_sdl_t g_sdl = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_sdl_display_show(void)
{
	int result = ERROR_SUCCESS;

	if(g_sdl.redraw) {
		g_sdl.redraw = false;

		if(SDL_RenderClear(g_sdl.renderer)) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
			goto exit;
		}

		if(SDL_UpdateTexture(g_sdl.texture, NULL, (dmg_sdl_bgra_t *)g_sdl.pixel, WINDOW_WIDTH * sizeof(dmg_sdl_bgra_t))) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
			goto exit;
		}
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
dmg_sdl_display_load(
	__in const dmg_t *configuration,
	__in const char *title
	)
{
	int result;

	for(int index = 0; index < DMG_PALETTE_MAX; ++index) {
		g_sdl.palette[index].red = ((configuration->palette[index] & PALETTE_MASK_RED) >> PALETTE_SHIFT_RED);
		g_sdl.palette[index].green = ((configuration->palette[index] & PALETTE_MASK_GREEN) >> PALETTE_SHIFT_GREEN);
		g_sdl.palette[index].blue = ((configuration->palette[index] & PALETTE_MASK_BLUE) >> PALETTE_SHIFT_BLUE);
		g_sdl.palette[index].alpha = UINT8_MAX;
		TRACE_FORMAT(LEVEL_VERBOSE, "SDL palette[%i]=%08x (%02x, %02x, %02x)", index, g_sdl.palette[index].raw,
			g_sdl.palette[index].red, g_sdl.palette[index].green, g_sdl.palette[index].blue);
	}

	g_sdl.scale = configuration->scale;
	if(g_sdl.scale < SCALE_MIN) {
		g_sdl.scale = SCALE_MIN;
		TRACE_FORMAT(LEVEL_WARNING, "Scale is too small: %u (setting to %u)", configuration->scale, g_sdl.scale);
	} else if(g_sdl.scale > SCALE_MAX) {
		g_sdl.scale = SCALE_MAX;
		TRACE_FORMAT(LEVEL_WARNING, "Scale is too large: %u (setting to %u)", configuration->scale, g_sdl.scale);
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "SDL scale=%u", g_sdl.scale);

	if(snprintf(g_sdl.title, sizeof(g_sdl.title), "%s -- %s", strlen(title) ? title : TITLE_UNTITLED, TITLE) < 0) {
		memcpy(g_sdl.title, TITLE_UNTITLED, strlen(TITLE_UNTITLED));
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "SDL title[%u]=\"%s\"", strlen(g_sdl.title), g_sdl.title);

	if(!(g_sdl.window = SDL_CreateWindow(g_sdl.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH * g_sdl.scale, WINDOW_HEIGHT * g_sdl.scale, SDL_WINDOW_RESIZABLE))) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(!(g_sdl.renderer = SDL_CreateRenderer(g_sdl.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC))) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_RenderSetLogicalSize(g_sdl.renderer, WINDOW_WIDTH, WINDOW_HEIGHT)) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_SetRenderDrawColor(g_sdl.renderer, COLOR_BACKGROUND.red, COLOR_BACKGROUND.green, COLOR_BACKGROUND.blue,
			COLOR_BACKGROUND.alpha)) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0") == SDL_FALSE) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(!(g_sdl.texture = SDL_CreateTexture(g_sdl.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
			WINDOW_WIDTH, WINDOW_HEIGHT))) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	result = dmg_sdl_display_show();

exit:
	return result;
}

static void
dmg_sdl_display_unload(void)
{
	SDL_DestroyTexture(g_sdl.texture);
	SDL_DestroyRenderer(g_sdl.renderer);
	SDL_DestroyWindow(g_sdl.window);
}

bool
dmg_sdl_button(
	__in int button
	)
{
	return (SDL_GetKeyboardState(NULL)[button] > 0);
}

bool
dmg_sdl_direction(
	__in int direction
	)
{
	return (SDL_GetKeyboardState(NULL)[direction] > 0);
}

int
dmg_sdl_load(
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

	if((result = dmg_sdl_display_load(configuration, title)) != ERROR_SUCCESS) {
		goto exit;
	}

	TRACE(LEVEL_INFORMATION, "SDL loaded");

exit:
	return result;
}

void
dmg_sdl_pixel(
	__in uint8_t color,
	__in uint8_t x,
	__in uint8_t y
	)
{
	g_sdl.redraw = true;
	g_sdl.pixel[y][x].raw = g_sdl.palette[color].raw;
}

bool
dmg_sdl_poll(
	__in bool complete,
	__in float rate
	)
{
	bool result = true;
	SDL_Event event = {};

#ifndef NDEBUG

	if(complete) {
		char title[TITLE_LENGTH_MAX] = {};

		if(snprintf(title, TITLE_LENGTH_MAX, "%s [%.01f fps]", g_sdl.title, rate) > 0) {
			SDL_SetWindowTitle(g_sdl.window, title);
		}

		TRACE_FORMAT(LEVEL_VERBOSE, "SDL framerate=%.01f", rate);
	}
#endif /* NDEBUG */

	while(SDL_PollEvent(&event)) {

		switch(event.type) {
			case SDL_WINDOWEVENT:
				g_sdl.redraw = true;
				break;
			case SDL_QUIT:
				result = false;
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
dmg_sdl_sync(void)
{
	dmg_sdl_display_show();
}

void
dmg_sdl_unload(void)
{
	TRACE(LEVEL_INFORMATION, "SDL unloading");
	dmg_sdl_display_unload();
	SDL_Quit();
	TRACE(LEVEL_INFORMATION, "SDL unloaded");
	memset(&g_sdl, 0, sizeof(g_sdl));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
