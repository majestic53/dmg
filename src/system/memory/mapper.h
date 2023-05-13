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

typedef union
{
    struct
    {
        uint8_t rtc : 1;
    };
    uint8_t raw;
} dmg_attribute_t;

typedef struct
{
    const dmg_attribute_t *attribute;
    void (*clock)(dmg_handle_t const handle);
    void (*load)(dmg_handle_t const handle, const void *const data, uint32_t length);
    uint8_t (*read)(dmg_handle_t const handle, uint16_t address);
    void (*save)(dmg_handle_t const handle, void *const data, uint32_t length);
    void (*write)(dmg_handle_t const handle, uint16_t address, uint8_t value);
    union
    {
        dmg_mbc1_t mbc1;
        dmg_mbc2_t mbc2;
        dmg_mbc3_t mbc3;
        dmg_mbc5_t mbc5;
    };
} dmg_mapper_t;

const dmg_attribute_t *dmg_mapper_attribute(dmg_handle_t const handle);
void dmg_mapper_clock(dmg_handle_t const handle);
dmg_error_e dmg_mapper_initialize(dmg_handle_t const handle, uint8_t id);
void dmg_mapper_load(dmg_handle_t const handle, const void *const data, uint32_t length);
uint8_t dmg_mapper_read(dmg_handle_t const handle, uint16_t address);
void dmg_mapper_save(dmg_handle_t const handle, void *const data, uint32_t length);
void dmg_mapper_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_MAPPER_H_ */
