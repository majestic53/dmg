/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_BUS_H_
#define DMG_BUS_H_

#include <SDL.h>
#include <audio.h>
#include <input.h>
#include <memory.h>
#include <processor.h>
#include <serial.h>
#include <timer.h>
#include <video.h>

typedef struct dmg_s
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
} dmg_bus_t;

#define DMG_ERROR(_HANDLE_, _FORMAT_, ...) \
    dmg_bus_error(_HANDLE_, __FILE__, __LINE__, _FORMAT_, ##__VA_ARGS__)

dmg_error_t dmg_bus_error(dmg_handle_t const handle, const char *file, uint32_t line, const char *format, ...);
uint8_t dmg_bus_read(dmg_handle_t const handle, uint16_t address);
void dmg_bus_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_BUS_H_ */
