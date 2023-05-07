/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_INPUT_H_
#define DMG_INPUT_H_

#include <stdbool.h>
#include <dmg.h>

typedef enum
{
    DMG_BUTTON_A = 0,
    DMG_BUTTON_B,
    DMG_BUTTON_SELECT,
    DMG_BUTTON_START,
    DMG_BUTTON_RIGHT,
    DMG_BUTTON_LEFT,
    DMG_BUTTON_UP,
    DMG_BUTTON_DOWN,
    DMG_BUTTON_MAX,
} dmg_button_e;

typedef struct
{
    bool state[DMG_BUTTON_MAX];
    union
    {
        struct
        {
            uint8_t state : 4;
            uint8_t direction : 1;
            uint8_t button : 1;
        };
        uint8_t raw;
    } control;
} dmg_input_t;

uint8_t dmg_input_read(dmg_handle_t const handle, uint16_t address);
void dmg_input_set(dmg_handle_t const handle, dmg_button_e button, bool state);
void dmg_input_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_INPUT_H_ */
