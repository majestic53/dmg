/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_MBC1_H_
#define DMG_MBC1_H_

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

void dmg_mbc1_initialize(dmg_t const dmg);
uint8_t dmg_mbc1_read(dmg_t const dmg, uint16_t address);
void dmg_mbc1_write(dmg_t const dmg, uint16_t address, uint8_t value);

#endif /* DMG_MBC1_H_ */
