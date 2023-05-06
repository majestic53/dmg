/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

static const dmg_color_t PALETTE[][DMG_COLOR_MAX] =
{
    { /* GB */
        /* WHITE               LIGHT-GREY             DARK-GREY              BLACK */
        { .raw = 0xFFAAB10F }, { .raw = 0xFF5C9136 }, { .raw = 0xFF33714C }, { .raw = 0xFF0F4222 },
    },
    { /* GBP */
        /* WHITE               LIGHT-GREY             DARK-GREY              BLACK */
        { .raw = 0xFFFEFEFE }, { .raw = 0xFFC1CFBE }, { .raw = 0xFF97A791 }, { .raw = 0xFF656865 },
    },
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

static dmg_error_e dmg_system_initialize_audio(dmg_handle_t const handle)
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

static dmg_error_e dmg_system_initialize_video(dmg_handle_t const handle, dmg_palette_e palette)
{
    if (!(handle->service.window = SDL_CreateWindow(dmg_memory_get_title(handle), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 288, SDL_WINDOW_RESIZABLE)))
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
    handle->service.palette = (palette >= DMG_PALETTE_MAX) ? 0 : palette;
    return DMG_SUCCESS;
}

static void dmg_system_uninitialize_audio(dmg_handle_t const handle)
{
    if (handle->service.audio.id)
    {
        SDL_PauseAudioDevice(handle->service.audio.id, true);
        SDL_CloseAudioDevice(handle->service.audio.id);
    }
}

static void dmg_system_uninitialize_video(dmg_handle_t const handle)
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

dmg_error_e dmg_system_initialize(dmg_handle_t handle, const dmg_data_t *const data, const dmg_output_f output, dmg_palette_e palette)
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
    if ((result = dmg_serial_initialize(handle, output)) != DMG_SUCCESS)
    {
        return result;
    }
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO))
    {
        return DMG_ERROR(handle, "SDL_Init failed -- %s", SDL_GetError());
    }
    if ((result = dmg_system_initialize_video(handle, palette)) == DMG_SUCCESS)
    {
        result = dmg_system_initialize_audio(handle);
    }
    dmg_audio_initialize(handle);
    handle->initialized = true;
    return result;
}

dmg_error_e dmg_system_input(dmg_handle_t const handle, uint8_t input, uint8_t *output)
{
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

dmg_error_e dmg_system_load(dmg_handle_t const handle, const dmg_data_t *const data)
{
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    return dmg_memory_load(handle, data);
}

bool dmg_system_poll(dmg_handle_t const handle)
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
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    while (dmg_system_poll(handle))
    {
        dmg_error_e result;
        while ((result = dmg_system_clock(handle)) != DMG_COMPLETE)
        {
            if (result == DMG_FAILURE)
            {
                return result;
            }
        }
        if ((result = dmg_system_sync(handle)) != DMG_SUCCESS)
        {
            return result;
        }
    }
    return DMG_SUCCESS;
}

dmg_error_e dmg_system_save(dmg_handle_t const handle, dmg_data_t *const data)
{
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    return dmg_memory_save(handle, data);
}

uint8_t dmg_system_silence(dmg_handle_t const handle)
{
    return handle->service.audio.spec.silence;
}

dmg_error_e dmg_system_sync(dmg_handle_t const handle)
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
                    dmg_color_t value = PALETTE[handle->service.palette][color];
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

void dmg_system_uninitialize(dmg_handle_t const handle)
{
    handle->initialized = false;
    dmg_system_uninitialize_audio(handle);
    dmg_system_uninitialize_video(handle);
    SDL_Quit();
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
