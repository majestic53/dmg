/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_MEMORY_H_
#define DMG_MEMORY_H_

#include <stdbool.h>
#include <dmg.h>

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

typedef struct
{
    struct
    {
        uint8_t ram;
        struct
        {
            uint8_t low;
            uint8_t high;
        } rom;
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
} dmg_mbc5_t;

typedef struct
{
    struct
    {
        bool enabled;
    } bootloader;
    struct
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
    } cartridge;
    struct
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
    } mapper;
    struct
    {
        uint8_t high[0x80];
        uint8_t work[0x2000];
    } ram;
} dmg_memory_t;

dmg_error_t dmg_memory_initialize(dmg_handle_t const handle, const dmg_data_t *const data);
dmg_error_t dmg_memory_load(dmg_handle_t const handle, const dmg_data_t *const data);
uint8_t dmg_memory_read(dmg_handle_t const handle, uint16_t address);
dmg_error_t dmg_memory_save(dmg_handle_t const handle, dmg_data_t *const data);
const char *dmg_memory_title(dmg_handle_t const handle);
void dmg_memory_uninitialize(dmg_handle_t const handle);
void dmg_memory_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_MEMORY_H_ */
