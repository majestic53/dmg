/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

static void dmg_mbc3_update_banks(dmg_handle_t const handle)
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

static void dmg_mbc3_update_rtc(dmg_handle_t const handle)
{
    if(++handle->memory.mapper.mbc3.rtc.counter[0].second.counter == 60)
    {
        handle->memory.mapper.mbc3.rtc.counter[0].second.counter = 0;
        if(++handle->memory.mapper.mbc3.rtc.counter[0].minute.counter == 60)
        {
            handle->memory.mapper.mbc3.rtc.counter[0].minute.counter = 0;
            if(++handle->memory.mapper.mbc3.rtc.counter[0].hour.counter == 24)
            {
                handle->memory.mapper.mbc3.rtc.counter[0].hour.counter = 0;
                if((handle->memory.mapper.mbc3.rtc.counter[0].day.carry = (handle->memory.mapper.mbc3.rtc.counter[0].day.counter == 511)))
                {
                    handle->memory.mapper.mbc3.rtc.counter[0].day.counter = 0;
                }
                else
                {
                    ++handle->memory.mapper.mbc3.rtc.counter[0].day.counter;
                }
            }
        }
    }
}

void dmg_mbc3_clock(dmg_handle_t const handle)
{
    if (handle->memory.mapper.mbc3.rtc.enabled && !handle->memory.mapper.mbc3.rtc.counter[0].day.halt)
    {
        if (!handle->memory.mapper.mbc3.rtc.delay)
        {
            dmg_mbc3_update_rtc(handle);
            handle->memory.mapper.mbc3.rtc.delay = 60; /* 1Hz */
        }
        --handle->memory.mapper.mbc3.rtc.delay;
    }
}

void dmg_mbc3_initialize(dmg_handle_t const handle, bool enabled)
{
    handle->memory.mapper.mbc3.rtc.enabled = enabled;
    dmg_mbc3_update_banks(handle);
}

void dmg_mbc3_load(dmg_handle_t const handle, const void *const data, uint32_t length)
{
    if (length == sizeof (handle->memory.mapper.mbc3.rtc.counter))
    {
        memcpy(handle->memory.mapper.mbc3.rtc.counter, data, length);
    }
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

void dmg_mbc3_save(dmg_handle_t const handle, void *const data, uint32_t length)
{
    if (length == sizeof (handle->memory.mapper.mbc3.rtc.counter))
    {
        memcpy(data, handle->memory.mapper.mbc3.rtc.counter, length);
    }
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
            dmg_mbc3_update_banks(handle);
            break;
        case 0x4000 ... 0x5FFF: /* RAM BANK */
            handle->memory.mapper.mbc3.bank.ram = value & 3;
            dmg_mbc3_update_banks(handle);
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
