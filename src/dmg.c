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

#include <common.h>

typedef union
{
    struct
    {
        uint8_t alpha;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };
    uint32_t raw;
} dmg_color_t;

static const dmg_color_t PALETTE[][DMG_PALETTE_MAX][DMG_COLOR_MAX] =
{
    { /* FOREGROUND */
        { /* DMG */
            /* WHITE               LIGHT-GREY             DARK-GREY              BLACK */
            { .raw = 0xFF939905 }, { .raw = 0xFF4D7D2C }, { .raw = 0xFF296341 }, { .raw = 0xFF0C3B1C },
        },
        { /* GBP */
            /* WHITE               LIGHT-GREY             DARK-GREY              BLACK */
            { .raw = 0xFFE5E5E5 }, { .raw = 0xFFAEBBAA }, { .raw = 0xFF889683 }, { .raw = 0xFF5C5F5C },
        },
    },
    { /* BACKGROUND */
        { /* DMG */
            /* WHITE               LIGHT-GREY             DARK-GREY              BLACK */
            { .raw = 0xFFAAB10F }, { .raw = 0xFF5C9136 }, { .raw = 0xFF33714C }, { .raw = 0xFF0F4222 },
        },
        { /* GBP */
            /* WHITE               LIGHT-GREY             DARK-GREY              BLACK */
            { .raw = 0xFFFEFEFE }, { .raw = 0xFFC1CFBE }, { .raw = 0xFF97A791 }, { .raw = 0xFF656865 },
        },
    }
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

static bool dmg_service_poll(dmg_handle_t const handle)
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

static dmg_error_e dmg_service_setup_audio(dmg_handle_t const handle)
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

static dmg_error_e dmg_service_setup_video(dmg_handle_t const handle, dmg_palette_e palette)
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
    if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0") == SDL_FALSE)
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

static dmg_error_e dmg_service_setup(dmg_handle_t const handle, dmg_palette_e palette)
{
    dmg_error_e result;
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO))
    {
        return DMG_ERROR(handle, "SDL_Init failed -- %s", SDL_GetError());
    }
    if ((result = dmg_service_setup_video(handle, palette)) == DMG_SUCCESS)
    {
        result = dmg_service_setup_audio(handle);
    }
    return result;
}

static dmg_error_e dmg_service_sync(dmg_handle_t const handle)
{
    uint32_t elapsed, pixel[432][480] = {};
    for (uint8_t y = 0; y < 144; ++y)
    {
        for (uint8_t x = 0; x < 160; ++x)
        {
            uint16_t x_base = x * 3, y_base = y * 3;
            dmg_color_e color = handle->service.pixel[y][x], color_above = color;
            if (y)
            {
                color_above = handle->service.pixel[y - 1][x];
            }
            for (uint8_t y_off = 0; y_off < 3; ++y_off)
            {
                for (uint8_t x_off = 0; x_off < 3; ++x_off)
                {
                    dmg_color_t value = PALETTE[((x_off > 0) && (y_off > 0))][handle->service.palette][color];
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

static void dmg_service_teardown_audio(dmg_handle_t const handle)
{
    if (handle->service.audio.id)
    {
        SDL_PauseAudioDevice(handle->service.audio.id, true);
        SDL_CloseAudioDevice(handle->service.audio.id);
    }
}

static void dmg_service_teardown_video(dmg_handle_t const handle)
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

static void dmg_service_teardown(dmg_handle_t const handle)
{
    dmg_service_teardown_audio(handle);
    dmg_service_teardown_video(handle);
    SDL_Quit();
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

dmg_color_e dmg_get_pixel(dmg_handle_t const handle, uint8_t x, uint8_t y)
{
    return handle->service.pixel[y][x];
}

uint8_t dmg_get_silence(dmg_handle_t const handle)
{
    return handle->service.audio.spec.silence;
}

const dmg_version_t *dmg_get_version(void)
{
    return &VERSION;
}

dmg_error_e dmg_initialize(dmg_handle_t *handle, const dmg_data_t *const data, const dmg_output_f output, dmg_palette_e palette)
{
    dmg_error_e result;
    if (!handle || (!*handle && !(*handle = calloc(1, sizeof (**handle)))))
    {
        return EXIT_FAILURE;
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
    if ((result = dmg_service_setup(*handle, palette)) != DMG_SUCCESS)
    {
        return result;
    }
    (*handle)->initialized = true;
    return result;
}

dmg_error_e dmg_input(dmg_handle_t const handle, uint8_t input, uint8_t *output)
{
    if (!handle || !output)
    {
        return EXIT_FAILURE;
    }
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    *output = dmg_serial_input(handle, input);
    return DMG_SUCCESS;
}

dmg_error_e dmg_load(dmg_handle_t const handle, const dmg_data_t *const data)
{
    if (!handle)
    {
        return EXIT_FAILURE;
    }
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    return dmg_memory_load(handle, data);
}

uint8_t dmg_read(dmg_handle_t const handle, uint16_t address)
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

dmg_error_e dmg_run(dmg_handle_t const handle)
{
    if (!handle)
    {
        return EXIT_FAILURE;
    }
    while (dmg_service_poll(handle))
    {
        dmg_error_e result;
        while ((result = dmg_clock(handle)) != DMG_COMPLETE)
        {
            if (result == DMG_FAILURE)
            {
                return result;
            }
        }
        if ((result = dmg_service_sync(handle)) != DMG_SUCCESS)
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
        return EXIT_FAILURE;
    }
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    return dmg_memory_save(handle, data);
}

dmg_error_e dmg_set_error(dmg_handle_t const handle, const char *file, uint32_t line, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    vsnprintf(handle->error, sizeof (handle->error), format, arguments);
    va_end(arguments);
    snprintf(handle->error + strlen(handle->error), sizeof (handle->error) - strlen(handle->error), " (%s:%u)", file, line);
    return DMG_FAILURE;
}

void dmg_set_pixel(dmg_handle_t const handle, dmg_color_e color, uint8_t x, uint8_t y)
{
    handle->service.pixel[y][x] = color;
}

void dmg_uninitialize(dmg_handle_t *handle)
{
    if (handle && *handle)
    {
        (*handle)->initialized = false;
        dmg_service_teardown(*handle);
        dmg_memory_uninitialize(*handle);
        free(*handle);
        *handle = NULL;
    }
}

void dmg_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
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
