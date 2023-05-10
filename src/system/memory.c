/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

dmg_error_e dmg_memory_initialize(dmg_handle_t const handle, const dmg_data_t *const data)
{
    dmg_error_e result;
    if (!data)
    {
        return DMG_ERROR(handle, "Invalid data -- %p", data);
    }
    if ((result = dmg_cartridge_initialize(handle, data)) != DMG_SUCCESS)
    {
        return result;
    }
    dmg_bootloader_initialize(handle);
    return result;
}

uint8_t dmg_memory_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x0000 ... 0x00FF: /* BOOTLOADER/MAPPER */
            if (dmg_bootloader_enabled(handle))
            {
                result = dmg_bootloader_read(handle, address);
            }
            else
            {
                result = dmg_mapper_read(handle, address);
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
            result = dmg_mapper_read(handle, address);
            break;
    }
    return result;
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
        case 0xFF50: /* BOOTLOADER */
            dmg_bootloader_write(handle, address, value);
            break;
        case 0xFF80 ... 0xFFFE: /* HIGH RAM */
            handle->memory.ram.high[address - 0xFF80] = value;
            break;
        default: /* MAPPER */
            dmg_mapper_write(handle, address, value);
            break;
    }
}
