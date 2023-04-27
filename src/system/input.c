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
