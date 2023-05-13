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

static void dmg_timer_increment_audio(dmg_t const dmg)
{
    bool overflow = dmg->timer.divider & OFFSET[4];
    if (dmg->timer.overflow[1] && !overflow)
    {
        dmg_audio_update(dmg);
    }
    dmg->timer.overflow[1] = overflow;
}

static void dmg_timer_increment_counter(dmg_t const dmg)
{
    bool overflow = dmg->timer.divider & OFFSET[dmg->timer.control.mode];
    if (dmg->timer.overflow[0] && !overflow && !++dmg->timer.counter)
    {
        dmg->timer.counter = dmg->timer.modulo;
        dmg_processor_interrupt(dmg, DMG_INTERRUPT_TIMER);
    }
    dmg->timer.overflow[0] = overflow;
}

void dmg_timer_clock(dmg_t const dmg)
{
    ++dmg->timer.divider;
    dmg_timer_increment_audio(dmg);
    if (dmg->timer.control.enabled)
    {
        dmg_timer_increment_counter(dmg);
    }
}

uint8_t dmg_timer_read(dmg_t const dmg, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0xFF04: /* DIV */
            result = dmg->timer.divider >> 8;
            break;
        case 0xFF05: /* TIMA */
            result = dmg->timer.counter;
            break;
        case 0xFF06: /* TMA */
            result = dmg->timer.modulo;
            break;
        case 0xFF07: /* TAC */
            result = dmg->timer.control.raw;
            break;
        default:
            break;
    }
    return result;
}

void dmg_timer_write(dmg_t const dmg, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0xFF04: /* DIV */
            dmg->timer.divider = 0;
            dmg_timer_increment_audio(dmg);
            if (dmg->timer.control.enabled)
            {
                dmg_timer_increment_counter(dmg);
            }
            break;
        case 0xFF05: /* TIMA */
            dmg->timer.counter = value;
            break;
        case 0xFF06: /* TMA */
            dmg->timer.modulo = value;
            break;
        case 0xFF07: /* TAC */
            dmg->timer.control.raw = value & 7;
            break;
        default:
            break;
    }
}
