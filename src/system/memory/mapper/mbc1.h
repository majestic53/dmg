/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_MBC1_H_
#define DMG_MBC1_H_

#include <stdbool.h>
#include <common.h>

typedef struct
{
    struct
    {
        uint8_t low;
        uint8_t high;
        uint8_t select;
    } bank;
    struct
    {
        uint16_t bank;
        bool enabled;
    } ram;
    struct
    {
        uint16_t bank[2];
    } rom;
} dmg_mbc1_t;

uint8_t dmg_mbc1_read(dmg_handle_t const handle, uint16_t address);
void dmg_mbc1_update(dmg_handle_t const handle);
void dmg_mbc1_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_MBC1_H_ */
