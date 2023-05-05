/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <common.h>

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

static const uint8_t BOOTROM[] =
{
    0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
    0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,
    0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
    0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
    0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
    0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
    0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
    0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
    0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xE2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
    0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
    0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
    0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
    0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
    0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3C, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x3C,
    0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20,
    0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50,
};

static const uint16_t RAM[] =
{
    0, 1, 1, 4, 16, 8,
};

static const uint16_t ROM[] =
{
    2, 4, 8, 16, 32, 64, 128, 256, 512,
};

static const dmg_memory_type_t TYPE[] =
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

static uint8_t dmg_memory_checksum(const uint8_t *const data, uint16_t begin, uint16_t end)
{
    uint8_t result = 0;
    for (uint16_t index = begin; index <= end; ++index)
    {
        result = result - data[index] - 1;
    }
    return result;
}

static const dmg_memory_header_t *dmg_memory_header(const uint8_t *const data)
{
    return (const dmg_memory_header_t *)&data[0x0100];
}

static dmg_mapper_e dmg_memory_type(uint8_t id)
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

static uint8_t dmg_memory_ram_read(dmg_handle_t const handle, uint16_t bank, uint16_t address)
{
    uint8_t result = 0xFF;
    if (handle->memory.cartridge.ram.data)
    {
        result = handle->memory.cartridge.ram.data[(bank * 0x2000) + address];
    }
    return result;
}

static void dmg_memory_ram_write(dmg_handle_t const handle, uint16_t bank, uint16_t address, uint8_t value)
{
    if (handle->memory.cartridge.ram.data)
    {
        handle->memory.cartridge.ram.data[(bank * 0x2000) + address] = value;
    }
}

static uint8_t dmg_memory_rom_read(dmg_handle_t const handle, uint16_t bank, uint16_t address)
{
    return handle->memory.cartridge.rom.data[(bank * 0x4000) + address];
}

static uint8_t dmg_memory_mbc0_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x0000 ... 0x3FFF: /* ROM 0 */
            result = dmg_memory_rom_read(handle, 0, address);
            break;
        case 0x4000 ... 0x7FFF: /* ROM 1 */
            result = dmg_memory_rom_read(handle, 1, address - 0x4000);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0 */
            result = dmg_memory_ram_read(handle, 0, address - 0xA000);
            break;
        default:
            break;
    }
    return result;
}

static void dmg_memory_mbc0_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address) {
        case 0xA000 ... 0xBFFF: /* RAM 0 */
            dmg_memory_ram_write(handle, 0, address - 0xA000, value);
            break;
        default:
            break;
    }
}

static uint8_t dmg_memory_mbc1_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x0000 ... 0x3FFF: /* ROM 0,32,64,96 */
            result = dmg_memory_rom_read(handle, handle->memory.mapper.mbc1.rom.bank[0], address);
            break;
        case 0x4000 ... 0x7FFF: /* ROM 1-31,33-63,65-95,97-127 */
            result = dmg_memory_rom_read(handle, handle->memory.mapper.mbc1.rom.bank[1], address - 0x4000);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0-3 */
            if (handle->memory.mapper.mbc1.ram.enabled) {
                result = dmg_memory_ram_read(handle, handle->memory.mapper.mbc1.ram.bank, address - 0xA000);
            }
            break;
        default:
            break;
    }
    return result;
}

static void dmg_memory_mbc1_update(dmg_handle_t const handle)
{
    if (handle->memory.cartridge.rom.count >= 64)
    { /* >=1MB */
        handle->memory.mapper.mbc1.ram.bank = 0;
        if (handle->memory.mapper.mbc1.bank.select & 1)
        {
            handle->memory.mapper.mbc1.rom.bank[0] = (handle->memory.mapper.mbc1.bank.high & 3) << 5;
        }
        else
        {
            handle->memory.mapper.mbc1.rom.bank[0] = 0;
        }
        handle->memory.mapper.mbc1.rom.bank[1] = ((handle->memory.mapper.mbc1.bank.high & 3) << 5) | (handle->memory.mapper.mbc1.bank.low & 31);
    }
    else
    { /* <=512KB */
        handle->memory.mapper.mbc1.ram.bank = (handle->memory.mapper.mbc1.bank.select & 1) ? handle->memory.mapper.mbc1.bank.high & 3 : 0;
        handle->memory.mapper.mbc1.rom.bank[0] = 0;
        handle->memory.mapper.mbc1.rom.bank[1] = handle->memory.mapper.mbc1.bank.low & 31;
    }
    switch (handle->memory.mapper.mbc1.rom.bank[1])
    {
        case 0: /* BANK 0->1 */
        case 32: /* BANK 32->33 */
        case 64: /* BANK 64->65 */
        case 96: /* BANK 96->97 */
            ++handle->memory.mapper.mbc1.rom.bank[1];
            break;
        default:
            break;
    }
    handle->memory.mapper.mbc1.ram.bank &= handle->memory.cartridge.ram.count - 1;
    handle->memory.mapper.mbc1.rom.bank[0] &= handle->memory.cartridge.rom.count - 1;
    handle->memory.mapper.mbc1.rom.bank[1] &= handle->memory.cartridge.rom.count - 1;
}

