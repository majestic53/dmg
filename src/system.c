/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

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
} dmg_pixel_t;

static const dmg_pixel_t PALETTE[] =
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

static dmg_error_e dmg_system_initialize_audio(dmg_t const dmg)
{
    SDL_AudioSpec desired =
    {
        .freq = 44100, .format = AUDIO_S16SYS, .channels = 1, .samples = 4096,
        .callback = dmg_audio_output, .userdata = dmg
    };
    if (!(dmg->service.audio.id = SDL_OpenAudioDevice(NULL, false, &desired, &dmg->service.audio.spec, 0)))
    {
        return DMG_ERROR(dmg, "SDL_OpenAudioDevice failed -- %s", SDL_GetError());
    }
    SDL_PauseAudioDevice(dmg->service.audio.id, false);
    return DMG_SUCCESS;
}

static dmg_error_e dmg_system_initialize_video(dmg_t const dmg)
{
    if (!(dmg->service.window = SDL_CreateWindow(dmg_cartridge_title(dmg), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 432, SDL_WINDOW_RESIZABLE)))
    {
        return DMG_ERROR(dmg, "SDL_CreateWindow failed -- %s", SDL_GetError());
    }
    if (!(dmg->service.renderer = SDL_CreateRenderer(dmg->service.window, -1, SDL_RENDERER_PRESENTVSYNC)))
    {
        return DMG_ERROR(dmg, "SDL_CreateRenderer failed -- %s", SDL_GetError());
    }
    if (SDL_RenderSetLogicalSize(dmg->service.renderer, 480, 432))
    {
        return DMG_ERROR(dmg, "SDL_RenderSetLogicalSize failed -- %s", SDL_GetError());
    }
    if (SDL_SetRenderDrawColor(dmg->service.renderer, 0, 0, 0, 0))
    {
        return DMG_ERROR(dmg, "SDL_SetRenderDrawColor failed -- %s", SDL_GetError());
    }
    if (SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1") == SDL_FALSE)
    {
        return DMG_ERROR(dmg, "SDL_SetHint failed -- %s", SDL_GetError());
    }
    if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0") == SDL_FALSE)
    {
        return DMG_ERROR(dmg, "SDL_SetHint failed -- %s", SDL_GetError());
    }
    if (!(dmg->service.texture = SDL_CreateTexture(dmg->service.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 480, 432)))
    {
        return DMG_ERROR(dmg, "SDL_CreateTexture failed -- %s", SDL_GetError());
    }
    if (!(dmg->service.cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR)))
    {
        return DMG_ERROR(dmg, "SDL_CreateSystemCursor failed -- %s", SDL_GetError());
    }
    SDL_SetCursor(dmg->service.cursor);
    return DMG_SUCCESS;
}

static void dmg_system_uninitialize_audio(dmg_t const dmg)
{
    if (dmg->service.audio.id)
    {
        SDL_PauseAudioDevice(dmg->service.audio.id, true);
        SDL_CloseAudioDevice(dmg->service.audio.id);
    }
}

static void dmg_system_uninitialize_video(dmg_t const dmg)
{
    if (dmg->service.cursor)
    {
        SDL_FreeCursor(dmg->service.cursor);
    }
    if (dmg->service.texture)
    {
        SDL_DestroyTexture(dmg->service.texture);
    }
    if (dmg->service.renderer)
    {
        SDL_DestroyRenderer(dmg->service.renderer);
    }
    if (dmg->service.window)
    {
        SDL_DestroyWindow(dmg->service.window);
    }
}

static void dmg_system_update(dmg_t const dmg)
{
    for (uint8_t y = 0; y < 144; ++y)
    {
        for (uint8_t x = 0; x < 160; ++x)
        {
            uint16_t x_base = x * 3, y_base = y * 3;
            dmg_color_e color = dmg_video_color(dmg, x, y), color_above = color;
            if (y)
            {
                color_above = dmg_video_color(dmg, x, y - 1);
            }
            for (uint8_t y_off = 0; y_off < 3; ++y_off)
            {
                for (uint8_t x_off = 0; x_off < 3; ++x_off)
                {
                    dmg_pixel_t value = PALETTE[color];
                    if (!x_off || !y_off)
                    {
                        value.red *= 0.95;
                        value.green *= 0.95;
                        value.blue *= 0.95;
                    }
                    if (color_above > color)
                    {
                        value.red *= 0.75;
                        value.green *= 0.75;
                        value.blue *= 0.75;
                    }
                    dmg->service.pixel[y_base + y_off][x_base + x_off] = value.raw;
                }
            }
        }
    }
}

dmg_error_e dmg_system_input(dmg_t const dmg, uint8_t input, uint8_t *output)
{
    if (!dmg->initialized)
    {
        return DMG_ERROR(dmg, "System uninitialized");
    }
    if (!output)
    {
        return DMG_ERROR(dmg, "Invalid output -- %p", output);
    }
    *output = dmg_serial_input(dmg, input);
    return DMG_SUCCESS;
}

dmg_error_e dmg_system_initialize(dmg_t dmg, const dmg_data_t *const data, const dmg_output_f output)
{
    dmg_error_e result;
    if (dmg->initialized)
    {
        return DMG_ERROR(dmg, "System reinitialized");
    }
    if ((result = dmg_memory_initialize(dmg, data)) != DMG_SUCCESS)
    {
        return result;
    }
    if ((result = dmg_serial_initialize(dmg, output)) != DMG_SUCCESS)
    {
        return result;
    }
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO))
    {
        return DMG_ERROR(dmg, "SDL_Init failed -- %s", SDL_GetError());
    }
    if ((result = dmg_system_initialize_video(dmg)) != DMG_SUCCESS)
    {
        return result;
    }
    if ((result = dmg_system_initialize_audio(dmg)) != DMG_SUCCESS)
    {
        return result;
    }
    dmg_audio_initialize(dmg);
    dmg->initialized = true;
    return result;
}

