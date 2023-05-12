/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

static void dmg_mbc3_update(dmg_handle_t const handle)
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

void dmg_mbc3_initialize(dmg_handle_t const handle)
{
    dmg_mbc3_update(handle);
}

uint8_t dmg_mbc3_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x0000 ... 0x3FFF: /* ROM 0 */
            result = dmg_cartridge_read_rom(handle, handle->memory.mapper.mbc3.rom.bank[0], address);
            break;
        case 0x4000 ... 0x7FFF: /* ROM 1-127 */
            result = dmg_cartridge_read_rom(handle, handle->memory.mapper.mbc3.rom.bank[1], address - 0x4000);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0-3 */
            if (handle->memory.mapper.mbc3.ram.enabled)
            {
                result = dmg_cartridge_read_ram(handle, handle->memory.mapper.mbc3.ram.bank, address - 0xA000);
            }
            break;
        default:
            break;
    }
    return result;
}

void dmg_mbc3_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0x0000 ... 0x1FFF: /* RAM ENABLE */
            handle->memory.mapper.mbc3.ram.enabled = ((value & 0x0F) == 0x0A);
            break;
        case 0x2000 ... 0x3FFF: /* ROM BANK */
            handle->memory.mapper.mbc3.bank.rom = value;
            dmg_mbc3_update(handle);
            break;
        case 0x4000 ... 0x5FFF: /* RAM BANK */
            handle->memory.mapper.mbc3.bank.ram = value & 3;
            dmg_mbc3_update(handle);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0-3 */
            if (handle->memory.mapper.mbc3.ram.enabled)
            {
                dmg_cartridge_write_ram(handle, handle->memory.mapper.mbc3.ram.bank, address - 0xA000, value);
            }
            break;
        default:
            break;
    }
}
