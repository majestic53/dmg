/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_SYSTEM_H_
#define DMG_SYSTEM_H_

#include <audio.h>
#include <input.h>
#include <memory.h>
#include <processor.h>
#include <serial.h>
#include <service.h>
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
    dmg_service_t service;
    dmg_timer_t timer;
    dmg_video_t video;
} dmg_system_t;

uint8_t dmg_system_read(dmg_handle_t const handle, uint16_t address);
void dmg_system_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_SYSTEM_H_ */
