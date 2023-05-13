/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

dmg_error_e dmg_memory_initialize(dmg_t const dmg, const dmg_data_t *const data)
{
    dmg_error_e result;
    if (!data)
    {
        return DMG_ERROR(dmg, "Invalid data -- %p", data);
    }
    if ((result = dmg_cartridge_initialize(dmg, data)) != DMG_SUCCESS)
    {
        return result;
    }
    dmg_bootloader_initialize(dmg);
    return result;
}

uint8_t dmg_memory_read(dmg_t const dmg, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x0000 ... 0x00FF: /* BOOTLOADER/MAPPER */
            if (dmg_bootloader_enabled(dmg))
            {
                result = dmg_bootloader_read(dmg, address);
            }
            else
            {
                result = dmg_mapper_read(dmg, address);
            }
            break;
        case 0xC000 ... 0xDFFF: /* WORK RAM */
            result = dmg->memory.ram.work[address - 0xC000];
            break;
        case 0xE000 ... 0xFDFF: /* WORK RAM (MIRROR) */
            result = dmg->memory.ram.work[address - 0xE000];
            break;
        case 0xFEA0 ... 0xFEFF: /* UNUSED */
            result = 0;
            break;
        case 0xFF80 ... 0xFFFE: /* HIGH RAM */
            result = dmg->memory.ram.high[address - 0xFF80];
            break;
        default: /* MAPPER */
            result = dmg_mapper_read(dmg, address);
            break;
    }
    return result;
}

void dmg_memory_write(dmg_t const dmg, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0xC000 ... 0xDFFF: /* WORK RAM */
            dmg->memory.ram.work[address - 0xC000] = value;
            break;
        case 0xE000 ... 0xFDFF: /* WORK RAM (MIRROR) */
            dmg->memory.ram.work[address - 0xE000] = value;
            break;
        case 0xFEA0 ... 0xFEFF: /* UNUSED */
            break;
        case 0xFF50: /* BOOTLOADER */
            dmg_bootloader_write(dmg, address, value);
            break;
        case 0xFF80 ... 0xFFFE: /* HIGH RAM */
            dmg->memory.ram.high[address - 0xFF80] = value;
            break;
        default: /* MAPPER */
            dmg_mapper_write(dmg, address, value);
            break;
    }
}
