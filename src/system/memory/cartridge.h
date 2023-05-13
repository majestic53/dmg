/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_CARTRIDGE_H_
#define DMG_CARTRIDGE_H_

#include <common.h>

typedef struct
{
    char title[12];
    struct
    {
        uint16_t count;
        uint8_t *data;
    } ram;
    struct
    {
        uint16_t count;
        const uint8_t *data;
    } rom;
} dmg_cartridge_t;

dmg_error_e dmg_cartridge_initialize(dmg_t const dmg, const dmg_data_t *const data);
dmg_error_e dmg_cartridge_load(dmg_t const dmg, const dmg_data_t *const data);
uint8_t dmg_cartridge_read_ram(dmg_t const dmg, uint16_t bank, uint16_t address);
uint8_t dmg_cartridge_read_rom(dmg_t const dmg, uint16_t bank, uint16_t address);
dmg_error_e dmg_cartridge_save(dmg_t const dmg, dmg_data_t *const data);
const char *dmg_cartridge_title(dmg_t const dmg);
void dmg_cartridge_uninitialize(dmg_t const dmg);
void dmg_cartridge_write_ram(dmg_t const dmg, uint16_t bank, uint16_t address, uint8_t value);

#endif /* DMG_CARTRIDGE_H_ */
