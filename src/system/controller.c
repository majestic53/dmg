/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

uint8_t dmg_controller_read(dmg_t const dmg, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0xFF00: /* P1 */
            if (!dmg->controller.control.direction)
            {
                for (dmg_button_e button = DMG_BUTTON_RIGHT; button <= DMG_BUTTON_DOWN; ++button)
                {
                    if (dmg->controller.state[button])
                    {
                        dmg->controller.control.state &= ~(1 << (button - DMG_BUTTON_RIGHT));
                    }
                }
            }
            if (!dmg->controller.control.button)
            {
                for (dmg_button_e button = DMG_BUTTON_A; button <= DMG_BUTTON_START; ++button)
                {
                    if (dmg->controller.state[button])
                    {
                        dmg->controller.control.state &= ~(1 << (button - DMG_BUTTON_A));
                    }
                }
            }
            result = dmg->controller.control.raw;
            break;
        default:
            break;
    }
    return result;
}

void dmg_controller_update(dmg_t const dmg, dmg_button_e button, bool state)
{
    if ((!dmg->controller.control.button || !dmg->controller.control.direction) && !dmg->controller.state[button] && state)
    {
        dmg_processor_interrupt(dmg, DMG_INTERRUPT_INPUT);
    }
    dmg->controller.state[button] = state;
}

void dmg_controller_write(dmg_t const dmg, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0xFF00: /* P1 */
            dmg->controller.control.raw = 0xCF | value;
            break;
        default:
            break;
    }
}
