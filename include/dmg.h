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

#ifndef DMG_H_
#define DMG_H_

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
 * Serial transfer callback
 * @param Input bit
 * @return Output bit
 */
typedef unsigned (*dmg_serial_transfer)(unsigned);

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
	/* Serial transfer callback */
	dmg_serial_transfer transfer;
	/* Button key-bindings */
	unsigned button[DMG_BUTTON_MAX];
	/* Direction key-bindings */
	unsigned direction[DMG_DIRECTION_MAX];
	/* Display palette */
	unsigned palette[DMG_PALETTE_MAX];
	/* Display scale */
	unsigned scale;
	/* Input save file path */
	char *save_in;
	/* Output save file path */
	char *save_out;
} __attribute__((packed)) dmg_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Run emulator
 * @param Const pointer to configuration struct
 * @return EXIT_SUCCESS on success
 */
int dmg(const dmg_t *);

/**
 * Retrieve emulator error
 * @return Const pointer to error string
 */
const char *dmg_error(void);

/**
 * Retrieve emulator version
 * @return Const pointer to version struct
 */
const dmg_version_t *dmg_version(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_H_ */
