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

	if(g_sdl.display.redraw) {
		g_sdl.display.redraw = false;

		if(SDL_RenderClear(g_sdl.display.renderer)) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
			goto exit;
		}

		if(SDL_UpdateTexture(g_sdl.display.texture, NULL, (dmg_bgra_t *)g_sdl.display.pixel, WINDOW_WIDTH * sizeof(dmg_bgra_t))) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
			goto exit;
		}

		memset(g_sdl.display.pixel_viewport, 0, sizeof(g_sdl.display.pixel_viewport));

		for(uint32_t y = g_sdl.display.viewport_y; y < (VIEWPORT_HEIGHT + g_sdl.display.viewport_y); ++y) {

			for(uint32_t x = g_sdl.display.viewport_x; x < (VIEWPORT_WIDTH + g_sdl.display.viewport_x); ++x) {

				if((x == g_sdl.display.viewport_x)
						|| (x == (VIEWPORT_WIDTH + g_sdl.display.viewport_x - 1))
						|| (y == g_sdl.display.viewport_y)
						|| (y == (VIEWPORT_HEIGHT + g_sdl.display.viewport_y - 1))) {
					g_sdl.display.pixel_viewport[y % WINDOW_HEIGHT][x % WINDOW_WIDTH].raw = COLOR_VIEWPORT.raw;
				}
			}
		}

		if(SDL_UpdateTexture(g_sdl.display.texture_viewport, NULL, &g_sdl.display.pixel_viewport, WINDOW_WIDTH * sizeof(dmg_bgra_t))) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
			goto exit;
		}

		if(g_sdl.display.window_enable) {
			memset(g_sdl.display.pixel_window, 0, sizeof(g_sdl.display.pixel_window));

			for(uint32_t y = (g_sdl.display.viewport_y + g_sdl.display.window_y);
					y < ((g_sdl.display.viewport_y + g_sdl.display.window_y) + (VIEWPORT_HEIGHT - g_sdl.display.window_y)); ++y) {

				for(uint32_t x = (g_sdl.display.viewport_x + g_sdl.display.window_x);
						x < ((g_sdl.display.viewport_x + g_sdl.display.window_x) + (VIEWPORT_WIDTH - g_sdl.display.window_x)); ++x) {
					if((x == (g_sdl.display.viewport_x + g_sdl.display.window_x))
							|| (y == (g_sdl.display.viewport_y + g_sdl.display.window_y))) {
						g_sdl.display.pixel_window[y % WINDOW_HEIGHT][x % WINDOW_WIDTH].raw = COLOR_WINDOW.raw;
					}
				}
			}

			if(SDL_UpdateTexture(g_sdl.display.texture_window, NULL, &g_sdl.display.pixel_window, WINDOW_WIDTH * sizeof(dmg_bgra_t))) {
				result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
				goto exit;
			}
		}
	}

	if(SDL_RenderCopy(g_sdl.display.renderer, g_sdl.display.texture, NULL, NULL)) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(g_sdl.display.window_enable) {

		if(SDL_RenderCopy(g_sdl.display.renderer, g_sdl.display.texture_window, NULL, NULL)) {
			result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
			goto exit;
		}
	}

	if(SDL_RenderCopy(g_sdl.display.renderer, g_sdl.display.texture_viewport, NULL, NULL)) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	SDL_RenderPresent(g_sdl.display.renderer);

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

	for(int index = 0; index < DMG_PALETTE_MAX; ++index) {
		g_sdl.display.palette[index].red = ((configuration->palette[index] & PALETTE_MASK_RED) >> PALETTE_SHIFT_RED);
		g_sdl.display.palette[index].green = ((configuration->palette[index] & PALETTE_MASK_GREEN) >> PALETTE_SHIFT_GREEN);
		g_sdl.display.palette[index].blue = ((configuration->palette[index] & PALETTE_MASK_BLUE) >> PALETTE_SHIFT_BLUE);
		g_sdl.display.palette[index].alpha = UINT8_MAX;
		TRACE_FORMAT(LEVEL_VERBOSE, "Palette[%zu]=%08x (%02x, %02x, %02x)", index, g_sdl.display.palette[index].raw,
			g_sdl.display.palette[index].red, g_sdl.display.palette[index].green, g_sdl.display.palette[index].blue);
	}

	for(uint32_t y = 0; y < WINDOW_HEIGHT; ++y) {

		for(uint32_t x = 0; x < WINDOW_WIDTH; ++x) {
			g_sdl.display.pixel[x][y].raw = g_sdl.display.palette[DMG_PALETTE_WHITE].raw;
		}
	}

	g_sdl.display.scale = configuration->scale;
	if(g_sdl.display.scale < SCALE_MIN) {
		g_sdl.display.scale = SCALE_MIN;
		TRACE_FORMAT(LEVEL_WARNING, "Scale is too small: %u (setting to %u)", configuration->scale, g_sdl.display.scale);
	} else if(g_sdl.display.scale > SCALE_MAX) {
		g_sdl.display.scale = SCALE_MAX;
		TRACE_FORMAT(LEVEL_WARNING, "Scale is too large: %u (setting to %u)", configuration->scale, g_sdl.display.scale);
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "Scale=%u", g_sdl.display.scale);

	if(snprintf(g_sdl.display.title, sizeof(g_sdl.display.title), "%s -- %s", strlen(title) ? title : TITLE_UNTITLED, TITLE) < 0) {
		memcpy(g_sdl.display.title, TITLE_UNTITLED, strlen(TITLE_UNTITLED));
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "Title[%u]=\"%s\"", strlen(g_sdl.display.title), g_sdl.display.title);

	if(!(g_sdl.display.window = SDL_CreateWindow(g_sdl.display.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH * g_sdl.display.scale, WINDOW_HEIGHT * g_sdl.display.scale, SDL_WINDOW_RESIZABLE))) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(!(g_sdl.display.renderer = SDL_CreateRenderer(g_sdl.display.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC))) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_RenderSetLogicalSize(g_sdl.display.renderer, WINDOW_WIDTH, WINDOW_HEIGHT)) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_SetRenderDrawColor(g_sdl.display.renderer, COLOR_BACKGROUND.red, COLOR_BACKGROUND.green, COLOR_BACKGROUND.blue,
			COLOR_BACKGROUND.alpha)) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0") == SDL_FALSE) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(!(g_sdl.display.texture = SDL_CreateTexture(g_sdl.display.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
			WINDOW_WIDTH, WINDOW_HEIGHT))) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(!(g_sdl.display.texture_viewport = SDL_CreateTexture(g_sdl.display.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
			WINDOW_WIDTH, WINDOW_HEIGHT))) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_SetTextureBlendMode(g_sdl.display.texture_viewport, SDL_BLENDMODE_BLEND)) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(!(g_sdl.display.texture_window = SDL_CreateTexture(g_sdl.display.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
			WINDOW_WIDTH, WINDOW_HEIGHT))) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_SetTextureBlendMode(g_sdl.display.texture_window, SDL_BLENDMODE_BLEND)) {
		result = ERROR_SET_FORMAT(ERROR_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	dmg_service_viewport(0, 0);
	dmg_service_window(false, 0, 0);
	result = dmg_service_display_show();

exit:
	return result;
}

static void
dmg_service_display_unload(void)
{
	SDL_DestroyTexture(g_sdl.display.texture_window);
	SDL_DestroyTexture(g_sdl.display.texture_viewport);
	SDL_DestroyTexture(g_sdl.display.texture);
	SDL_DestroyRenderer(g_sdl.display.renderer);
	SDL_DestroyWindow(g_sdl.display.window);
}

static int
dmg_service_input_load(
	__in const dmg_t *configuration
	)
{
	int result = ERROR_SUCCESS;

	for(int index = 0; index < DMG_BUTTON_MAX; ++index) {
		g_sdl.input.button[index] = configuration->button[index];
		TRACE_FORMAT(LEVEL_VERBOSE, "Button[%zu]=%u", index, g_sdl.input.button[index]);
	}

	for(int index = 0; index < DMG_DIRECTION_MAX; ++index) {
		g_sdl.input.direction[index] = configuration->direction[index];
		TRACE_FORMAT(LEVEL_VERBOSE, "Direction[%zu]=%u", index, g_sdl.input.direction[index]);
	}

	return result;
}

bool
dmg_service_button(
	__in int button
	)
{
	return (SDL_GetKeyboardState(NULL)[g_sdl.input.button[button]] > 0);
}

bool
dmg_service_direction(
	__in int direction
	)
{
	return (SDL_GetKeyboardState(NULL)[g_sdl.input.direction[direction]] > 0);
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

bool
dmg_service_poll(void)
{
	bool result = true;
	SDL_Event event = {};

	g_sdl.frame.end = SDL_GetTicks();

	if((g_sdl.frame.rate = (g_sdl.frame.end - g_sdl.frame.begin)) >= MS_PER_SEC) {
		g_sdl.frame.rate = (g_sdl.frame.count - ((g_sdl.frame.rate - MS_PER_SEC) / (float)FRAME_PER_SEC));
		g_sdl.frame.rate = ((g_sdl.frame.rate > 0.f) ? g_sdl.frame.rate : 0.f);
#ifndef NDEBUG
		char title[TITLE_LENGTH_MAX] = {};

		if(snprintf(title, TITLE_LENGTH_MAX, "%s [%.01f fps]", g_sdl.display.title, g_sdl.frame.rate) > 0) {
			SDL_SetWindowTitle(g_sdl.display.window, title);
		}

		TRACE_FORMAT(LEVEL_VERBOSE, "Framerate=%.01f", g_sdl.frame.rate);
#endif /* NDEBUG */
		g_sdl.frame.begin = g_sdl.frame.end;
		g_sdl.frame.count = 0;
	}

	while(SDL_PollEvent(&event)) {

		switch(event.type) {
			case SDL_WINDOWEVENT:
				g_sdl.display.redraw = true;
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
dmg_service_sync(void)
{
	dmg_service_display_show();

	if((g_sdl.frame.frequency = (SDL_GetTicks() - g_sdl.frame.end)) < FRAME_RATE) {
		SDL_Delay(FRAME_RATE - g_sdl.frame.frequency);
	}

	++g_sdl.frame.count;
}

void
dmg_service_tile(
	__in uint8_t *tile,
	__in uint8_t x,
	__in uint8_t y
	)
{
	g_sdl.display.redraw = true;

	for(uint32_t ty = 0; ty < TILE_HEIGHT; ++ty) {

		for(uint32_t tx = 0; tx < TILE_WIDTH; ++tx) {
			g_sdl.display.pixel[(TILE_HEIGHT * y) + ty][(TILE_WIDTH * x) + tx].raw
				= g_sdl.display.palette[tile[(TILE_WIDTH * ty) + tx]].raw;
		}
	}
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

void
dmg_service_viewport(
	__in uint8_t x,
	__in uint8_t y
	)
{

	if((x != g_sdl.display.viewport_x) || (y != g_sdl.display.viewport_y)) {
		g_sdl.display.redraw = true;
		g_sdl.display.viewport_x = x;
		g_sdl.display.viewport_y = y;
	}
}

void
dmg_service_window(
	__in bool enable,
	__in uint8_t x,
	__in uint8_t y
	)
{

	if((enable != g_sdl.display.window_enable) || (x != g_sdl.display.window_x) || (y != g_sdl.display.window_y)) {
		g_sdl.display.redraw = true;
		g_sdl.display.window_enable = enable;
		g_sdl.display.window_x = x;
		g_sdl.display.window_y = y;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
