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
} dmg_system_t;

dmg_error_e dmg_system_input(dmg_handle_t const handle, uint8_t input, uint8_t *output);
dmg_error_e dmg_system_load(dmg_handle_t const handle, const dmg_data_t *const data);
bool dmg_system_poll(dmg_handle_t const handle);
uint8_t dmg_system_read(dmg_handle_t const handle, uint16_t address);
dmg_error_e dmg_system_run(dmg_handle_t const handle);
dmg_error_e dmg_system_save(dmg_handle_t const handle, dmg_data_t *const data);
dmg_error_e dmg_system_setup(dmg_handle_t handle, const dmg_data_t *const data, const dmg_output_f output, dmg_palette_e palette);
uint8_t dmg_system_silence(dmg_handle_t const handle);
dmg_error_e dmg_system_sync(dmg_handle_t const handle);
void dmg_system_teardown(dmg_handle_t const handle);
void dmg_system_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_SYSTEM_H_ */