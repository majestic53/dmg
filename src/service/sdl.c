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

#ifdef SDL

#include "./sdl_type.h"

static dmg_sdl_t g_sdl = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_sdl_audio_load(void)
{
	int result = DMG_STATUS_SUCCESS;
	SDL_AudioSpec specification = {};

	TRACE(LEVEL_INFORMATION, "SDL audio loading");

	specification.freq = AUDIO_FREQUENCY;
	specification.format = AUDIO_FORMAT;
	specification.channels = AUDIO_CHANNELS;
	specification.samples = AUDIO_SAMPLES;

	g_sdl.audio.device = SDL_OpenAudioDevice(NULL, AUDIO_DIRECTION, &specification, &g_sdl.audio.specification, 0);
	if(!g_sdl.audio.device) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	SDL_PauseAudioDevice(g_sdl.audio.device, AUDIO_PLAY);

	TRACE(LEVEL_INFORMATION, "SDL audio loaded");

exit:
	return result;
}

static void
dmg_sdl_audio_unload(void)
{
	TRACE(LEVEL_INFORMATION, "SDL audio unloading");

	SDL_PauseAudioDevice(g_sdl.audio.device, AUDIO_PAUSE);
	SDL_CloseAudioDevice(g_sdl.audio.device);

	TRACE(LEVEL_INFORMATION, "SDL audio unloaded");
}

static int
dmg_sdl_display_fullscreen(void)
{
	int result = DMG_STATUS_SUCCESS;
	bool fullscreen = !g_sdl.video.fullscreen;

	if(SDL_SetWindowFullscreen(g_sdl.video.window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0)) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	SDL_ShowCursor(fullscreen ? SDL_DISABLE : SDL_ENABLE);
	g_sdl.video.fullscreen = fullscreen;
	TRACE_FORMAT(LEVEL_INFORMATION, "SDL display %s", g_sdl.video.fullscreen ? "fullscreen" : "windowed");

exit:
	return result;
}

static int
dmg_sdl_display_show(void)
{
	int result = DMG_STATUS_SUCCESS;

	if(g_sdl.video.redraw) {
		g_sdl.video.redraw = false;

		if(SDL_RenderClear(g_sdl.video.renderer)) {
			result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s", SDL_GetError());
			goto exit;
		}

		if(SDL_UpdateTexture(g_sdl.video.texture, NULL, (dmg_sdl_bgra_t *)g_sdl.video.pixel, WINDOW_WIDTH * sizeof(dmg_sdl_bgra_t))) {
			result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s", SDL_GetError());
			goto exit;
		}

		if(SDL_RenderCopy(g_sdl.video.renderer, g_sdl.video.texture, NULL, NULL)) {
			result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s", SDL_GetError());
			goto exit;
		}
	}

	SDL_RenderPresent(g_sdl.video.renderer);

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

	TRACE(LEVEL_INFORMATION, "SDL display loading");

	for(int index = 0; index < DMG_PALETTE_MAX; ++index) {
		g_sdl.video.palette[index].red = ((configuration->palette[index] & PALETTE_MASK_RED) >> PALETTE_SHIFT_RED);
		g_sdl.video.palette[index].green = ((configuration->palette[index] & PALETTE_MASK_GREEN) >> PALETTE_SHIFT_GREEN);
		g_sdl.video.palette[index].blue = ((configuration->palette[index] & PALETTE_MASK_BLUE) >> PALETTE_SHIFT_BLUE);
		g_sdl.video.palette[index].alpha = UINT8_MAX;
		TRACE_FORMAT(LEVEL_VERBOSE, "SDL palette[%i]=%08x (%02x, %02x, %02x)", index, g_sdl.video.palette[index].raw,
			g_sdl.video.palette[index].red, g_sdl.video.palette[index].green, g_sdl.video.palette[index].blue);
	}

	g_sdl.video.scale = configuration->scale;
	if(g_sdl.video.scale < SCALE_MIN) {
		g_sdl.video.scale = SCALE_MIN;
		TRACE_FORMAT(LEVEL_WARNING, "Scale is too small: %u (setting to %u)", configuration->scale, g_sdl.video.scale);
	} else if(g_sdl.video.scale > SCALE_MAX) {
		g_sdl.video.scale = SCALE_MAX;
		TRACE_FORMAT(LEVEL_WARNING, "Scale is too large: %u (setting to %u)", configuration->scale, g_sdl.video.scale);
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "SDL scale=%u", g_sdl.video.scale);

	if(snprintf(g_sdl.video.title, sizeof(g_sdl.video.title), "%s -- %s", strlen(title) ? title : TITLE_UNTITLED, TITLE) < 0) {
		memcpy(g_sdl.video.title, TITLE_UNTITLED, strlen(TITLE_UNTITLED));
	}

	TRACE_FORMAT(LEVEL_VERBOSE, "SDL title[%u]=\"%s\"", strlen(g_sdl.video.title), g_sdl.video.title);

	if(!(g_sdl.video.window = SDL_CreateWindow(g_sdl.video.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH * g_sdl.video.scale, WINDOW_HEIGHT * g_sdl.video.scale, SDL_WINDOW_RESIZABLE))) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(!(g_sdl.video.renderer = SDL_CreateRenderer(g_sdl.video.window, -1, SDL_RENDERER_ACCELERATED))) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_RenderSetLogicalSize(g_sdl.video.renderer, WINDOW_WIDTH, WINDOW_HEIGHT)) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_SetRenderDrawColor(g_sdl.video.renderer, COLOR_BACKGROUND.red, COLOR_BACKGROUND.green, COLOR_BACKGROUND.blue,
			COLOR_BACKGROUND.alpha)) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0") == SDL_FALSE) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(!(g_sdl.video.texture = SDL_CreateTexture(g_sdl.video.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
			WINDOW_WIDTH, WINDOW_HEIGHT))) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if(configuration->fullscreen && ((result = dmg_sdl_display_fullscreen()) != DMG_STATUS_SUCCESS)) {
		goto exit;
	}

	TRACE(LEVEL_INFORMATION, "SDL display loaded");
	result = dmg_sdl_display_show();

