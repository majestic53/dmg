/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_MBC0_H_
#define DMG_MBC0_H_

#include <common.h>

uint8_t dmg_mbc0_read(dmg_t const dmg, uint16_t address);
void dmg_mbc0_write(dmg_t const dmg, uint16_t address, uint8_t value);

#endif /* DMG_MBC0_H_ */
