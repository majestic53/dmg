/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

static void dmg_mbc5_update(dmg_t const dmg)
{
    dmg->memory.mapper.mbc5.ram.bank = (dmg->memory.mapper.mbc5.bank.ram & 15) & (dmg->memory.cartridge.ram.count - 1);
    dmg->memory.mapper.mbc5.rom.bank[0] = 0;
    dmg->memory.mapper.mbc5.rom.bank[1] = ((dmg->memory.mapper.mbc5.bank.rom.high & 1) << 8) | dmg->memory.mapper.mbc5.bank.rom.low;
    dmg->memory.mapper.mbc5.rom.bank[1] &= dmg->memory.cartridge.rom.count - 1;
}

void dmg_mbc5_initialize(dmg_t const dmg)
{
    dmg->memory.mapper.mbc5.bank.rom.low = 1;
    dmg_mbc5_update(dmg);
}

uint8_t dmg_mbc5_read(dmg_t const dmg, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x0000 ... 0x3FFF: /* ROM 0 */
            result = dmg_cartridge_read_rom(dmg, dmg->memory.mapper.mbc5.rom.bank[0], address);
            break;
        case 0x4000 ... 0x7FFF: /* ROM 1-511 */
            result = dmg_cartridge_read_rom(dmg, dmg->memory.mapper.mbc5.rom.bank[1], address - 0x4000);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0-15 */
            if (dmg->memory.mapper.mbc5.ram.enabled)
            {
                result = dmg_cartridge_read_ram(dmg, dmg->memory.mapper.mbc5.ram.bank, address - 0xA000);
            }
            break;
        default:
            break;
    }
    return result;
}

void dmg_mbc5_write(dmg_t const dmg, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0x0000 ... 0x1FFF: /* RAM ENABLE */
            dmg->memory.mapper.mbc5.ram.enabled = ((value & 0x0F) == 0x0A);
            break;
        case 0x2000 ... 0x2FFF: /* LOW ROM BANK */
            dmg->memory.mapper.mbc5.bank.rom.low = value;
            dmg_mbc5_update(dmg);
            break;
        case 0x3000 ... 0x3FFF: /* HIGH ROM BANK */
            dmg->memory.mapper.mbc5.bank.rom.high = value;
            dmg_mbc5_update(dmg);
            break;
        case 0x4000 ... 0x5FFF: /* RAM BANK */
            dmg->memory.mapper.mbc5.bank.ram = value;
            dmg_mbc5_update(dmg);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0-15 */
            if (dmg->memory.mapper.mbc5.ram.enabled)
            {
                dmg_cartridge_write_ram(dmg, dmg->memory.mapper.mbc5.ram.bank, address - 0xA000, value);
            }
            break;
        default:
            break;
    }
}