static void dmg_memory_mbc1_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0x0000 ... 0x1FFF: /* RAM ENABLE */
            handle->memory.mapper.mbc1.ram.enabled = ((value & 0x0F) == 0x0A);
            break;
        case 0x2000 ... 0x3FFF: /* BANK LOW */
            handle->memory.mapper.mbc1.bank.low = value;
            dmg_memory_mbc1_update(handle);
            break;
        case 0x4000 ... 0x5FFF: /* BANK HIGH */
            handle->memory.mapper.mbc1.bank.high = value;
            dmg_memory_mbc1_update(handle);
            break;
        case 0x6000 ... 0x7FFF: /* BANK SELECT */
            handle->memory.mapper.mbc1.bank.select = value;
            dmg_memory_mbc1_update(handle);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0-3 */
            if (handle->memory.mapper.mbc1.ram.enabled)
            {
                dmg_memory_ram_write(handle, handle->memory.mapper.mbc1.ram.bank, address - 0xA000, value);
            }
            break;
        default:
            break;
    }
}

static uint8_t dmg_memory_mbc2_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x0000 ... 0x3FFF: /* ROM 0 */
            result = dmg_memory_rom_read(handle, handle->memory.mapper.mbc2.rom.bank[0], address);
            break;
        case 0x4000 ... 0x7FFF: /* ROM 1-15 */
            result = dmg_memory_rom_read(handle, handle->memory.mapper.mbc2.rom.bank[1], address - 0x4000);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0 */
            if (handle->memory.mapper.mbc2.ram.enabled)
            {
                result = 0xF0 | handle->memory.mapper.mbc2.ram.data[(address - 0xA000) % 512];
            }
            break;
        default:
            break;
    }
    return result;
}

static void dmg_memory_mbc2_update(dmg_handle_t const handle)
{
    handle->memory.mapper.mbc2.rom.bank[0] = 0;
    handle->memory.mapper.mbc2.rom.bank[1] = handle->memory.mapper.mbc2.bank.rom & 15;
    if (!handle->memory.mapper.mbc2.rom.bank[1])
    { /* BANK 0->1 */
        ++handle->memory.mapper.mbc2.rom.bank[1];
    }
    handle->memory.mapper.mbc2.rom.bank[1] &= handle->memory.cartridge.rom.count - 1;
}

static void dmg_memory_mbc2_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address) {
        case 0x0000 ... 0x3FFF: /* RAM ENABLE/ROM BANK */
            if (address & 0x100)
            { /* ROM BANK */
                handle->memory.mapper.mbc2.bank.rom = value;
                dmg_memory_mbc2_update(handle);
            }
            else
            { /* RAM ENABLE */
                handle->memory.mapper.mbc2.ram.enabled = ((value & 0x0F) == 0x0A);
            }
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0 */
            if (handle->memory.mapper.mbc2.ram.enabled)
            {
                handle->memory.mapper.mbc2.ram.data[(address - 0xA000) % 512] = 0xF0 | value;
            }
        default:
            break;
    }
}

static uint8_t dmg_memory_mbc3_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address) {
        case 0x0000 ... 0x3FFF: /* ROM 0 */
            result = dmg_memory_rom_read(handle, handle->memory.mapper.mbc3.rom.bank[0], address);
            break;
        case 0x4000 ... 0x7FFF: /* ROM 1-127 */
            result = dmg_memory_rom_read(handle, handle->memory.mapper.mbc3.rom.bank[1], address - 0x4000);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0-3 */
            if (handle->memory.mapper.mbc3.ram.enabled)
            {
                result = dmg_memory_ram_read(handle, handle->memory.mapper.mbc3.ram.bank, address - 0xA000);
            }
            break;
        default:
            break;
    }
    return result;
}

