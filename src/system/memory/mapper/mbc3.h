/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_MBC3_H_
#define DMG_MBC3_H_

#include <common.h>

typedef union
{
    struct
    {
        uint16_t counter : 9;
        uint16_t : 5;
        uint16_t halt : 1;
        uint16_t carry : 1;
    };
    struct
    {
        uint8_t low;
        uint8_t high;
    };
} dmg_rtc_day_t;

typedef union
{
    struct
    {
        uint8_t counter : 5;
    };
    uint8_t raw;
} dmg_rtc_hour_t;

typedef union
{
    struct
    {
        uint8_t counter : 6;
    };
    uint8_t raw;
} dmg_rtc_minute_t;

typedef union
{
    struct
    {
        uint8_t counter : 6;
    };
    uint8_t raw;
} dmg_rtc_second_t;

typedef struct
{
    dmg_rtc_second_t second;
    dmg_rtc_minute_t minute;
    dmg_rtc_hour_t hour;
    dmg_rtc_day_t day;
} dmg_rtc_t;

typedef struct
{
    bool enabled;
    struct
    {
        uint8_t ram;
        uint8_t rom;
        uint8_t rtc;
    } bank;
    struct
    {
        uint16_t bank;
    } ram;
    struct
    {
        uint16_t bank[2];
    } rom;
    struct
    {
        int16_t delay;
        bool enabled;
        bool latched;
        dmg_rtc_t counter[2];
    } rtc;
} dmg_mbc3_t;

void dmg_mbc3_initialize(dmg_t const dmg, bool enabled);
void dmg_mbc3_interrupt(dmg_t const dmg);
void dmg_mbc3_load(dmg_t const dmg, const void *const data, uint32_t length);
uint8_t dmg_mbc3_read(dmg_t const dmg, uint16_t address);
void dmg_mbc3_save(dmg_t const dmg, void *const data, uint32_t length);
void dmg_mbc3_write(dmg_t const dmg, uint16_t address, uint8_t value);

#endif /* DMG_MBC3_H_ */
