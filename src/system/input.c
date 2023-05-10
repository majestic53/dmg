/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

uint8_t dmg_input_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0xFF00: /* P1 */
            if (!handle->input.control.direction)
            {
                for (dmg_button_e button = DMG_BUTTON_RIGHT; button <= DMG_BUTTON_DOWN; ++button)
                {
                    if (handle->input.state[button])
                    {
                        handle->input.control.state &= ~(1 << (button - DMG_BUTTON_RIGHT));
                    }
                }
            }
            if (!handle->input.control.button)
            {
                for (dmg_button_e button = DMG_BUTTON_A; button <= DMG_BUTTON_START; ++button)
                {
                    if (handle->input.state[button])
                    {
                        handle->input.control.state &= ~(1 << (button - DMG_BUTTON_A));
                    }
                }
            }
            result = handle->input.control.raw;
            break;
        default:
            break;
    }
    return result;
}

void dmg_input_update(dmg_handle_t const handle, dmg_button_e button, bool state)
{
    if ((!handle->input.control.button || !handle->input.control.direction) && !handle->input.state[button] && state)
    {
        dmg_processor_interrupt(handle, DMG_INTERRUPT_INPUT);
    }
    handle->input.state[button] = state;
}

void dmg_input_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0xFF00: /* P1 */
            handle->input.control.raw = 0xCF | value;
            break;
        default:
            break;
    }
}
