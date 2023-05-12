/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_MBC3_H_
#define DMG_MBC3_H_

#include <common.h>

typedef struct
{
    struct
    {
        uint8_t ram;
        uint8_t rom;
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
} dmg_mbc3_t;

void dmg_mbc3_initialize(dmg_handle_t const handle);
uint8_t dmg_mbc3_read(dmg_handle_t const handle, uint16_t address);
void dmg_mbc3_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_MBC3_H_ */
