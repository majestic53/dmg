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
    { 0, DMG_MAPPER_MBC0, { .timer = false }, },
    { 8, DMG_MAPPER_MBC0, { .timer = false }, },
    { 9, DMG_MAPPER_MBC0, { .timer = false }, },
    /* MBC1 */
    { 1, DMG_MAPPER_MBC1, { .timer = false }, },
    { 2, DMG_MAPPER_MBC1, { .timer = false }, },
    { 3, DMG_MAPPER_MBC1, { .timer = false }, },
    /* MBC2 */
    { 5, DMG_MAPPER_MBC2, { .timer = false }, },
    { 6, DMG_MAPPER_MBC2, { .timer = false }, },
    /* MBC3 */
    { 15, DMG_MAPPER_MBC3, { .timer = true }, },
    { 16, DMG_MAPPER_MBC3, { .timer = true }, },
    { 17, DMG_MAPPER_MBC3, { .timer = false }, },
    { 18, DMG_MAPPER_MBC3, { .timer = false }, },
    { 19, DMG_MAPPER_MBC3, { .timer = false }, },
    /* MBC5 */
    { 25, DMG_MAPPER_MBC5, { .timer = false }, },
    { 26, DMG_MAPPER_MBC5, { .timer = false }, },
    { 27, DMG_MAPPER_MBC5, { .timer = false }, },
    { 28, DMG_MAPPER_MBC5, { .timer = false }, },
    { 29, DMG_MAPPER_MBC5, { .timer = false }, },
    { 30, DMG_MAPPER_MBC5, { .timer = false }, },
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
            dmg_mbc3_initialize(handle);
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

uint8_t dmg_mapper_read(dmg_handle_t const handle, uint16_t address)
{
    return handle->memory.mapper.read(handle, address);
}

void dmg_mapper_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    handle->memory.mapper.write(handle, address, value);
}