static void dmg_memory_mbc3_update(dmg_handle_t const handle)
{
    handle->memory.mapper.mbc3.ram.bank = (handle->memory.mapper.mbc3.bank.ram & 3) & (handle->memory.cartridge.ram.count - 1);
    handle->memory.mapper.mbc3.rom.bank[0] = 0;
    handle->memory.mapper.mbc3.rom.bank[1] = handle->memory.mapper.mbc3.bank.rom & 127;
    if (!handle->memory.mapper.mbc3.rom.bank[1])
    { /* BANK 0->1 */
        ++handle->memory.mapper.mbc3.rom.bank[1];
    }
    handle->memory.mapper.mbc3.rom.bank[1] &= handle->memory.cartridge.rom.count - 1;
}

static void dmg_memory_mbc3_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address) {
        case 0x0000 ... 0x1FFF: /* RAM ENABLE */
            handle->memory.mapper.mbc3.ram.enabled = ((value & 0x0F) == 0x0A);
            break;
        case 0x2000 ... 0x3FFF: /* ROM BANK */
            handle->memory.mapper.mbc3.bank.rom = value;
            dmg_memory_mbc3_update(handle);
            break;
        case 0x4000 ... 0x5FFF: /* RAM BANK */
            handle->memory.mapper.mbc3.bank.ram = value & 3;
            dmg_memory_mbc3_update(handle);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0-3 */
            if (handle->memory.mapper.mbc3.ram.enabled)
            {
                dmg_memory_ram_write(handle, handle->memory.mapper.mbc3.ram.bank, address - 0xA000, value);
            }
            break;
        default:
            break;
    }
}

static uint8_t dmg_memory_mbc5_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address) {
        case 0x0000 ... 0x3FFF: /* ROM 0 */
            result = dmg_memory_rom_read(handle, handle->memory.mapper.mbc5.rom.bank[0], address);
            break;
        case 0x4000 ... 0x7FFF: /* ROM 1-511 */
            result = dmg_memory_rom_read(handle, handle->memory.mapper.mbc5.rom.bank[1], address - 0x4000);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0-15 */
            if (handle->memory.mapper.mbc5.ram.enabled)
            {
                result = dmg_memory_ram_read(handle, handle->memory.mapper.mbc5.ram.bank, address - 0xA000);
            }
            break;
        default:
            break;
    }
    return result;
}

static void dmg_memory_mbc5_update(dmg_handle_t const handle)
{
    handle->memory.mapper.mbc5.ram.bank = (handle->memory.mapper.mbc5.bank.ram & 15) & (handle->memory.cartridge.ram.count - 1);
    handle->memory.mapper.mbc5.rom.bank[0] = 0;
    handle->memory.mapper.mbc5.rom.bank[1] = ((handle->memory.mapper.mbc5.bank.rom.high & 1) << 8) | handle->memory.mapper.mbc5.bank.rom.low;
    handle->memory.mapper.mbc5.rom.bank[1] &= handle->memory.cartridge.rom.count - 1;
}

static void dmg_memory_mbc5_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0x0000 ... 0x1FFF: /* RAM ENABLE */
            handle->memory.mapper.mbc5.ram.enabled = ((value & 0x0F) == 0x0A);
            break;
        case 0x2000 ... 0x2FFF: /* LOW ROM BANK */
            handle->memory.mapper.mbc5.bank.rom.low = value;
            dmg_memory_mbc5_update(handle);
            break;
        case 0x3000 ... 0x3FFF: /* HIGH ROM BANK */
            handle->memory.mapper.mbc5.bank.rom.high = value;
            dmg_memory_mbc5_update(handle);
            break;
        case 0x4000 ... 0x5FFF: /* RAM BANK */
            handle->memory.mapper.mbc5.bank.ram = value;
            dmg_memory_mbc5_update(handle);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0-15 */
            if (handle->memory.mapper.mbc5.ram.enabled)
            {
                dmg_memory_ram_write(handle, handle->memory.mapper.mbc5.ram.bank, address - 0xA000, value);
            }
            break;
        default:
            break;
    }
}

