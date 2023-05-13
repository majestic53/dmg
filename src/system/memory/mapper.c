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

const dmg_attribute_t *dmg_mapper_attribute(dmg_t const dmg)
{
    return dmg->memory.mapper.attribute;
}

void dmg_mapper_clock(dmg_t const dmg)
{
    if (dmg->memory.mapper.clock)
    {
        dmg->memory.mapper.clock(dmg);
    }
}

dmg_error_e dmg_mapper_initialize(dmg_t const dmg, uint8_t id)
{
    dmg_error_e result = DMG_SUCCESS;
    switch (dmg_mapper_type(id, &dmg->memory.mapper.attribute))
    {
        case DMG_MAPPER_MBC0:
            dmg->memory.mapper.read = dmg_mbc0_read;
            dmg->memory.mapper.write = dmg_mbc0_write;
            break;
        case DMG_MAPPER_MBC1:
            dmg_mbc1_initialize(dmg);
            dmg->memory.mapper.read = dmg_mbc1_read;
            dmg->memory.mapper.write = dmg_mbc1_write;
            break;
        case DMG_MAPPER_MBC2:
            dmg_mbc2_initialize(dmg);
            dmg->memory.mapper.read = dmg_mbc2_read;
            dmg->memory.mapper.write = dmg_mbc2_write;
            break;
        case DMG_MAPPER_MBC3:
            dmg_mbc3_initialize(dmg, dmg->memory.mapper.attribute->rtc);
            if (dmg->memory.mapper.attribute->rtc)
            {
                dmg->memory.mapper.clock = dmg_mbc3_clock;
                dmg->memory.mapper.load = dmg_mbc3_load;
                dmg->memory.mapper.save = dmg_mbc3_save;
            }
            dmg->memory.mapper.read = dmg_mbc3_read;
            dmg->memory.mapper.write = dmg_mbc3_write;
            break;
        case DMG_MAPPER_MBC5:
            dmg_mbc5_initialize(dmg);
            dmg->memory.mapper.read = dmg_mbc5_read;
            dmg->memory.mapper.write = dmg_mbc5_write;
            break;
        default:
            result = DMG_ERROR(dmg, "Unsupported mapper type -- %u", id);
            break;
    }
    return result;
}

void dmg_mapper_load(dmg_t const dmg, const void *const data, uint32_t length)
{
    if (dmg->memory.mapper.load)
    {
        dmg->memory.mapper.load(dmg, data, length);
    }
}

uint8_t dmg_mapper_read(dmg_t const dmg, uint16_t address)
{
    return dmg->memory.mapper.read(dmg, address);
}

void dmg_mapper_save(dmg_t const dmg, void *const data, uint32_t length)
{
    if (dmg->memory.mapper.save)
    {
        dmg->memory.mapper.save(dmg, data, length);
    }
}

void dmg_mapper_write(dmg_t const dmg, uint16_t address, uint8_t value)
{
    dmg->memory.mapper.write(dmg, address, value);
}
