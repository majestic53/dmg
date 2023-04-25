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

#ifndef DMG_SERIAL_H_
#define DMG_SERIAL_H_

#include <common.h>

typedef struct
{
    uint8_t data;
    uint16_t delay;
    uint8_t index;
    union
    {
        struct
        {
            uint8_t mode : 1;
            uint8_t : 6;
            uint8_t enabled : 1;
        };
        uint8_t raw;
    } control;
} dmg_serial_t;

void dmg_serial_clock(dmg_handle_t const handle);
uint8_t dmg_serial_read(dmg_handle_t const handle, uint16_t address);
void dmg_serial_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_SERIAL_H_ */
