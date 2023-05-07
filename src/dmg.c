/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

static const dmg_color_t PALETTE[] =
{
    /* WHITE               LIGHT-GREY             DARK-GREY              BLACK */
    { .raw = 0xFFAAB10F }, { .raw = 0xFF5C9136 }, { .raw = 0xFF33714C }, { .raw = 0xFF0F4222 },
};

static const SDL_Scancode SCANCODE[] =
{
    /* A            B               SELECT          START */
    SDL_SCANCODE_L, SDL_SCANCODE_K, SDL_SCANCODE_C, SDL_SCANCODE_SPACE,
    /* RIGHT        LEFT            UP              DOWN */
    SDL_SCANCODE_D, SDL_SCANCODE_A, SDL_SCANCODE_W, SDL_SCANCODE_S,
};

static const dmg_version_t VERSION =
{
    DMG_MAJOR, DMG_MINOR, DMG_PATCH
};

static dmg_error_e dmg_clock(dmg_handle_t const handle)
{
    dmg_error_e result;
    dmg_audio_clock(handle);
    dmg_serial_clock(handle);
    dmg_timer_clock(handle);
    if ((result = dmg_processor_clock(handle)) == DMG_SUCCESS)
    {
        result = dmg_video_clock(handle);
    }
    return result;
}

static dmg_error_e dmg_initialize_audio(dmg_handle_t const handle)
{
    SDL_AudioSpec desired =
    {
        .freq = 44100, .format = AUDIO_S16SYS, .channels = 1, .samples = 4096,
        .callback = dmg_audio_output, .userdata = handle
    };
    if (!(handle->service.audio.id = SDL_OpenAudioDevice(NULL, false, &desired, &handle->service.audio.spec, 0)))
    {
        return DMG_ERROR(handle, "SDL_OpenAudioDevice failed -- %s", SDL_GetError());
    }
    SDL_PauseAudioDevice(handle->service.audio.id, false);
    return DMG_SUCCESS;
}

static dmg_error_e dmg_initialize_video(dmg_handle_t const handle)
{
    if (!(handle->service.window = SDL_CreateWindow(dmg_memory_get_title(handle), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 432, SDL_WINDOW_RESIZABLE)))
    {
        return DMG_ERROR(handle, "SDL_CreateWindow failed -- %s", SDL_GetError());
    }
    if (!(handle->service.renderer = SDL_CreateRenderer(handle->service.window, -1, SDL_RENDERER_PRESENTVSYNC)))
    {
        return DMG_ERROR(handle, "SDL_CreateRenderer failed -- %s", SDL_GetError());
    }
    if (SDL_RenderSetLogicalSize(handle->service.renderer, 480, 432))
    {
        return DMG_ERROR(handle, "SDL_RenderSetLogicalSize failed -- %s", SDL_GetError());
    }
    if (SDL_SetRenderDrawColor(handle->service.renderer, 0, 0, 0, 0))
    {
        return DMG_ERROR(handle, "SDL_SetRenderDrawColor failed -- %s", SDL_GetError());
    }
    if (SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1") == SDL_FALSE)
    {
        return DMG_ERROR(handle, "SDL_SetHint failed -- %s", SDL_GetError());
    }
    if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1") == SDL_FALSE)
    {
        return DMG_ERROR(handle, "SDL_SetHint failed -- %s", SDL_GetError());
    }
    if (!(handle->service.texture = SDL_CreateTexture(handle->service.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 480, 432)))
    {
        return DMG_ERROR(handle, "SDL_CreateTexture failed -- %s", SDL_GetError());
    }
    if (!(handle->service.cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR)))
    {
        return DMG_ERROR(handle, "SDL_CreateSystemCursor failed -- %s", SDL_GetError());
    }
    SDL_SetCursor(handle->service.cursor);
    return DMG_SUCCESS;
}

static bool dmg_poll(dmg_handle_t const handle)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (!event.key.repeat)
                {
                    for (dmg_button_e button = 0; button < DMG_BUTTON_MAX; ++button)
                    {
                        if (SCANCODE[button] == event.key.keysym.scancode)
                        {
                            dmg_input_set(handle, button, event.type == SDL_KEYDOWN);
                            break;
                        }
                    }
                }
                break;
            case SDL_QUIT:
                return false;
            default:
                break;
        }
    }
    return true;
}

