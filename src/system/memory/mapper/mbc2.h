/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_MBC2_H_
#define DMG_MBC2_H_

#include <common.h>

typedef struct
{
    struct
    {
        uint8_t rom;
    } bank;
    struct
    {
        uint8_t data[512];
        bool enabled;
    } ram;
    struct
    {
        uint16_t bank[2];
    } rom;
} dmg_mbc2_t;

void dmg_mbc2_initialize(dmg_t const dmg);
uint8_t dmg_mbc2_read(dmg_t const dmg, uint16_t address);
void dmg_mbc2_write(dmg_t const dmg, uint16_t address, uint8_t value);

#endif /* DMG_MBC2_H_ */
