/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_SERIAL_H_
#define DMG_SERIAL_H_

#include <dmg.h>

typedef struct
{
    uint8_t data;
    uint16_t delay;
    uint8_t index;
    dmg_output_f output;
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
dmg_error_e dmg_serial_initialize(dmg_handle_t const handle, const dmg_output_f output);
uint8_t dmg_serial_input(dmg_handle_t const handle, uint8_t value);
uint8_t dmg_serial_read(dmg_handle_t const handle, uint16_t address);
void dmg_serial_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_SERIAL_H_ */