exit:
	return result;
}

static void
dmg_sdl_display_unload(void)
{
	TRACE(LEVEL_INFORMATION, "SDL display unloading");

	SDL_DestroyTexture(g_sdl.video.texture);
	SDL_DestroyRenderer(g_sdl.video.renderer);
	SDL_DestroyWindow(g_sdl.video.window);

	TRACE(LEVEL_INFORMATION, "SDL display unloaded");
}

static int
dmg_sdl_key_event(
	__in const SDL_KeyboardEvent *key,
	__in SDL_EventType type
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!key->repeat) {

		switch(key->keysym.scancode) {
			case KEY_FULLSCREEN:

				if(type == SDL_KEYUP) {
					result = dmg_sdl_display_fullscreen();
				}
				break;
			default:
				break;
		};
	}

	return result;
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

	if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

	if((result = dmg_sdl_audio_load()) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_sdl_display_load(configuration, title)) != DMG_STATUS_SUCCESS) {
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
	g_sdl.video.redraw = true;
	g_sdl.video.pixel[y][x].raw = g_sdl.video.palette[color].raw;
}

bool
dmg_sdl_poll(void)
{
	bool result = true;
	SDL_Event event = {};

	while(SDL_PollEvent(&event)) {

		switch(event.type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				TRACE_FORMAT(LEVEL_VERBOSE, "SDL key %s event: %x", (event.type == SDL_KEYDOWN) ? "down" : "up",
						event.key.keysym.scancode);

				if(dmg_sdl_key_event(&event.key, event.type) != DMG_STATUS_SUCCESS) {
					result = false;
				}
				break;
			case SDL_WINDOWEVENT:
				g_sdl.video.redraw = true;
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

int
dmg_sdl_sample(
	__in const void *sample
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(SDL_QueueAudio(g_sdl.audio.device, sample, AUDIO_SAMPLES)) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s", SDL_GetError());
		goto exit;
	}

exit:
	return result;
}

void
dmg_sdl_sync(void)
{
	uint32_t elapsed;

	dmg_sdl_display_show();

	if((elapsed = (SDL_GetTicks() - g_sdl.video.frame_start)) < FRAME_FREQUENCY) {
		SDL_Delay(FRAME_FREQUENCY - elapsed);
	}

	g_sdl.video.frame_start = SDL_GetTicks();

	if(g_sdl.video.frame >= FRAMES_PER_SEC) {
		g_sdl.video.framerate = (g_sdl.video.frame / ((SDL_GetTicks() - g_sdl.video.framerate_start) / (float)MS_PER_SEC));
		g_sdl.video.framerate_start = SDL_GetTicks();
		g_sdl.video.frame = 0;
#ifndef NDEBUG
		char title_debug[TITLE_LENGTH_MAX * 2] = {};

		snprintf(title_debug, sizeof(title_debug), "%s [%.02f]", g_sdl.video.title, g_sdl.video.framerate);
		SDL_SetWindowTitle(g_sdl.video.window, title_debug);
#endif /* NDEBUG */
	} else {
		++g_sdl.video.frame;
	}
}

void
dmg_sdl_unload(void)
{
	TRACE(LEVEL_INFORMATION, "SDL unloading");
	dmg_sdl_display_unload();
	dmg_sdl_audio_unload();
	SDL_Quit();
	TRACE(LEVEL_INFORMATION, "SDL unloaded");
	memset(&g_sdl, 0, sizeof(g_sdl));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SDL */
