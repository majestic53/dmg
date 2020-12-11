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

#ifndef DMG_H_
#define DMG_H_

/**
 * Action enum
 */
enum {
	/* No action */
	DMG_ACTION_NOP = 0,
	/* Retrieve cycle count */
	DMG_ACTION_CYCLE,
	/* Read byte */
	DMG_ACTION_READ,
	/* Serial transfer-in */
	DMG_ACTION_SERIAL_IN,
	/* Write byte */
	DMG_ACTION_WRITE,
	DMG_ACTION_MAX,
};

/**
 * Button key enum
 */
enum {
	DMG_BUTTON_A = 0,
	DMG_BUTTON_B,
	DMG_BUTTON_SELECT,
	DMG_BUTTON_START,
	DMG_BUTTON_MAX,
};

/**
 * Direction key enum
 */
enum {
	DMG_DIRECTION_RIGHT = 0,
	DMG_DIRECTION_LEFT,
	DMG_DIRECTION_UP,
	DMG_DIRECTION_DOWN,
	DMG_DIRECTION_MAX,
};

/**
 * Palette enum
 */
enum {
	DMG_PALETTE_WHITE = 0,
	DMG_PALETTE_GREY_LIGHT,
	DMG_PALETTE_GREY_DARK,
	DMG_PALETTE_BLACK,
	DMG_PALETTE_MAX,
};

/**
 * Register enum
 */
enum {
	DMG_REGISTER_PROCESSOR_A = 0,
	DMG_REGISTER_PROCESSOR_AF,
	DMG_REGISTER_PROCESSOR_B,
	DMG_REGISTER_PROCESSOR_BC,
	DMG_REGISTER_PROCESSOR_C,
	DMG_REGISTER_PROCESSOR_D,
	DMG_REGISTER_PROCESSOR_DE,
	DMG_REGISTER_PROCESSOR_E,
	DMG_REGISTER_PROCESSOR_F,
	DMG_REGISTER_PROCESSOR_H,
	DMG_REGISTER_PROCESSOR_HALT,
	DMG_REGISTER_PROCESSOR_HL,
	DMG_REGISTER_PROCESSOR_IE,
	DMG_REGISTER_PROCESSOR_IF,
	DMG_REGISTER_PROCESSOR_IME,
	DMG_REGISTER_PROCESSOR_L,
	DMG_REGISTER_PROCESSOR_PC,
	DMG_REGISTER_PROCESSOR_SP,
	DMG_REGISTER_PROCESSOR_STOP,
	DMG_REGISTER_MAX,
};

/**
 * Status enum
 */
enum {
	DMG_STATUS_SUCCESS = 0,
	DMG_STATUS_FAILURE,
	DMG_STATUS_INVALID,
	DMG_STATUS_MAX,
	/* Event-specific status */
	DMG_STATUS_BREAKPOINT = -1,
};

/**
 * Serial transfer-out event handler
 * @param Input bit
 * @return Output bit
 */
typedef unsigned (*dmg_serial_out)(unsigned);

/**
 * Action struct
 */
typedef struct {
	/* Action id */
	uint8_t id;
	/* Action type */
	uint8_t type;
	/* Action data length */
	uint8_t length;
	/* Action address */
	uint16_t address;

	/* Action data */
	union {

		union {

			struct {
				uint8_t unused : 4;
				uint8_t carry : 1;
				uint8_t carry_half : 1;
				uint8_t subtract : 1;
				uint8_t zero : 1;
			} flag;

			struct {
				uint8_t vblank : 1;
				uint8_t lcdc : 1;
				uint8_t timer : 1;
				uint8_t serial : 1;
				uint8_t joypad : 1;
				uint8_t unused : 3;
			} interrupt;

			uint8_t byte;
		};

		union {

			struct {
				uint8_t low;
				uint8_t high;
			};

			uint16_t word;
		};

		uint32_t dword;
	} data;
} __attribute__((packed)) dmg_action_t;

/**
 * Buffer struct
 */
typedef struct {
	/* Data pointer */
	void *data;
	/* Data length */
	unsigned length;
} __attribute__((packed)) dmg_buffer_t;

/**
 * Version struct
 */
typedef struct {
	/* Major version */
	unsigned major;
	/* Minor version */
	unsigned minor;
	/* Patch version */
	unsigned patch;
} __attribute__((packed)) dmg_version_t;

/**
 * Configuration struct
 */
typedef struct {
	/* Bootrom buffer */
	dmg_buffer_t bootrom;
	/* Rom buffer */
	dmg_buffer_t rom;
	/* Serial transfer-out callback */
	dmg_serial_out serial_out;
	/* Button key-bindings */
	unsigned button[DMG_BUTTON_MAX];
	/* Direction key-bindings */
	unsigned direction[DMG_DIRECTION_MAX];
	/* Display palette */
	unsigned palette[DMG_PALETTE_MAX];
	/* Display scale */
	unsigned scale;
	/* Input save file path */
	const char *save_in;
	/* Output save file path */
	const char *save_out;
} __attribute__((packed)) dmg_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************
 * Load/Unload Routines
 *****************************************/

/**
 * Load emulator instance
 * @param Const pointer to configuration struct
 * @return Emulator status
 */
int dmg_load(const dmg_t *);

/**
 * Unload emulator instance
 */
void dmg_unload(void);

/******************************************
 * Runtime Routines
 *****************************************/

/**
 * Send emulator instance action request
 * @param Const pointer to action request struct
 * @param Pointer to action response struct
 * @return Emulator status
 */
int dmg_action(const dmg_action_t *, dmg_action_t *);

/**
 * Run emulator instance
 * @param Const pointer to breakpoint addresses
 * @param Breakpoint address count
 * @return Emulator status
 */
int dmg_run(const unsigned short *, unsigned);

/**
 * Step emulator instance
 * @param Instruction count
 * @param Const pointer to breakpoint addresses
 * @param Breakpoint address count
 * @return Emulator status
 */
int dmg_step(unsigned, const unsigned short *, unsigned);

/******************************************
 * Helper Routines
 *****************************************/

/**
 * Retrieve emulator instance error
 * @return Const pointer to error string
 */
const char *dmg_error(void);

/**
 * Retrieve emulator instance version
 * @return Const pointer to version struct
 */
const dmg_version_t *dmg_version(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_H_ */