static dmg_error_e dmg_sync(dmg_handle_t const handle)
{
    uint32_t elapsed, pixel[432][480] = {};
    for (uint8_t y = 0; y < 144; ++y)
    {
        for (uint8_t x = 0; x < 160; ++x)
        {
            uint16_t x_base = x * 3, y_base = y * 3;
            dmg_color_e color = dmg_video_color(handle, x, y), color_above = color;
            if (y)
            {
                color_above = dmg_video_color(handle, x, y - 1);
            }
            for (uint8_t y_off = 0; y_off < 3; ++y_off)
            {
                for (uint8_t x_off = 0; x_off < 3; ++x_off)
                {
                    dmg_color_t value = PALETTE[color];
                    if (!x_off || !y_off)
                    {
                        value.red *= 0.9;
                        value.green *= 0.9;
                        value.blue *= 0.9;
                    }
                    if (color_above > color)
                    {
                        value.red *= 0.75;
                        value.green *= 0.75;
                        value.blue *= 0.75;
                    }
                    pixel[y_base + y_off][x_base + x_off] = value.raw;
                }
            }
        }
    }
    if (SDL_UpdateTexture(handle->service.texture, NULL, pixel, 480 * sizeof (uint32_t)))
    {
        return DMG_ERROR(handle, "SDL_UpdateTexture failed -- %s", SDL_GetError());
    }
    if (SDL_RenderClear(handle->service.renderer))
    {
        return DMG_ERROR(handle, "SDL_RenderClear failed -- %s", SDL_GetError());
    }
    if (SDL_RenderCopy(handle->service.renderer, handle->service.texture, NULL, NULL))
    {
        return DMG_ERROR(handle, "SDL_RenderCopy failed -- %s", SDL_GetError());
    }
    if ((elapsed = (SDL_GetTicks() - handle->service.tick)) < (1000 / (float)60))
    {
        SDL_Delay((1000 / (float)60) - elapsed);
    }
    SDL_RenderPresent(handle->service.renderer);
    handle->service.tick = SDL_GetTicks();
    return DMG_SUCCESS;
}

static void dmg_uninitialize_audio(dmg_handle_t const handle)
{
    if (handle->service.audio.id)
    {
        SDL_PauseAudioDevice(handle->service.audio.id, true);
        SDL_CloseAudioDevice(handle->service.audio.id);
    }
}

static void dmg_uninitialize_video(dmg_handle_t const handle)
{
    if (handle->service.cursor)
    {
        SDL_FreeCursor(handle->service.cursor);
    }
    if (handle->service.texture)
    {
        SDL_DestroyTexture(handle->service.texture);
    }
    if (handle->service.renderer)
    {
        SDL_DestroyRenderer(handle->service.renderer);
    }
    if (handle->service.window)
    {
        SDL_DestroyWindow(handle->service.window);
    }
}

const char *dmg_get_error(dmg_handle_t const handle)
{
    if (!handle)
    {
        return "Invalid handle";
    }
    if (!strlen(handle->error))
    {
        return "No error";
    }
    return handle->error;
}

const dmg_version_t *dmg_get_version(void)
{
    return &VERSION;
}

dmg_error_e dmg_initialize(dmg_handle_t *handle, const dmg_data_t *const data, const dmg_output_f output)
{
    dmg_error_e result;
    if (!handle || (!*handle && !(*handle = calloc(1, sizeof (**handle)))))
    {
        return DMG_FAILURE;
    }
    if ((*handle)->initialized)
    {
        return DMG_ERROR(*handle, "System reinitialized");
    }
    if ((result = dmg_memory_initialize(*handle, data)) != DMG_SUCCESS)
    {
        return result;
    }
    if ((result = dmg_serial_initialize(*handle, output)) != DMG_SUCCESS)
    {
        return result;
    }
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO))
    {
        return DMG_ERROR(*handle, "SDL_Init failed -- %s", SDL_GetError());
    }
    if ((result = dmg_initialize_video(*handle)) != DMG_SUCCESS)
    {
        return result;
    }
    if ((result = dmg_initialize_audio(*handle)) != DMG_SUCCESS)
    {
        return result;
    }
    dmg_audio_initialize(*handle);
    (*handle)->initialized = true;
    return result;
}

dmg_error_e dmg_input(dmg_handle_t const handle, uint8_t input, uint8_t *output)
{
    if (!handle)
    {
        return DMG_FAILURE;
    }
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    if (!output)
    {
        return DMG_ERROR(handle, "Invalid output -- %p", output);
    }
    *output = dmg_serial_input(handle, input);
    return DMG_SUCCESS;
}

dmg_error_e dmg_load(dmg_handle_t const handle, const dmg_data_t *const data)
{
    if (!handle)
    {
        return DMG_FAILURE;
    }
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    return dmg_memory_load(handle, data);
}

dmg_error_e dmg_run(dmg_handle_t const handle)
{
    if (!handle)
    {
        return DMG_FAILURE;
    }
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    while (dmg_poll(handle))
    {
        dmg_error_e result;
        while ((result = dmg_clock(handle)) != DMG_COMPLETE)
        {
            if (result == DMG_FAILURE)
            {
                return result;
            }
        }
        if ((result = dmg_sync(handle)) != DMG_SUCCESS)
        {
            return result;
        }
    }
    return DMG_SUCCESS;
}

dmg_error_e dmg_save(dmg_handle_t const handle, dmg_data_t *const data)
{
    if (!handle)
    {
        return DMG_FAILURE;
    }
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    return dmg_memory_save(handle, data);
}

void dmg_uninitialize(dmg_handle_t *handle)
{
    if (handle && *handle)
    {
        (*handle)->initialized = false;
        dmg_uninitialize_audio(*handle);
        dmg_uninitialize_video(*handle);
        SDL_Quit();
        dmg_memory_uninitialize(*handle);
        free(*handle);
        *handle = NULL;
    }
}
