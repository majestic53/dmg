/**
 * DMG
 * Copyright (C) 2020-2021 David Jolly
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

#include "../common/buffer.h"

typedef struct {

	struct {
		uint8_t shift : 3;
		uint8_t increase : 1;
		uint8_t time : 3;
		uint8_t unused : 1;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_mode_1_sweep_t;

typedef struct {

	struct {
		uint8_t length : 6;
		uint8_t duty : 2;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_mode_1_length_t;

typedef struct {

	struct {
		uint8_t sweep : 3;
		uint8_t up : 1;
		uint8_t volume : 4;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_mode_1_envelope_t;

typedef struct {

	struct {
		uint8_t high : 3;
		uint8_t unused : 3;
		uint8_t select : 1;
		uint8_t restart : 1;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_mode_1_frequency_high_t;

typedef struct {

	struct {
		uint8_t low;
		dmg_audio_mode_1_frequency_high_t high;
	};

	uint16_t raw;
} __attribute__((packed)) dmg_audio_mode_1_frequency_t;

typedef struct {
	dmg_audio_mode_1_sweep_t sweep;
	dmg_audio_mode_1_length_t length;
	dmg_audio_mode_1_envelope_t envelope;
	dmg_audio_mode_1_frequency_t frequency;
} __attribute__((packed)) dmg_audio_mode_1_t;

typedef struct {

	struct {
		uint8_t length : 6;
		uint8_t duty : 2;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_mode_2_length_t;

typedef struct {

	struct {
		uint8_t sweep : 3;
		uint8_t up : 1;
		uint8_t volume : 4;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_mode_2_envelope_t;

typedef struct {

	struct {
		uint8_t high : 3;
		uint8_t unused : 3;
		uint8_t select : 1;
		uint8_t restart : 1;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_mode_2_frequency_high_t;

typedef struct {

	struct {
		uint8_t low;
		dmg_audio_mode_2_frequency_high_t high;
	};

	uint16_t raw;
} __attribute__((packed)) dmg_audio_mode_2_frequency_t;

typedef struct {
	dmg_audio_mode_2_length_t length;
	dmg_audio_mode_2_envelope_t envelope;
	dmg_audio_mode_2_frequency_t frequency;
} __attribute__((packed)) dmg_audio_mode_2_t;

typedef struct {

	struct {
		uint8_t unused : 7;
		uint8_t enable : 1;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_mode_3_enable_t;

typedef struct {

	struct {
		uint8_t unused_0 : 5;
		uint8_t output : 2;
		uint8_t unused_1 : 1;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_mode_3_level_t;

typedef struct {

	struct {
		uint8_t high : 3;
		uint8_t unused : 3;
		uint8_t select : 1;
		uint8_t restart : 1;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_mode_3_frequency_high_t;

typedef struct {

	struct {
		uint8_t low;
		dmg_audio_mode_3_frequency_high_t high;
	};

	uint16_t raw;
} __attribute__((packed)) dmg_audio_mode_3_frequency_t;

typedef struct {
	dmg_audio_mode_3_enable_t enable;
	uint8_t length;
	dmg_audio_mode_3_level_t level;
	dmg_audio_mode_3_frequency_t frequency;
} __attribute__((packed)) dmg_audio_mode_3_t;

typedef struct {

	struct {
		uint8_t length : 6;
		uint8_t unused : 2;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_mode_4_length_t;

typedef struct {

	struct {
		uint8_t sweep : 3;
		uint8_t up : 1;
		uint8_t volume : 4;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_mode_4_envelope_t;

typedef struct {

	struct {
		uint8_t ratio : 3;
		uint8_t select : 1;
		uint8_t shift : 4;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_mode_4_counter_polynomial_t;

typedef struct {

	struct {
		uint8_t unused_0 : 6;
		uint8_t select : 1;
		uint8_t restart : 1;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_mode_4_counter_consecutive_t;

typedef struct {
	dmg_audio_mode_4_length_t length;
	dmg_audio_mode_4_envelope_t envelope;
	dmg_audio_mode_4_counter_polynomial_t polynomial;
	dmg_audio_mode_4_counter_consecutive_t consecutive;
} __attribute__((packed)) dmg_audio_mode_4_t;

typedef struct {

	struct {
		uint8_t level_1 : 3;
		uint8_t enable_1 : 1;
		uint8_t level_2 : 3;
		uint8_t enable_2 : 1;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_control_t;

typedef struct {

	struct {
		uint8_t output_1_1 : 1;
		uint8_t output_2_1 : 1;
		uint8_t output_3_1 : 1;
		uint8_t output_4_1 : 1;
		uint8_t output_1_2 : 1;
		uint8_t output_2_2 : 1;
		uint8_t output_3_2 : 1;
		uint8_t output_4_2 : 1;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_output_t;

typedef struct {

	struct {
		uint8_t enable_1 : 1;
		uint8_t enable_2 : 1;
		uint8_t enable_3 : 1;
		uint8_t enable_4 : 1;
		uint8_t unused : 3;
		uint8_t enable : 1;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_audio_state_t;

typedef struct {
	uint32_t cycle;
	dmg_buffer_t ram;
	dmg_audio_control_t control;
	dmg_audio_mode_1_t mode_1;
	dmg_audio_mode_2_t mode_2;
	dmg_audio_mode_3_t mode_3;
	dmg_audio_mode_4_t mode_4;
	dmg_audio_output_t output;
	dmg_audio_state_t state;
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
