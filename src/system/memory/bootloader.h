/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_BOOTLOADER_H_
#define DMG_BOOTLOADER_H_

#include <common.h>

typedef struct
{
    bool enabled;
} dmg_bootloader_t;

bool dmg_bootloader_enabled(dmg_t const dmg);
void dmg_bootloader_initialize(dmg_t const dmg);
uint8_t dmg_bootloader_read(dmg_t const dmg, uint16_t address);
void dmg_bootloader_write(dmg_t const dmg, uint16_t address, uint8_t value);

#endif /* DMG_BOOTLOADER_H_ */