static void dmg_memory_setup_bootloader(dmg_handle_t const handle)
{
    handle->memory.bootloader.enabled = true;
}

static void dmg_memory_setup_mapper(dmg_handle_t const handle, uint8_t id)
{
    switch (dmg_memory_type(id))
    {
        case DMG_MAPPER_MBC0:
            handle->memory.mapper.read = dmg_memory_mbc0_read;
            handle->memory.mapper.write = dmg_memory_mbc0_write;
            break;
        case DMG_MAPPER_MBC1:
            dmg_memory_mbc1_update(handle);
            handle->memory.mapper.read = dmg_memory_mbc1_read;
            handle->memory.mapper.write = dmg_memory_mbc1_write;
            break;
        case DMG_MAPPER_MBC2:
            dmg_memory_mbc2_update(handle);
            handle->memory.mapper.read = dmg_memory_mbc2_read;
            handle->memory.mapper.write = dmg_memory_mbc2_write;
            break;
        case DMG_MAPPER_MBC3:
            dmg_memory_mbc3_update(handle);
            handle->memory.mapper.read = dmg_memory_mbc3_read;
            handle->memory.mapper.write = dmg_memory_mbc3_write;
            break;
        case DMG_MAPPER_MBC5:
            handle->memory.mapper.mbc5.bank.rom.low = 1;
            dmg_memory_mbc5_update(handle);
            handle->memory.mapper.read = dmg_memory_mbc5_read;
            handle->memory.mapper.write = dmg_memory_mbc5_write;
            break;
        default:
            break;
    }
}

static dmg_error_e dmg_memory_setup_ram(dmg_handle_t const handle, uint16_t count)
{
    if (count)
    {
        if (!(handle->memory.cartridge.ram.data = calloc(count, 0x2000)))
        {
            return DMG_ERROR(handle, "Failed to allocate cartridge ram -- %u banks", count);
        }
    }
    handle->memory.cartridge.ram.count = count;
    return DMG_SUCCESS;
}

static void dmg_memory_setup_rom(dmg_handle_t const handle, const uint8_t *const data, uint16_t count)
{
    handle->memory.cartridge.rom.data = data;
    handle->memory.cartridge.rom.count = count;
}

static void dmg_memory_setup_title(dmg_handle_t const handle, const char *title)
{
    if (strlen(title))
    {
        for (uint32_t index = 0; index < sizeof (handle->memory.cartridge.title) - 1; ++index)
        {
            if (isprint(title[index]))
            {
                handle->memory.cartridge.title[index] = !isspace(title[index]) ? title[index] : ' ';
            }
        }
    }
    else
    {
        snprintf(handle->memory.cartridge.title, sizeof (handle->memory.cartridge.title), "%s", "UNTITLED");
    }
}

static dmg_error_e dmg_memory_setup(dmg_handle_t const handle, const uint8_t *const data)
{
    dmg_error_e result;
    const dmg_memory_header_t *header = dmg_memory_header(data);
    if ((result = dmg_memory_setup_ram(handle, RAM[header->ram])) == DMG_SUCCESS)
    {
        dmg_memory_setup_bootloader(handle);
        dmg_memory_setup_mapper(handle, header->id);
        dmg_memory_setup_rom(handle, data, ROM[header->rom]);
        dmg_memory_setup_title(handle, (const char *)header->title);
    }
    return result;
}

static dmg_error_e dmg_memory_validate(dmg_handle_t const handle, const uint8_t *const data, uint32_t length)
{
    uint8_t checksum;
    uint32_t expected = 0x4000;
    const dmg_memory_header_t *header;
    if (!data)
    {
        return DMG_ERROR(handle, "Invalid cartridge data -- %p", data);
    }
    if ((length < expected) || (length > (ROM[(sizeof (ROM) / sizeof (*ROM)) - 1] * 0x4000)))
    {
        return DMG_ERROR(handle, "Invalid cartridge length -- %u bytes", length);
    }
    header = dmg_memory_header(data);
    if ((checksum = dmg_memory_checksum(data, 0x0134, 0x014C)) != header->checksum)
    {
        return DMG_ERROR(handle, "Invalid cartridge checksum -- %u (expecting %u)", checksum, header->checksum);
    }
    if (dmg_memory_type(header->id) >= DMG_MAPPER_MAX)
    {
        return DMG_ERROR(handle, "Unsupported cartridge type -- %u", header->id);
    }
    if (header->cgb == 0xC0)
    {
        return DMG_ERROR(handle, "Unsupported CGB cartridge -- %u", header->cgb);
    }
    if (header->ram >= sizeof (RAM) / sizeof (*RAM))
    {
        return DMG_ERROR(handle, "Unsupported cartridge RAM type -- %u", header->ram);
    }
    if (header->rom >= sizeof (ROM) / sizeof (*ROM))
    {
        return DMG_ERROR(handle, "Unsupported cartridge ROM type -- %u", header->rom);
    }
    if (length != (expected *= ROM[header->rom]))
    {
        return DMG_ERROR(handle, "Invalid cartridge length -- %u bytes (expecting %u bytes)", length, expected);
    }
    return DMG_SUCCESS;
}

