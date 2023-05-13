/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

void dmg_serial_clock(dmg_t const dmg)
{
    if (dmg->serial.control.enabled && dmg->serial.control.mode)
    {
        if (!dmg->serial.delay)
        {
            dmg->serial.data = (dmg->serial.data << 1) | (dmg->serial.output((dmg->serial.data & 0x80) ? 1 : 0) & 1);
            if (++dmg->serial.index > 7)
            {
                dmg_processor_interrupt(dmg, DMG_INTERRUPT_SERIAL);
                dmg->serial.control.enabled = false;
                dmg->serial.index = 0;
            }
            else
            {
                dmg->serial.delay = 512; /* 8KHz */
            }
        }
        --dmg->serial.delay;
    }
}

dmg_error_e dmg_serial_initialize(dmg_t const dmg, const dmg_output_f output)
{
    if (!output)
    {
        return DMG_ERROR(dmg, "Invalid serial output -- %p", output);
    }
    dmg->serial.output = output;
    return DMG_SUCCESS;
}

uint8_t dmg_serial_input(dmg_t const dmg, uint8_t value)
{
    uint8_t result;
    if (!dmg->serial.control.enabled)
    {
        dmg->serial.control.enabled = true;
        dmg->serial.index = 0;
    }
    result = (dmg->serial.data & 0x80) ? 1 : 0;
    dmg->serial.data = (dmg->serial.data << 1) | (value & 1);
    if (++dmg->serial.index > 7)
    {
        dmg_processor_interrupt(dmg, DMG_INTERRUPT_SERIAL);
        dmg->serial.control.enabled = false;
        dmg->serial.index = 0;
    }
    return result;
}

uint8_t dmg_serial_read(dmg_t const dmg, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0xFF01: /* SB */
            result = dmg->serial.data;
            break;
        case 0xFF02: /* SC */
            result = dmg->serial.control.raw;
            break;
        default:
            break;
    }
    return result;
}

void dmg_serial_write(dmg_t const dmg, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0xFF01: /* SB */
            dmg->serial.data = value;
            break;
        case 0xFF02: /* SC */
            dmg->serial.control.raw = 0x7E | value;
            if (dmg->serial.control.enabled)
            {
                dmg->serial.delay = 0;
                dmg->serial.index = 0;
            }
            break;
        default:
            break;
    }
}
