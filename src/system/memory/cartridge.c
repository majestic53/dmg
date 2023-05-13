/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

typedef struct
{
    char magic[4];
    uint32_t length;
    union
    {
        struct
        {
            uint32_t version : 8;
            uint32_t checksum : 8;
            uint32_t rtc : 1;
            uint32_t : 15;
        };
        uint32_t raw;
    } attribute;
} dmg_ram_header_t;

typedef struct
{
    dmg_ram_header_t header;
    dmg_rtc_t rtc;
} dmg_ram_t;

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
} dmg_rom_header_t;

static const uint16_t RAM[] =
{
    0, 1, 1, 4, 16, 8,
};

static const uint16_t ROM[] =
{
    2, 4, 8, 16, 32, 64, 128, 256, 512,
};

static uint8_t dmg_cartridge_checksum(const void *const data, uint16_t begin, uint16_t end)
{
    uint8_t result = 0;
    for (uint16_t index = begin; index <= end; ++index)
    {
        result = result - ((const uint8_t *const)data)[index] - 1;
    }
    return result;
}

static dmg_error_e dmg_cartridge_initialize_ram(dmg_t const dmg, uint16_t count)
{
    if (!(dmg->memory.cartridge.ram.data = calloc(1, (count * 0x2000) + sizeof (dmg_ram_t))))
    {
        return DMG_ERROR(dmg, "Failed to allocate RAM -- %u banks", count);
    }
    dmg->memory.cartridge.ram.count = count;
    return DMG_SUCCESS;
}

static void dmg_cartridge_initialize_rom(dmg_t const dmg, const uint8_t *const data, uint16_t count)
{
    dmg->memory.cartridge.rom.data = data;
    dmg->memory.cartridge.rom.count = count;
}

static void dmg_cartridge_initialize_title(dmg_t const dmg, const char *title)
{
    if (strlen(title))
    {
        for (uint32_t index = 0; index < sizeof (dmg->memory.cartridge.title) - 1; ++index)
        {
            if (isprint(title[index]))
            {
                dmg->memory.cartridge.title[index] = !isspace(title[index]) ? title[index] : ' ';
            }
        }
    }
    else
    {
        snprintf(dmg->memory.cartridge.title, sizeof (dmg->memory.cartridge.title), "%s", "UNTITLED");
    }
}

static void dmg_cartridge_load_ram(dmg_t const dmg, const dmg_data_t *const data)
{
    memcpy(dmg->memory.cartridge.ram.data, data->buffer, data->length);
}

static void dmg_cartridge_load_rtc(dmg_t const dmg, const dmg_data_t *const data)
{
    const dmg_ram_header_t *header = (const dmg_ram_header_t *)data->buffer;
    if (header->attribute.rtc)
    {
        dmg_ram_t *const ram = (dmg_ram_t *const)data->buffer;
        dmg_mapper_load(dmg, &ram->rtc, sizeof (ram->rtc));
    }
}

static void dmg_cartridge_save_ram(dmg_t const dmg, dmg_data_t *const data)
{
    dmg_ram_t *const ram = (dmg_ram_t *const)data->buffer;
    memcpy(ram->header.magic, "dmg", strlen("dmg"));
    ram->header.length = dmg->memory.cartridge.ram.count * 0x2000;
    ram->header.attribute.checksum = dmg_cartridge_checksum(ram, sizeof (dmg_ram_header_t), ram->header.length);
    ram->header.attribute.version = 1;
    data->length = ram->header.length + sizeof (dmg_ram_t);
}

static void dmg_cartridge_save_rtc(dmg_t const dmg, dmg_data_t *const data)
{
    dmg_ram_t *const ram = (dmg_ram_t *const)data->buffer;
    ram->header.attribute.rtc = dmg_mapper_attribute(dmg)->rtc;
    if (ram->header.attribute.rtc)
    {
        dmg_mapper_save(dmg, &ram->rtc, sizeof (ram->rtc));
    }
}

static dmg_error_e dmg_cartridge_validate_ram(dmg_t const dmg, const uint8_t *const data, uint32_t length)
{
    uint8_t checksum;
    uint32_t expected = (dmg->memory.cartridge.ram.count * 0x2000) + sizeof (dmg_ram_t);
    const dmg_ram_header_t *header;
    if (!data)
    {
        return DMG_ERROR(dmg, "Invalid RAM data -- %p", data);
    }
    if (length != expected)
    {
        return DMG_ERROR(dmg, "Invalid RAM length -- %u bytes (expecting %u bytes)", length, expected);
    }
    header = (const dmg_ram_header_t *)data;
    if (strncmp(header->magic, "dmg", strlen("dmg")))
    {
        return DMG_ERROR(dmg, "Invalid RAM magic");
    }
    expected -= sizeof (dmg_ram_t);
    if (header->length != expected)
    {
        return DMG_ERROR(dmg, "Invalid RAM length -- %u bytes (expecting %u bytes)", header->length, expected);
    }
    if (header->attribute.version != 1)
    {
        return DMG_ERROR(dmg, "Unsupported RAM version -- %u", header->attribute.version);
    }
    if ((checksum = dmg_cartridge_checksum(header, sizeof (dmg_ram_header_t), header->length)) != header->attribute.checksum)
    {
        return DMG_ERROR(dmg, "Invalid RAM checksum -- %u (expecting %u)", checksum, header->attribute.checksum);
    }
    return DMG_SUCCESS;
}

