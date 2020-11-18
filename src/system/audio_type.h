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

#ifndef DMG_SYSTEM_AUDIO_TYPE_H_
#define DMG_SYSTEM_AUDIO_TYPE_H_

#include "../../include/system/audio.h"
#include "../../include/runtime.h"
#include "../../include/service.h"

#define POST_CONTROL 0x77
#define POST_MODE_1_SWEEP 0x80
#define POST_MODE_1_LENGTH 0xbf
#define POST_MODE_1_ENVELOPE 0xf3
#define POST_MODE_1_FREQUENCY_HIGH 0xbf
#define POST_MODE_2_LENGTH 0x3f
#define POST_MODE_2_ENVELOPE 0x00
#define POST_MODE_2_FREQUENCY_HIGH 0xbf
#define POST_MODE_3_ENABLE 0x7f
#define POST_MODE_3_LENGTH 0xff
#define POST_MODE_3_LEVEL 0x9f
#define POST_MODE_3_FREQUENCY_LOW 0xbf
#define POST_MODE_4_LENGTH 0xff
#define POST_MODE_4_ENVELOPE 0x00
#define POST_MODE_4_COUNTER_POLYNOMIAL 0x00
#define POST_MODE_4_COUNTER_CONSECUTIVE 0xbf
#define POST_OUTPUT 0xf3
#define POST_STATE 0xf1

#define RAM_WIDTH ADDRESS_WIDTH(ADDRESS_AUDIO_RAM_BEGIN, ADDRESS_AUDIO_RAM_END)

#ifndef NDEBUG
#define TRACE_AUDIO(_LEVEL_, _AUDIO_) \
	if((_LEVEL_) <= (LEVEL)) { \
		dmg_audio_trace(_LEVEL_, _AUDIO_); \
	}
#else
#define TRACE_AUDIO(_LEVEL_, _AUDIO_)
#endif /* NDEBUG */

#endif /* DMG_SYSTEM_AUDIO_TYPE_H_ */
