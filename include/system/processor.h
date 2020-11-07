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

#ifndef DMG_SYSTEM_PROCESSOR_H_
#define DMG_SYSTEM_PROCESSOR_H_

#include "../common.h"

typedef union {

	struct {
		uint8_t unused : 4;
		uint8_t carry : 1;
		uint8_t carry_half : 1;
		uint8_t subtract : 1;
		uint8_t zero : 1;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_processor_flag_t;

typedef union {

	struct {
		uint8_t vblank : 1;
		uint8_t lcdc : 1;
		uint8_t timer : 1;
		uint8_t serial : 1;
		uint8_t joypad : 1;
		uint8_t unused : 3;
	};

	uint8_t raw;
} __attribute__((packed)) dmg_processor_interrupt_t;

typedef union {

	union {

		struct {

			union {

				struct {
					uint8_t low_lsb : 1;
					uint8_t low_unused : 6;
					uint8_t low_msb : 1;
				};

				dmg_processor_flag_t flag;
				uint8_t low;
			};

			union {

				struct {
					uint8_t high_lsb : 1;
					uint8_t high_unused : 6;
					uint8_t high_msb : 1;
				};

				uint8_t high;
			};
		};

		uint16_t word;
	};

	uint32_t dword;
} __attribute__((packed)) dmg_processor_register_t;

typedef struct {
	dmg_processor_register_t af;
	dmg_processor_register_t bc;
	dmg_processor_register_t de;
	dmg_processor_register_t hl;
	bool interrupts_enable;
	int interrupts_enable_state;
	dmg_processor_interrupt_t interrupt_enable;
	dmg_processor_interrupt_t interrupt_flag;
	bool halt;
	dmg_processor_register_t pc;
	dmg_processor_register_t sp;
	bool stop;
} __attribute__((packed)) dmg_processor_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_processor_export(
	__in const dmg_processor_t *processor,
	__in FILE *file
	);

int dmg_processor_import(
	__inout dmg_processor_t *processor,
	__in FILE *file
	);

int dmg_processor_load(
	__inout dmg_processor_t *processor,
	__in const dmg_t *configuration
	);

uint8_t dmg_processor_read(
	__in const dmg_processor_t *processor,
	__in uint16_t address
	);

uint32_t dmg_processor_step(
	__inout dmg_processor_t *processor
	);

void dmg_processor_unload(
	__inout dmg_processor_t *processor
	);

void dmg_processor_write(
	__inout dmg_processor_t *processor,
	__in uint16_t address,
	__in uint8_t value
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_SYSTEM_PROCESSOR_H_ */
