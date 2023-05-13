/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_SYSTEM_H_
#define DMG_SYSTEM_H_

#include <audio.h>
#include <controller.h>
#include <memory.h>
#include <processor.h>
#include <serial.h>
#include <timer.h>
#include <video.h>

struct dmg_s
{
    char error[256];
    bool initialized;
    dmg_audio_t audio;
    dmg_controller_t controller;
    dmg_memory_t memory;
    dmg_processor_t processor;
    dmg_serial_t serial;
    dmg_timer_t timer;
    dmg_video_t video;
    struct
    {
        uint32_t tick;
        uint32_t pixel[432][480];
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

dmg_error_e dmg_system_initialize(dmg_t dmg, const dmg_data_t *const data, const dmg_output_f output);
dmg_error_e dmg_system_input(dmg_t const dmg, uint8_t input, uint8_t *output);
dmg_error_e dmg_system_load(dmg_t const dmg, const dmg_data_t *const data);
bool dmg_system_poll(dmg_t const dmg);
uint8_t dmg_system_read(dmg_t const dmg, uint16_t address);
dmg_error_e dmg_system_run(dmg_t const dmg);
uint8_t dmg_system_silence(dmg_t const dmg);
dmg_error_e dmg_system_save(dmg_t const dmg, dmg_data_t *const data);
dmg_error_e dmg_system_sync(dmg_t const dmg);
void dmg_system_uninitialize(dmg_t const dmg);
void dmg_system_write(dmg_t const dmg, uint16_t address, uint8_t value);

#endif /* DMG_SYSTEM_H_ */
