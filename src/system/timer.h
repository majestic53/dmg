/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_TIMER_H_
#define DMG_TIMER_H_

#include <stdbool.h>
#include <common.h>

typedef struct
{
    uint8_t counter;
    uint16_t divider;
    uint8_t modulo;
    bool overflow[2];
    union
    {
        struct
        {
            uint8_t mode : 2;
            uint8_t enabled : 1;
        };
        uint8_t raw;
    } control;
} dmg_timer_t;

void dmg_timer_clock(dmg_handle_t const handle);
uint8_t dmg_timer_read(dmg_handle_t const handle, uint16_t address);
void dmg_timer_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_TIMER_H_ */
