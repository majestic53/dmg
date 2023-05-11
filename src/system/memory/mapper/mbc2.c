/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

uint8_t dmg_mbc2_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x0000 ... 0x3FFF: /* ROM 0 */
            result = dmg_cartridge_read_rom(handle, handle->memory.mapper.mbc2.rom.bank[0], address);
            break;
        case 0x4000 ... 0x7FFF: /* ROM 1-15 */
            result = dmg_cartridge_read_rom(handle, handle->memory.mapper.mbc2.rom.bank[1], address - 0x4000);
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

void dmg_mbc2_update(dmg_handle_t const handle)
{
    handle->memory.mapper.mbc2.rom.bank[0] = 0;
    handle->memory.mapper.mbc2.rom.bank[1] = handle->memory.mapper.mbc2.bank.rom & 15;
    if (!handle->memory.mapper.mbc2.rom.bank[1])
    { /* BANK 0->1 */
        ++handle->memory.mapper.mbc2.rom.bank[1];
    }
    handle->memory.mapper.mbc2.rom.bank[1] &= handle->memory.cartridge.rom.count - 1;
}

void dmg_mbc2_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0x0000 ... 0x3FFF: /* RAM ENABLE/ROM BANK */
            if (address & 0x100)
            { /* ROM BANK */
                handle->memory.mapper.mbc2.bank.rom = value;
                dmg_mbc2_update(handle);
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