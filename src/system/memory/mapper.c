/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

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
    uint8_t id;
    dmg_mapper_e type;
} dmg_mapper_type_t;

static const dmg_mapper_type_t TYPE[] =
{
    /* MBC0 */
    { 0, DMG_MAPPER_MBC0, }, { 8, DMG_MAPPER_MBC0, }, { 9, DMG_MAPPER_MBC0, },
    /* MBC1 */
    { 1, DMG_MAPPER_MBC1, }, { 2, DMG_MAPPER_MBC1, }, { 3, DMG_MAPPER_MBC1, },
    /* MBC2 */
    { 5, DMG_MAPPER_MBC2, }, { 6, DMG_MAPPER_MBC2, },
    /* MBC3 */
    { 15, DMG_MAPPER_MBC3, }, { 16, DMG_MAPPER_MBC3, }, { 17, DMG_MAPPER_MBC3, }, { 18, DMG_MAPPER_MBC3, },
    { 19, DMG_MAPPER_MBC3, },
    /* MBC5 */
    { 25, DMG_MAPPER_MBC5, }, { 26, DMG_MAPPER_MBC5, }, { 27, DMG_MAPPER_MBC5, }, { 28, DMG_MAPPER_MBC5, },
    { 29, DMG_MAPPER_MBC5, }, { 30, DMG_MAPPER_MBC5, },
};

static dmg_mapper_e dmg_mapper_type(uint8_t id)
{
    dmg_mapper_e result = DMG_MAPPER_MAX;
    for (uint16_t index = 0; index < sizeof (TYPE) / sizeof (*TYPE); ++index)
    {
        if (TYPE[index].id == id)
        {
            result = TYPE[index].type;
            break;
        }
    }
    return result;
}

dmg_error_e dmg_mapper_initialize(dmg_handle_t const handle, uint8_t id)
{
    dmg_error_e result = DMG_SUCCESS;
    switch (dmg_mapper_type(id))
    {
        case DMG_MAPPER_MBC0:
            handle->memory.mapper.read = dmg_mbc0_read;
            handle->memory.mapper.write = dmg_mbc0_write;
            break;
        case DMG_MAPPER_MBC1:
            dmg_mbc1_update(handle);
            handle->memory.mapper.read = dmg_mbc1_read;
            handle->memory.mapper.write = dmg_mbc1_write;
            break;
        case DMG_MAPPER_MBC2:
            dmg_mbc2_update(handle);
            handle->memory.mapper.read = dmg_mbc2_read;
            handle->memory.mapper.write = dmg_mbc2_write;
            break;
        case DMG_MAPPER_MBC3:
            dmg_mbc3_update(handle);
            handle->memory.mapper.read = dmg_mbc3_read;
            handle->memory.mapper.write = dmg_mbc3_write;
            break;
        case DMG_MAPPER_MBC5:
            handle->memory.mapper.mbc5.bank.rom.low = 1;
            dmg_mbc5_update(handle);
            handle->memory.mapper.read = dmg_mbc5_read;
            handle->memory.mapper.write = dmg_mbc5_write;
            break;
        default:
            result = DMG_ERROR(handle, "Unsupported mapper type -- %u", id);
            break;
    }
    return result;
}

uint8_t dmg_mapper_read(dmg_handle_t const handle, uint16_t address)
{
    return handle->memory.mapper.read(handle, address);
}

void dmg_mapper_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    handle->memory.mapper.write(handle, address, value);
}
