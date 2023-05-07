/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_SYSTEM_H_
#define DMG_SYSTEM_H_

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
#define DMG_PATCH 0xf0bde09

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

uint8_t dmg_get_silence(dmg_handle_t const handle);
uint8_t dmg_read(dmg_handle_t const handle, uint16_t address);
dmg_error_e dmg_set_error(dmg_handle_t const handle, const char *file, uint32_t line, const char *format, ...);
void dmg_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_SYSTEM_H_ */
