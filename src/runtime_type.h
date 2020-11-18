/**
 * DMG
 * Copyright (C) 2020 David Jolly
 *
 * DMG is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DMG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DMG_RUNTIME_TYPE_H_
#define DMG_RUNTIME_TYPE_H_

#include "../include/system/audio.h"
#include "../include/system/joypad.h"
#include "../include/system/memory.h"
#include "../include/system/processor.h"
#include "../include/system/serial.h"
#include "../include/system/timer.h"
#include "../include/system/video.h"
#include "../include/runtime.h"
#include "../include/service.h"

typedef struct {
	dmg_audio_t audio;
	dmg_joypad_t joypad;
	dmg_memory_t memory;
	dmg_processor_t processor;
	dmg_serial_t serial;
	dmg_timer_t timer;
	dmg_video_t video;
} __attribute__((packed)) dmg_runtime_t;

#endif /* DMG_RUNTIME_TYPE_H_ */
