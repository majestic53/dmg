/*
 * DMG
 * Copyright (C) 2023 David Jolly
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <common.h>

static uint8_t dmg_serial_output(uint8_t value)
{
    return value;
}

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
    handle->serial.output = ((!output) ? dmg_serial_output : output);
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
                handle->serial.index = 0;
            }
            break;
        default:
            break;
    }
}
