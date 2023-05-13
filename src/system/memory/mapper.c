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
    dmg_attribute_t attribute;
} dmg_mapper_type_t;

static const dmg_mapper_type_t TYPE[] =
{
    /* MBC0 */
    { 0, DMG_MAPPER_MBC0, { .rtc = false }, },
    { 8, DMG_MAPPER_MBC0, { .rtc = false }, },
    { 9, DMG_MAPPER_MBC0, { .rtc = false }, },
    /* MBC1 */
    { 1, DMG_MAPPER_MBC1, { .rtc = false }, },
    { 2, DMG_MAPPER_MBC1, { .rtc = false }, },
    { 3, DMG_MAPPER_MBC1, { .rtc = false }, },
    /* MBC2 */
    { 5, DMG_MAPPER_MBC2, { .rtc = false }, },
    { 6, DMG_MAPPER_MBC2, { .rtc = false }, },
    /* MBC3 */
    { 15, DMG_MAPPER_MBC3, { .rtc = true }, },
    { 16, DMG_MAPPER_MBC3, { .rtc = true }, },
    { 17, DMG_MAPPER_MBC3, { .rtc = false }, },
    { 18, DMG_MAPPER_MBC3, { .rtc = false }, },
    { 19, DMG_MAPPER_MBC3, { .rtc = false }, },
    /* MBC5 */
    { 25, DMG_MAPPER_MBC5, { .rtc = false }, },
    { 26, DMG_MAPPER_MBC5, { .rtc = false }, },
    { 27, DMG_MAPPER_MBC5, { .rtc = false }, },
    { 28, DMG_MAPPER_MBC5, { .rtc = false }, },
    { 29, DMG_MAPPER_MBC5, { .rtc = false }, },
    { 30, DMG_MAPPER_MBC5, { .rtc = false }, },
};

static dmg_mapper_e dmg_mapper_type(uint8_t id, const dmg_attribute_t **attribute)
{
    dmg_mapper_e result = DMG_MAPPER_MAX;
    for (uint16_t index = 0; index < sizeof (TYPE) / sizeof (*TYPE); ++index)
    {
        const dmg_mapper_type_t *mapper = &TYPE[index];
        if (mapper->id == id)
        {
            result = mapper->type;
            *attribute = &mapper->attribute;
            break;
        }
    }
    return result;
}

const dmg_attribute_t *dmg_mapper_attribute(dmg_handle_t const handle)
{
    return handle->memory.mapper.attribute;
}

void dmg_mapper_clock(dmg_handle_t const handle)
{
    if (handle->memory.mapper.clock)
    {
        handle->memory.mapper.clock(handle);
    }
}

dmg_error_e dmg_mapper_initialize(dmg_handle_t const handle, uint8_t id)
{
    dmg_error_e result = DMG_SUCCESS;
    switch (dmg_mapper_type(id, &handle->memory.mapper.attribute))
    {
        case DMG_MAPPER_MBC0:
            handle->memory.mapper.read = dmg_mbc0_read;
            handle->memory.mapper.write = dmg_mbc0_write;
            break;
        case DMG_MAPPER_MBC1:
            dmg_mbc1_initialize(handle);
            handle->memory.mapper.read = dmg_mbc1_read;
            handle->memory.mapper.write = dmg_mbc1_write;
            break;
        case DMG_MAPPER_MBC2:
            dmg_mbc2_initialize(handle);
            handle->memory.mapper.read = dmg_mbc2_read;
            handle->memory.mapper.write = dmg_mbc2_write;
            break;
        case DMG_MAPPER_MBC3:
            dmg_mbc3_initialize(handle, handle->memory.mapper.attribute->rtc);
            if (handle->memory.mapper.attribute->rtc)
            {
                handle->memory.mapper.clock = dmg_mbc3_clock;
                handle->memory.mapper.load = dmg_mbc3_load;
                handle->memory.mapper.save = dmg_mbc3_save;
            }
            handle->memory.mapper.read = dmg_mbc3_read;
            handle->memory.mapper.write = dmg_mbc3_write;
            break;
        case DMG_MAPPER_MBC5:
            dmg_mbc5_initialize(handle);
            handle->memory.mapper.read = dmg_mbc5_read;
            handle->memory.mapper.write = dmg_mbc5_write;
            break;
        default:
            result = DMG_ERROR(handle, "Unsupported mapper type -- %u", id);
            break;
    }
    return result;
}

void dmg_mapper_load(dmg_handle_t const handle, const void *const data, uint32_t length)
{
    if (handle->memory.mapper.load)
    {
        handle->memory.mapper.load(handle, data, length);
    }
}

uint8_t dmg_mapper_read(dmg_handle_t const handle, uint16_t address)
{
    return handle->memory.mapper.read(handle, address);
}

void dmg_mapper_save(dmg_handle_t const handle, void *const data, uint32_t length)
{
    if (handle->memory.mapper.save)
    {
        handle->memory.mapper.save(handle, data, length);
    }
}

void dmg_mapper_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    handle->memory.mapper.write(handle, address, value);
}
