/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

static void dmg_mbc3_update_banks(dmg_t const dmg)
{
    dmg->memory.mapper.mbc3.ram.bank = (dmg->memory.mapper.mbc3.bank.ram & 3) & (dmg->memory.cartridge.ram.count - 1);
    dmg->memory.mapper.mbc3.rom.bank[0] = 0;
    dmg->memory.mapper.mbc3.rom.bank[1] = dmg->memory.mapper.mbc3.bank.rom & 127;
    if (!dmg->memory.mapper.mbc3.rom.bank[1])
    { /* BANK 0->1 */
        ++dmg->memory.mapper.mbc3.rom.bank[1];
    }
    dmg->memory.mapper.mbc3.rom.bank[1] &= dmg->memory.cartridge.rom.count - 1;
}

static void dmg_mbc3_update_rtc(dmg_t const dmg)
{
    if(++dmg->memory.mapper.mbc3.rtc.counter[0].second.counter == 60)
    {
        dmg->memory.mapper.mbc3.rtc.counter[0].second.counter = 0;
        if(++dmg->memory.mapper.mbc3.rtc.counter[0].minute.counter == 60)
        {
            dmg->memory.mapper.mbc3.rtc.counter[0].minute.counter = 0;
            if(++dmg->memory.mapper.mbc3.rtc.counter[0].hour.counter == 24)
            {
                dmg->memory.mapper.mbc3.rtc.counter[0].hour.counter = 0;
                dmg->memory.mapper.mbc3.rtc.counter[0].day.carry = (dmg->memory.mapper.mbc3.rtc.counter[0].day.counter == 511);
                if(dmg->memory.mapper.mbc3.rtc.counter[0].day.carry)
                {
                    dmg->memory.mapper.mbc3.rtc.counter[0].day.counter = 0;
                }
                else
                {
                    ++dmg->memory.mapper.mbc3.rtc.counter[0].day.counter;
                }
            }
        }
    }
}

void dmg_mbc3_clock(dmg_t const dmg)
{
    if (dmg->memory.mapper.mbc3.rtc.enabled && !dmg->memory.mapper.mbc3.rtc.counter[0].day.halt)
    {
        if (!dmg->memory.mapper.mbc3.rtc.delay)
        {
            dmg_mbc3_update_rtc(dmg);
            dmg->memory.mapper.mbc3.rtc.delay = 60; /* 1Hz */
        }
        --dmg->memory.mapper.mbc3.rtc.delay;
    }
}

void dmg_mbc3_initialize(dmg_t const dmg, bool enabled)
{
    dmg->memory.mapper.mbc3.rtc.enabled = enabled;
    dmg_mbc3_update_banks(dmg);
}

void dmg_mbc3_load(dmg_t const dmg, const void *const data, uint32_t length)
{
    if (length == sizeof (*dmg->memory.mapper.mbc3.rtc.counter))
    {
        memcpy(&dmg->memory.mapper.mbc3.rtc.counter[0], data, length);
    }
}

uint8_t dmg_mbc3_read(dmg_t const dmg, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x0000 ... 0x3FFF: /* ROM 0 */
            result = dmg_cartridge_read_rom(dmg, dmg->memory.mapper.mbc3.rom.bank[0], address);
            break;
        case 0x4000 ... 0x7FFF: /* ROM 1-127 */
            result = dmg_cartridge_read_rom(dmg, dmg->memory.mapper.mbc3.rom.bank[1], address - 0x4000);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0-3 */
            if (dmg->memory.mapper.mbc3.ram.enabled)
            {
                result = dmg_cartridge_read_ram(dmg, dmg->memory.mapper.mbc3.ram.bank, address - 0xA000);
            }
            break;
        default:
            break;
    }
    return result;
}

void dmg_mbc3_save(dmg_t const dmg, void *const data, uint32_t length)
{
    if (length == sizeof (*dmg->memory.mapper.mbc3.rtc.counter))
    {
        memcpy(data, &dmg->memory.mapper.mbc3.rtc.counter[0], length);
    }
}

void dmg_mbc3_write(dmg_t const dmg, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0x0000 ... 0x1FFF: /* RAM ENABLE */
            dmg->memory.mapper.mbc3.ram.enabled = ((value & 0x0F) == 0x0A);
            break;
        case 0x2000 ... 0x3FFF: /* ROM BANK */
            dmg->memory.mapper.mbc3.bank.rom = value;
            dmg_mbc3_update_banks(dmg);
            break;
        case 0x4000 ... 0x5FFF: /* RAM BANK */
            dmg->memory.mapper.mbc3.bank.ram = value & 3;
            dmg_mbc3_update_banks(dmg);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0-3 */
            if (dmg->memory.mapper.mbc3.ram.enabled)
            {
                dmg_cartridge_write_ram(dmg, dmg->memory.mapper.mbc3.ram.bank, address - 0xA000, value);
            }
            break;
        default:
            break;
    }
}
