/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_MEMORY_H_
#define DMG_MEMORY_H_

#include <bootloader.h>
#include <cartridge.h>
#include <mapper.h>

typedef struct
{
    dmg_bootloader_t bootloader;
    dmg_cartridge_t cartridge;
    dmg_mapper_t mapper;
    struct
    {
        uint8_t high[0x80];
        uint8_t work[0x2000];
    } ram;
} dmg_memory_t;

dmg_error_e dmg_memory_initialize(dmg_t const dmg, const dmg_data_t *const data);
uint8_t dmg_memory_read(dmg_t const dmg, uint16_t address);
void dmg_memory_write(dmg_t const dmg, uint16_t address, uint8_t value);

#endif /* DMG_MEMORY_H_ */
