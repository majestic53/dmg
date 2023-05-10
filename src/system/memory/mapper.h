/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_MAPPER_H_
#define DMG_MAPPER_H_

#include <mbc0.h>
#include <mbc1.h>
#include <mbc2.h>
#include <mbc3.h>
#include <mbc5.h>

typedef struct
{
    uint8_t (*read)(dmg_handle_t const handle, uint16_t address);
    void (*write)(dmg_handle_t const handle, uint16_t address, uint8_t value);
    union
    {
        dmg_mbc1_t mbc1;
        dmg_mbc2_t mbc2;
        dmg_mbc3_t mbc3;
        dmg_mbc5_t mbc5;
    };
} dmg_mapper_t;

dmg_error_e dmg_mapper_initialize(dmg_handle_t const handle, uint8_t id);
uint8_t dmg_mapper_read(dmg_handle_t const handle, uint16_t address);
void dmg_mapper_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_MAPPER_H_ */
