/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_CONTROLLER_H_
#define DMG_CONTROLLER_H_

#include <common.h>

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
} dmg_controller_t;

uint8_t dmg_controller_read(dmg_t const dmg, uint16_t address);
void dmg_controller_update(dmg_t const dmg, dmg_button_e button, bool state);
void dmg_controller_write(dmg_t const dmg, uint16_t address, uint8_t value);

#endif /* DMG_CONTROLLER_H_ */
