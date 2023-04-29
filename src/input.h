/*
 * DMG
 * Copyright (C) 2023 David Jolly
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef DMG_INPUT_H_
#define DMG_INPUT_H_

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