static dmg_error_e dmg_cartridge_validate_rom(dmg_t const dmg, const uint8_t *const data, uint32_t length)
{
    uint8_t checksum;
    uint32_t expected = 0x4000;
    const dmg_rom_header_t *header;
    if (!data)
    {
        return DMG_ERROR(dmg, "Invalid ROM data -- %p", data);
    }
    if ((length < expected) || (length > (ROM[(sizeof (ROM) / sizeof (*ROM)) - 1] * 0x4000)))
    {
        return DMG_ERROR(dmg, "Invalid ROM length -- %u bytes", length);
    }
    header = (const dmg_rom_header_t *)&data[0x0100];
    if ((checksum = dmg_cartridge_checksum(data, 0x0134, 0x014C)) != header->checksum)
    {
        return DMG_ERROR(dmg, "Invalid ROM checksum -- %u (expecting %u)", checksum, header->checksum);
    }
    if (header->cgb == 0xC0)
    {
        return DMG_ERROR(dmg, "Unsupported ROM type -- CGB");
    }
    if (header->ram >= sizeof (RAM) / sizeof (*RAM))
    {
        return DMG_ERROR(dmg, "Unsupported RAM type -- %u", header->ram);
    }
    if (header->rom >= sizeof (ROM) / sizeof (*ROM))
    {
        return DMG_ERROR(dmg, "Unsupported ROM type -- %u", header->rom);
    }
    if (length != (expected *= ROM[header->rom]))
    {
        return DMG_ERROR(dmg, "Invalid ROM length -- %u bytes (expecting %u bytes)", length, expected);
    }
    return DMG_SUCCESS;
}

dmg_error_e dmg_cartridge_initialize(dmg_t const dmg, const dmg_data_t *const data)
{
    dmg_error_e result;
    const dmg_rom_header_t *header;
    if (!data)
    {
        return DMG_ERROR(dmg, "Invalid data -- %p", data);
    }
    if ((result = dmg_cartridge_validate_rom(dmg, data->buffer, data->length)) != DMG_SUCCESS)
    {
        return result;
    }
    header = (const dmg_rom_header_t *)&data->buffer[0x0100];
    if ((result = dmg_mapper_initialize(dmg, header->id)) != DMG_SUCCESS)
    {
        return result;
    }
    if ((result = dmg_cartridge_initialize_ram(dmg, RAM[header->ram])) != DMG_SUCCESS)
    {
        return result;
    }
    dmg_cartridge_initialize_rom(dmg, data->buffer, ROM[header->rom]);
    dmg_cartridge_initialize_title(dmg, (const char *)header->title);
    return result;
}

dmg_error_e dmg_cartridge_load(dmg_t const dmg, const dmg_data_t *const data)
{
    dmg_error_e result;
    if (!data)
    {
        return DMG_ERROR(dmg, "Invalid data -- %p", data);
    }
    if ((result = dmg_cartridge_validate_ram(dmg, data->buffer, data->length)) != DMG_SUCCESS)
    {
        return result;
    }
    dmg_cartridge_load_ram(dmg, data);
    dmg_cartridge_load_rtc(dmg, data);
    return result;
}

uint8_t dmg_cartridge_read_ram(dmg_t const dmg, uint16_t bank, uint16_t address)
{
    uint8_t result = 0xFF;
    if (dmg->memory.cartridge.ram.data)
    {
        result = dmg->memory.cartridge.ram.data[(bank * 0x2000) + address + sizeof (dmg_ram_t)];
    }
    return result;
}

uint8_t dmg_cartridge_read_rom(dmg_t const dmg, uint16_t bank, uint16_t address)
{
    return dmg->memory.cartridge.rom.data[(bank * 0x4000) + address];
}

dmg_error_e dmg_cartridge_save(dmg_t const dmg, dmg_data_t *const data)
{
    if (!data)
    {
        return DMG_ERROR(dmg, "Invalid data -- %p", data);
    }
    if ((data->buffer = dmg->memory.cartridge.ram.data))
    {
        dmg_cartridge_save_rtc(dmg, data);
        dmg_cartridge_save_ram(dmg, data);
    }
    return DMG_SUCCESS;
}

const char *dmg_cartridge_title(dmg_t const dmg)
{
    return dmg->memory.cartridge.title;
}

void dmg_cartridge_uninitialize(dmg_t const dmg)
{
    if (dmg->memory.cartridge.ram.data)
    {
        free(dmg->memory.cartridge.ram.data);
    }
}

void dmg_cartridge_write_ram(dmg_t const dmg, uint16_t bank, uint16_t address, uint8_t value)
{
    if (dmg->memory.cartridge.ram.data)
    {
        dmg->memory.cartridge.ram.data[(bank * 0x2000) + address + sizeof (dmg_ram_t)] = value;
    }
}
