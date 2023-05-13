/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

static void dmg_mbc1_update(dmg_t const dmg)
{
    if (dmg->memory.cartridge.rom.count >= 64)
    { /* >=1MB */
        dmg->memory.mapper.mbc1.ram.bank = 0;
        if (dmg->memory.mapper.mbc1.bank.select & 1)
        {
            dmg->memory.mapper.mbc1.rom.bank[0] = (dmg->memory.mapper.mbc1.bank.high & 3) << 5;
        }
        else
        {
            dmg->memory.mapper.mbc1.rom.bank[0] = 0;
        }
        dmg->memory.mapper.mbc1.rom.bank[1] = ((dmg->memory.mapper.mbc1.bank.high & 3) << 5) | (dmg->memory.mapper.mbc1.bank.low & 31);
    }
    else
    { /* <=512KB */
        dmg->memory.mapper.mbc1.ram.bank = (dmg->memory.mapper.mbc1.bank.select & 1) ? dmg->memory.mapper.mbc1.bank.high & 3 : 0;
        dmg->memory.mapper.mbc1.rom.bank[0] = 0;
        dmg->memory.mapper.mbc1.rom.bank[1] = dmg->memory.mapper.mbc1.bank.low & 31;
    }
    switch (dmg->memory.mapper.mbc1.rom.bank[1])
    {
        case 0: /* BANK 0->1 */
        case 32: /* BANK 32->33 */
        case 64: /* BANK 64->65 */
        case 96: /* BANK 96->97 */
            ++dmg->memory.mapper.mbc1.rom.bank[1];
            break;
        default:
            break;
    }
    dmg->memory.mapper.mbc1.ram.bank &= dmg->memory.cartridge.ram.count - 1;
    dmg->memory.mapper.mbc1.rom.bank[0] &= dmg->memory.cartridge.rom.count - 1;
    dmg->memory.mapper.mbc1.rom.bank[1] &= dmg->memory.cartridge.rom.count - 1;
}

void dmg_mbc1_initialize(dmg_t const dmg)
{
    dmg_mbc1_update(dmg);
}

uint8_t dmg_mbc1_read(dmg_t const dmg, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x0000 ... 0x3FFF: /* ROM 0,32,64,96 */
            result = dmg_cartridge_read_rom(dmg, dmg->memory.mapper.mbc1.rom.bank[0], address);
            break;
        case 0x4000 ... 0x7FFF: /* ROM 1-31,33-63,65-95,97-127 */
            result = dmg_cartridge_read_rom(dmg, dmg->memory.mapper.mbc1.rom.bank[1], address - 0x4000);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0-3 */
            if (dmg->memory.mapper.mbc1.ram.enabled)
            {
                result = dmg_cartridge_read_ram(dmg, dmg->memory.mapper.mbc1.ram.bank, address - 0xA000);
            }
            break;
        default:
            break;
    }
    return result;
}

void dmg_mbc1_write(dmg_t const dmg, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0x0000 ... 0x1FFF: /* RAM ENABLE */
            dmg->memory.mapper.mbc1.ram.enabled = ((value & 0x0F) == 0x0A);
            break;
        case 0x2000 ... 0x3FFF: /* BANK LOW */
            dmg->memory.mapper.mbc1.bank.low = value;
            dmg_mbc1_update(dmg);
            break;
        case 0x4000 ... 0x5FFF: /* BANK HIGH */
            dmg->memory.mapper.mbc1.bank.high = value;
            dmg_mbc1_update(dmg);
            break;
        case 0x6000 ... 0x7FFF: /* BANK SELECT */
            dmg->memory.mapper.mbc1.bank.select = value;
            dmg_mbc1_update(dmg);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0-3 */
            if (dmg->memory.mapper.mbc1.ram.enabled)
            {
                dmg_cartridge_write_ram(dmg, dmg->memory.mapper.mbc1.ram.bank, address - 0xA000, value);
            }
            break;
        default:
            break;
    }
}
