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

#ifndef DMG_COMMON_H_
#define DMG_COMMON_H_

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <audio.h>
#include <input.h>
#include <memory.h>
#include <processor.h>
#include <serial.h>
#include <timer.h>
#include <video.h>

#define DMG_MAJOR 0
#define DMG_MINOR 1
#define DMG_PATCH 0xa66d3b7

typedef enum
{
    DMG_COLOR_WHITE = 0,
    DMG_COLOR_LIGHT_GREY,
    DMG_COLOR_DARK_GREY,
    DMG_COLOR_BLACK,
    DMG_COLOR_MAX,
} dmg_color_e;

struct dmg_s
{
    char error[256];
    bool initialized;
    dmg_audio_t audio;
    dmg_input_t input;
    dmg_memory_t memory;
    dmg_processor_t processor;
    dmg_serial_t serial;
    dmg_timer_t timer;
    dmg_video_t video;
    struct
    {
        uint32_t tick;
        dmg_palette_e palette;
        dmg_color_e pixel[144][160];
        SDL_Cursor *cursor;
        SDL_Renderer *renderer;
        SDL_Texture *texture;
        SDL_Window *window;
        struct
        {
            SDL_AudioDeviceID id;
            SDL_AudioSpec spec;
        } audio;
    } service;
};

#define DMG_ERROR(_HANDLE_, _FORMAT_, ...) \
    dmg_set_error(_HANDLE_, __FILE__, __LINE__, _FORMAT_, ##__VA_ARGS__)

dmg_color_e dmg_get_pixel(dmg_handle_t const handle, uint8_t x, uint8_t y);
uint8_t dmg_get_silence(dmg_handle_t const handle);
uint8_t dmg_read(dmg_handle_t const handle, uint16_t address);
dmg_error_e dmg_set_error(dmg_handle_t const handle, const char *file, uint32_t line, const char *format, ...);
void dmg_set_pixel(dmg_handle_t const handle, dmg_color_e color, uint8_t x, uint8_t y);
void dmg_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_COMMON_H_ */