dmg_error_e dmg_system_load(dmg_t const dmg, const dmg_data_t *const data)
{
    if (!dmg->initialized)
    {
        return DMG_ERROR(dmg, "System uninitialized");
    }
    return dmg_cartridge_load(dmg, data);
}

bool dmg_system_poll(dmg_t const dmg)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (!event.key.repeat)
                {
                    for (dmg_button_e button = 0; button < DMG_BUTTON_MAX; ++button)
                    {
                        if (SCANCODE[button] == event.key.keysym.scancode)
                        {
                            dmg_controller_update(dmg, button, event.type == SDL_KEYDOWN);
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

uint8_t dmg_system_read(dmg_t const dmg, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x8000 ... 0x9FFF: /* VIDEO */
        case 0xFE00 ... 0xFE9F:
        case 0xFF40 ... 0xFF4B:
            result = dmg_video_read(dmg, address);
            break;
        case 0xFF00: /* CONTROLLER */
            result = dmg_controller_read(dmg, address);
            break;
        case 0xFF01 ... 0xFF02: /* SERIAL */
            result = dmg_serial_read(dmg, address);
            break;
        case 0xFF04 ... 0xFF07: /* TIMER */
            result = dmg_timer_read(dmg, address);
            break;
        case 0xFF0F: /* PROCESSOR */
        case 0xFFFF:
            result = dmg_processor_read(dmg, address);
            break;
        case 0xFF10 ... 0xFF14: /* AUDIO */
        case 0xFF16 ... 0xFF19:
        case 0xFF1A ... 0xFF1E:
        case 0xFF20 ... 0xFF26:
        case 0xFF30 ... 0xFF3F:
            result = dmg_audio_read(dmg, address);
            break;
        default: /* MEMORY */
            result = dmg_memory_read(dmg, address);
            break;
    }
    return result;
}

dmg_error_e dmg_system_run(dmg_t const dmg)
{
    if (!dmg->initialized)
    {
        return DMG_ERROR(dmg, "System uninitialized");
    }
    while (dmg_system_poll(dmg))
    {
        dmg_error_e result;
        do
        {
            dmg_audio_clock(dmg);
            dmg_serial_clock(dmg);
            dmg_timer_clock(dmg);
            dmg_processor_clock(dmg);
        } while (!dmg_video_clock(dmg));
        if ((result = dmg_system_sync(dmg)) != DMG_SUCCESS)
        {
            return result;
        }
    }
    return DMG_SUCCESS;
}

dmg_error_e dmg_system_save(dmg_t const dmg, dmg_data_t *const data)
{
    if (!dmg->initialized)
    {
        return DMG_ERROR(dmg, "System uninitialized");
    }
    return dmg_cartridge_save(dmg, data);
}

uint8_t dmg_system_silence(dmg_t const dmg)
{
    return dmg->service.audio.spec.silence;
}

dmg_error_e dmg_system_sync(dmg_t const dmg)
{
    uint32_t elapsed;
    dmg_system_update(dmg);
    if (SDL_UpdateTexture(dmg->service.texture, NULL, dmg->service.pixel, 480 * sizeof (uint32_t)))
    {
        return DMG_ERROR(dmg, "SDL_UpdateTexture failed -- %s", SDL_GetError());
    }
    if (SDL_RenderClear(dmg->service.renderer))
    {
        return DMG_ERROR(dmg, "SDL_RenderClear failed -- %s", SDL_GetError());
    }
    if (SDL_RenderCopy(dmg->service.renderer, dmg->service.texture, NULL, NULL))
    {
        return DMG_ERROR(dmg, "SDL_RenderCopy failed -- %s", SDL_GetError());
    }
    if ((elapsed = (SDL_GetTicks() - dmg->service.tick)) < (1000 / (float)60))
    {
        SDL_Delay((1000 / (float)60) - elapsed);
    }
    SDL_RenderPresent(dmg->service.renderer);
    dmg->service.tick = SDL_GetTicks();
    return DMG_SUCCESS;
}

void dmg_system_uninitialize(dmg_t const dmg)
{
    dmg->initialized = false;
    dmg_system_uninitialize_audio(dmg);
    dmg_system_uninitialize_video(dmg);
    SDL_Quit();
    dmg_cartridge_uninitialize(dmg);
}

void dmg_system_write(dmg_t const dmg, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0x8000 ... 0x9FFF: /* VIDEO */
        case 0xFE00 ... 0xFE9F:
        case 0xFF40 ... 0xFF4B:
            dmg_video_write(dmg, address, value);
            break;
        case 0xFF00: /* CONTROLLER */
            dmg_controller_write(dmg, address, value);
            break;
        case 0xFF01 ... 0xFF02: /* SERIAL */
            dmg_serial_write(dmg, address, value);
            break;
        case 0xFF04 ... 0xFF07: /* TIMER */
            dmg_timer_write(dmg, address, value);
            break;
        case 0xFF0F: /* PROCESSOR */
        case 0xFFFF:
            dmg_processor_write(dmg, address, value);
            break;
        case 0xFF10 ... 0xFF14: /* AUDIO */
        case 0xFF16 ... 0xFF19:
        case 0xFF1A ... 0xFF1E:
        case 0xFF20 ... 0xFF26:
        case 0xFF30 ... 0xFF3F:
            dmg_audio_write(dmg, address, value);
            break;
        default: /* MEMORY */
            dmg_memory_write(dmg, address, value);
            break;
    }
}
