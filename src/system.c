/*
 * DMG
 * Copyright (C) 2023 David Jolly
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <system.h>

static const uint32_t PALETTE[] =
{
    /* WHITE    LIGHT-GREY  DARK-GREY   BLACK */
    0xFFFFFFFF, 0xFF949494, 0xFF525252, 0xFF000000,
};

static const SDL_Scancode SCANCODE[] =
{
    /* A            B               SELECT          START */
    SDL_SCANCODE_L, SDL_SCANCODE_K, SDL_SCANCODE_C, SDL_SCANCODE_SPACE,
    /* RIGHT        LEFT            UP              DOWN */
    SDL_SCANCODE_D, SDL_SCANCODE_A, SDL_SCANCODE_W, SDL_SCANCODE_S,
};

static dmg_error_e dmg_system_clock(dmg_handle_t const handle)
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

static void dmg_system_input(dmg_handle_t const handle, SDL_Scancode scancode, bool pressed)
{
    for (dmg_button_e button = 0; button < DMG_BUTTON_MAX; ++button)
    {
        if (SCANCODE[button] == scancode)
        {
            dmg_input_update(handle, button, pressed);
            break;
        }
    }
}

static bool dmg_system_sdl_continue(dmg_handle_t const handle)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (!event.key.repeat)
                {
                    dmg_system_input(handle, event.key.keysym.scancode, event.type == SDL_KEYDOWN);
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

static dmg_error_e dmg_system_sdl_setup_audio(dmg_handle_t const handle)
{
    SDL_AudioSpec desired =
    {
        .freq = 44100, .format = AUDIO_S16SYS, .channels = 1, .samples = 4096,
        .callback = dmg_audio_update, .userdata = handle
    };
    if (!(handle->sdl.audio.id = SDL_OpenAudioDevice(NULL, false, &desired, &handle->sdl.audio.spec, 0)))
    {
        return DMG_ERROR(handle, "SDL_OpenAudioDevice failed -- %s", SDL_GetError());
    }
    SDL_PauseAudioDevice(handle->sdl.audio.id, false);
    return DMG_SUCCESS;
}

static dmg_error_e dmg_system_sdl_setup_video(dmg_handle_t const handle)
{
    if (!(handle->sdl.window = SDL_CreateWindow(dmg_memory_get_title(handle), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 288, SDL_WINDOW_RESIZABLE)))
    {
        return DMG_ERROR(handle, "SDL_CreateWindow failed -- %s", SDL_GetError());
    }
    if (!(handle->sdl.renderer = SDL_CreateRenderer(handle->sdl.window, -1, SDL_RENDERER_PRESENTVSYNC)))
    {
        return DMG_ERROR(handle, "SDL_CreateRenderer failed -- %s", SDL_GetError());
    }
    if (SDL_RenderSetLogicalSize(handle->sdl.renderer, 160, 144))
    {
        return DMG_ERROR(handle, "SDL_RenderSetLogicalSize failed -- %s", SDL_GetError());
    }
    if (SDL_SetRenderDrawColor(handle->sdl.renderer, 0, 0, 0, 0))
    {
        return DMG_ERROR(handle, "SDL_SetRenderDrawColor failed -- %s", SDL_GetError());
    }
    if (SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1") == SDL_FALSE)
    {
        return DMG_ERROR(handle, "SDL_SetHint failed -- %s", SDL_GetError());
    }
    if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0") == SDL_FALSE)
    {
        return DMG_ERROR(handle, "SDL_SetHint failed -- %s", SDL_GetError());
    }
    if (!(handle->sdl.texture = SDL_CreateTexture(handle->sdl.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 160, 144)))
    {
        return DMG_ERROR(handle, "SDL_CreateTexture failed -- %s", SDL_GetError());
    }
    if (!(handle->sdl.cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR)))
    {
        return DMG_ERROR(handle, "SDL_CreateSystemCursor failed -- %s", SDL_GetError());
    }
    SDL_SetCursor(handle->sdl.cursor);
    return DMG_SUCCESS;
}

static dmg_error_e dmg_system_sdl_setup(dmg_handle_t const handle)
{
    dmg_error_e result;
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO))
    {
        return DMG_ERROR(handle, "SDL_Init failed -- %s", SDL_GetError());
    }
    if ((result = dmg_system_sdl_setup_video(handle)) == DMG_SUCCESS)
    {
        result = dmg_system_sdl_setup_audio(handle);
    }
    return result;
}

static void dmg_system_sdl_teardown_audio(dmg_handle_t const handle)
{
    if (handle->sdl.audio.id)
    {
        SDL_PauseAudioDevice(handle->sdl.audio.id, true);
        SDL_CloseAudioDevice(handle->sdl.audio.id);
    }
}

static void dmg_system_sdl_teardown_video(dmg_handle_t const handle)
{
    if (handle->sdl.cursor)
    {
        SDL_FreeCursor(handle->sdl.cursor);
    }
    if (handle->sdl.texture)
    {
        SDL_DestroyTexture(handle->sdl.texture);
    }
    if (handle->sdl.renderer)
    {
        SDL_DestroyRenderer(handle->sdl.renderer);
    }
    if (handle->sdl.window)
    {
        SDL_DestroyWindow(handle->sdl.window);
    }
    SDL_Quit();
}

static void dmg_system_sdl_teardown(dmg_handle_t const handle)
{
    dmg_system_sdl_teardown_audio(handle);
    dmg_system_sdl_teardown_video(handle);
}

