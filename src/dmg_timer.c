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

#include <dmg_internal.h>

static const uint16_t OFFSET[] =
{
    /* TIMER (4KHz,256KHz,64KHz,16KHz) */
    512, 8, 32, 128,
    /* AUDIO (256Hz) */
    8192,
};

void dmg_timer_clock(dmg_handle_t const handle)
{
    bool overflow;
    ++handle->timer.divider;
    if (handle->timer.control.enabled)
    {
        overflow = handle->timer.divider & OFFSET[handle->timer.control.mode];
        if (handle->timer.overflow[0] && !overflow && !++handle->timer.counter)
        {
            handle->timer.counter = handle->timer.modulo;
            dmg_processor_interrupt(handle, DMG_INTERRUPT_TIMER);
        }
        handle->timer.overflow[0] = overflow;
    }
    overflow = handle->timer.divider & OFFSET[4];
    if (handle->timer.overflow[1] && !overflow)
    {
        dmg_audio_interrupt(handle);
    }
    handle->timer.overflow[1] = overflow;
}

uint8_t dmg_timer_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0xFF04: /* DIV */
            result = handle->timer.divider >> 8;
            break;
        case 0xFF05: /* TIMA */
            result = handle->timer.counter;
            break;
        case 0xFF06: /* TMA */
            result = handle->timer.modulo;
            break;
        case 0xFF07: /* TAC */
            result = handle->timer.control.raw;
            break;
        default:
            break;
    }
    return result;
}

void dmg_timer_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0xFF04: /* DIV */
            handle->timer.divider = 0;
            break;
        case 0xFF05: /* TIMA */
            handle->timer.counter = value;
            break;
        case 0xFF06: /* TMA */
            handle->timer.modulo = value;
            break;
        case 0xFF07: /* TAC */
            handle->timer.control.raw = value & 7;
            break;
        default:
            break;
    }
}
