/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <common.h>

void dmg_serial_clock(dmg_handle_t const handle)
{
    if (handle->serial.control.enabled && handle->serial.control.mode)
    {
        if (!handle->serial.delay)
        {
            handle->serial.data = (handle->serial.data << 1) | (handle->serial.output((handle->serial.data & 0x80) ? 1 : 0) & 1);
            if (++handle->serial.index > 7)
            {
                dmg_processor_interrupt(handle, DMG_INTERRUPT_SERIAL);
                handle->serial.control.enabled = false;
                handle->serial.index = 0;
            }
            else
            {
                handle->serial.delay = 512; /* 8KHz */
            }
        }
        --handle->serial.delay;
    }
}

dmg_error_e dmg_serial_initialize(dmg_handle_t const handle, const dmg_output_f output)
{
    if (!output)
    {
        return DMG_ERROR(handle, "Invalid serial output -- %p", output);
    }
    handle->serial.output = output;
    return DMG_SUCCESS;
}

uint8_t dmg_serial_input(dmg_handle_t const handle, uint8_t value)
{
    uint8_t result;
    if (!handle->serial.control.enabled)
    {
        handle->serial.control.enabled = true;
        handle->serial.index = 0;
    }
    result = (handle->serial.data & 0x80) ? 1 : 0;
    handle->serial.data = (handle->serial.data << 1) | (value & 1);
    if (++handle->serial.index > 7)
    {
        dmg_processor_interrupt(handle, DMG_INTERRUPT_SERIAL);
        handle->serial.control.enabled = false;
        handle->serial.index = 0;
    }
    return result;
}

uint8_t dmg_serial_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0xFF01: /* SB */
            result = handle->serial.data;
            break;
        case 0xFF02: /* SC */
            result = handle->serial.control.raw;
            break;
        default:
            break;
    }
    return result;
}

void dmg_serial_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0xFF01: /* SB */
            handle->serial.data = value;
            break;
        case 0xFF02: /* SC */
            handle->serial.control.raw = 0x7E | value;
            if (handle->serial.control.enabled)
            {
                handle->serial.delay = 0;
                handle->serial.index = 0;
            }
            break;
        default:
            break;
    }
}