const char *dmg_memory_get_title(dmg_handle_t const handle)
{
    return handle->memory.cartridge.title;
}

dmg_error_e dmg_memory_initialize(dmg_handle_t const handle, const dmg_data_t *const data)
{
    dmg_error_e result;
    if (!data)
    {
        return DMG_ERROR(handle, "Invalid data -- %p", data);
    }
    if ((result = dmg_memory_validate(handle, data->buffer, data->length)) == DMG_SUCCESS)
    {
        result = dmg_memory_setup(handle, data->buffer);
    }
    return result;
}

dmg_error_e dmg_memory_load(dmg_handle_t const handle, const dmg_data_t *const data)
{
    uint32_t expected = handle->memory.cartridge.ram.count * 0x2000;
    if (!data)
    {
        return DMG_ERROR(handle, "Invalid data -- %p", data);
    }
    if (!data->buffer)
    {
        return DMG_ERROR(handle, "Invalid load data -- %p", data);
    }
    if (data->length != expected)
    {
        return DMG_ERROR(handle, "Invalid load length -- %u bytes (expecting %u bytes)", data->length, expected);
    }
    memcpy(handle->memory.cartridge.ram.data, data->buffer, data->length);
    return DMG_SUCCESS;
}

uint8_t dmg_memory_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x0000 ... 0x00FF: /* BOOTROM/MAPPER */
            if (handle->memory.bootloader.enabled)
            {
                result = BOOTROM[address];
            }
            else
            {
                result = handle->memory.mapper.read(handle, address);
            }
            break;
        case 0xC000 ... 0xDFFF: /* WORK RAM */
            result = handle->memory.ram.work[address - 0xC000];
            break;
        case 0xE000 ... 0xFDFF: /* WORK RAM (MIRROR) */
            result = handle->memory.ram.work[address - 0xE000];
            break;
        case 0xFEA0 ... 0xFEFF: /* UNUSED */
            result = 0;
            break;
        case 0xFF80 ... 0xFFFE: /* HIGH RAM */
            result = handle->memory.ram.high[address - 0xFF80];
            break;
        default: /* MAPPER */
            result = handle->memory.mapper.read(handle, address);
            break;
    }
    return result;
}

dmg_error_e dmg_memory_save(dmg_handle_t const handle, dmg_data_t *const data)
{
    if (!data)
    {
        return DMG_ERROR(handle, "Invalid data -- %p", data);
    }
    data->buffer = handle->memory.cartridge.ram.data;
    data->length = handle->memory.cartridge.ram.count * 0x2000;
    return DMG_SUCCESS;
}

void dmg_memory_uninitialize(dmg_handle_t const handle)
{
    if (handle->memory.cartridge.ram.data)
    {
        free(handle->memory.cartridge.ram.data);
    }
}

void dmg_memory_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0xC000 ... 0xDFFF: /* WORK RAM */
            handle->memory.ram.work[address - 0xC000] = value;
            break;
        case 0xE000 ... 0xFDFF: /* WORK RAM (MIRROR) */
            handle->memory.ram.work[address - 0xE000] = value;
            break;
        case 0xFEA0 ... 0xFEFF: /* UNUSED */
            break;
        case 0xFF50: /* BOOTROM DISABLE */
            if (value)
            {
                handle->memory.bootloader.enabled = false;
            }
            break;
        case 0xFF80 ... 0xFFFE: /* HIGH RAM */
            handle->memory.ram.high[address - 0xFF80] = value;
            break;
        default: /* MAPPER */
            handle->memory.mapper.write(handle, address, value);
            break;
    }
}
