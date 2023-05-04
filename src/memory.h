/*
 * DMG
 * Copyright (C) 2023 David Jolly
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef DMG_MEMORY_H_
#define DMG_MEMORY_H_

#include <stdbool.h>
#include <dmg.h>

typedef enum
{
    DMG_MAPPER_MBC0 = 0,
    DMG_MAPPER_MBC1,
    DMG_MAPPER_MBC2,
    DMG_MAPPER_MBC3,
    DMG_MAPPER_MBC5,
    DMG_MAPPER_MAX,
} dmg_mapper_e;

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
    uint8_t entry[4];
    uint8_t logo[48];
    uint8_t title[11];
    uint8_t manufacturer[4];
    uint8_t cgb;
    uint8_t licensee[2];
    uint8_t sgb;
    uint8_t id;
    uint8_t rom;
    uint8_t ram;
    uint8_t destination;
    uint8_t licensee_old;
    uint8_t version;
    uint8_t checksum;
    uint16_t checksum_global;
} dmg_memory_header_t;

typedef struct
{
    uint8_t id;
    dmg_mapper_e type;
} dmg_memory_type_t;

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

const char *dmg_memory_get_title(dmg_handle_t const handle);
dmg_error_e dmg_memory_initialize(dmg_handle_t const handle, const dmg_data_t *const data);
dmg_error_e dmg_memory_load(dmg_handle_t const handle, const dmg_data_t *const data);
uint8_t dmg_memory_read(dmg_handle_t const handle, uint16_t address);
dmg_error_e dmg_memory_save(dmg_handle_t const handle, dmg_data_t *const data);
void dmg_memory_uninitialize(dmg_handle_t const handle);
void dmg_memory_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_MEMORY_H_ */
