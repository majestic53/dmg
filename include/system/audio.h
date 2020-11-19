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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DMG_SYSTEM_AUDIO_H_
#define DMG_SYSTEM_AUDIO_H_

#include "../type/buffer.h"

// TODO

typedef struct {
	uint32_t cycle;
	dmg_buffer_t ram;

	// TODO

} __attribute__((packed)) dmg_audio_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_audio_export(
	__in const dmg_audio_t *audio,
	__in FILE *file
	);

int dmg_audio_import(
	__inout dmg_audio_t *audio,
	__in FILE *file
	);

int dmg_audio_load(
	__inout dmg_audio_t *audio,
	__in const dmg_t *configuration
	);

uint8_t dmg_audio_read(
	__in const dmg_audio_t *audio,
	__in uint16_t address
	);

void dmg_audio_step(
	__inout dmg_audio_t *audio,
	__in uint32_t cycle
	);

void dmg_audio_unload(
	__inout dmg_audio_t *audio
	);

void dmg_audio_write(
	__inout dmg_audio_t *audio,
	__in uint16_t address,
	__in uint8_t value
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_SYSTEM_AUDIO_H_ */
