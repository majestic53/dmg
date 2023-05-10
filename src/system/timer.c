/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

static const uint16_t OFFSET[] =
{
    /* TIMER (4KHz,256KHz,64KHz,16KHz) */
    512, 8, 32, 128,
    /* AUDIO (256Hz) */
    8192,
};

static void dmg_timer_increment_audio(dmg_handle_t const handle)
{
    bool overflow = handle->timer.divider & OFFSET[4];
    if (handle->timer.overflow[1] && !overflow)
    {
        dmg_audio_update(handle);
    }
    handle->timer.overflow[1] = overflow;
}

static void dmg_timer_increment_counter(dmg_handle_t const handle)
{
    bool overflow = handle->timer.divider & OFFSET[handle->timer.control.mode];
    if (handle->timer.overflow[0] && !overflow && !++handle->timer.counter)
    {
        handle->timer.counter = handle->timer.modulo;
        dmg_processor_interrupt(handle, DMG_INTERRUPT_TIMER);
    }
    handle->timer.overflow[0] = overflow;
}

void dmg_timer_clock(dmg_handle_t const handle)
{
    ++handle->timer.divider;
    dmg_timer_increment_audio(handle);
    if (handle->timer.control.enabled)
    {
        dmg_timer_increment_counter(handle);
    }
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
            dmg_timer_increment_audio(handle);
            if (handle->timer.control.enabled)
            {
                dmg_timer_increment_counter(handle);
            }
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