static dmg_error_e dmg_system_sdl_update(dmg_handle_t const handle)
{
    uint32_t elapsed, pixel[144][160] = {};
    for (uint8_t y = 0; y < 144; ++y)
    {
        for (uint8_t x = 0; x < 160; ++x)
        {
            pixel[y][x] = PALETTE[handle->sdl.pixel[y][x]];
        }
    }
    if (SDL_UpdateTexture(handle->sdl.texture, NULL, pixel, 160 * sizeof(uint32_t)))
    {
        return DMG_ERROR(handle, "SDL_UpdateTexture failed -- %s", SDL_GetError());
    }
    if (SDL_RenderClear(handle->sdl.renderer))
    {
        return DMG_ERROR(handle, "SDL_RenderClear failed -- %s", SDL_GetError());
    }
    if (SDL_RenderCopy(handle->sdl.renderer, handle->sdl.texture, NULL, NULL))
    {
        return DMG_ERROR(handle, "SDL_RenderCopy failed -- %s", SDL_GetError());
    }
    if ((elapsed = (SDL_GetTicks() - handle->sdl.tick)) < (1000 / (float)60))
    {
        SDL_Delay((1000 / (float)60) - elapsed);
    }
    SDL_RenderPresent(handle->sdl.renderer);
    handle->sdl.tick = SDL_GetTicks();
    return DMG_SUCCESS;
}

dmg_color_e dmg_system_get_pixel(dmg_handle_t const handle, uint8_t x, uint8_t y)
{
    return handle->sdl.pixel[y][x];
}

dmg_error_e dmg_system_initialize(dmg_handle_t const handle, const dmg_data_t *const data)
{
    dmg_error_e result;
    if (handle->initialized)
    {
        return DMG_ERROR(handle, "System reinitialized");
    }
    if ((result = dmg_memory_initialize(handle, data)) != DMG_SUCCESS)
    {
        return result;
    }
    if ((result = dmg_system_sdl_setup(handle)) != DMG_SUCCESS)
    {
        return result;
    }
    handle->initialized = true;
    return result;
}

dmg_error_e dmg_system_load(dmg_handle_t const handle, const dmg_data_t *const data)
{
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    return dmg_memory_load(handle, data);
}

uint8_t dmg_system_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x8000 ... 0x9FFF: /* VIDEO */
        case 0xFE00 ... 0xFE9F:
        case 0xFF40 ... 0xFF4B:
            result = dmg_video_read(handle, address);
            break;
        case 0xFF00: /* INPUT */
            result = dmg_input_read(handle, address);
            break;
        case 0xFF01 ... 0xFF02: /* SERIAL */
            result = dmg_serial_read(handle, address);
            break;
        case 0xFF04 ... 0xFF07: /* TIMER */
            result = dmg_timer_read(handle, address);
            break;
        case 0xFF0F: /* PROCESSOR */
        case 0xFFFF:
            result = dmg_processor_read(handle, address);
            break;
        case 0xFF10 ... 0xFF14: /* AUDIO */
        case 0xFF16 ... 0xFF19:
        case 0xFF1A ... 0xFF1E:
        case 0xFF20 ... 0xFF26:
        case 0xFF30 ... 0xFF3F:
            result = dmg_audio_read(handle, address);
            break;
        default: /* MEMORY */
            result = dmg_memory_read(handle, address);
            break;
    }
    return result;
}

dmg_error_e dmg_system_run(dmg_handle_t const handle)
{
    dmg_error_e result = DMG_SUCCESS;
    while (dmg_system_sdl_continue(handle))
    {
        if (result != DMG_SUCCESS)
        {
            return result;
        }
        while ((result = dmg_system_clock(handle)) != DMG_COMPLETE)
        {
            if (result == DMG_FAILURE)
            {
                return result;
            }
        }
        if ((result = dmg_system_sdl_update(handle)) != DMG_SUCCESS)
        {
            return result;
        }
    }
    return (result != DMG_FAILURE) ? DMG_SUCCESS : result;
}

dmg_error_e dmg_system_save(dmg_handle_t const handle, dmg_data_t *const data)
{
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    return dmg_memory_save(handle, data);
}

void dmg_system_set_pixel(dmg_handle_t const handle, dmg_color_e color, uint8_t x, uint8_t y)
{
    handle->sdl.pixel[y][x] = color;
}

void dmg_system_uninitialize(dmg_handle_t const handle)
{
    handle->initialized = false;
    dmg_system_sdl_teardown(handle);
    dmg_memory_uninitialize(handle);
}

void dmg_system_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0x8000 ... 0x9FFF: /* VIDEO */
        case 0xFE00 ... 0xFE9F:
        case 0xFF40 ... 0xFF4B:
            dmg_video_write(handle, address, value);
            break;
        case 0xFF00: /* INPUT */
            dmg_input_write(handle, address, value);
            break;
        case 0xFF01 ... 0xFF02: /* SERIAL */
            dmg_serial_write(handle, address, value);
            break;
        case 0xFF04 ... 0xFF07: /* TIMER */
            dmg_timer_write(handle, address, value);
            break;
        case 0xFF0F: /* PROCESSOR */
        case 0xFFFF:
            dmg_processor_write(handle, address, value);
            break;
        case 0xFF10 ... 0xFF14: /* AUDIO */
        case 0xFF16 ... 0xFF19:
        case 0xFF1A ... 0xFF1E:
        case 0xFF20 ... 0xFF26:
        case 0xFF30 ... 0xFF3F:
            dmg_audio_write(handle, address, value);
            break;
        default: /* MEMORY */
            dmg_memory_write(handle, address, value);
            break;
    }
}
