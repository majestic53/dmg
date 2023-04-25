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

#ifndef DMG_PROCESSOR_H_
#define DMG_PROCESSOR_H_

#include <common.h>

typedef enum
{
    DMG_INTERRUPT_VBLANK = 0,
    DMG_INTERRUPT_LCDC,
    DMG_INTERRUPT_TIMER,
    DMG_INTERRUPT_SERIAL,
    DMG_INTERRUPT_INPUT,
    DMG_INTERRUPT_MAX,
} dmg_interrupt_e;

typedef union
{
    struct
    {
        union
        {
            struct
            {
                uint8_t : 4;
                uint8_t carry : 1;
                uint8_t half_carry : 1;
                uint8_t negative : 1;
                uint8_t zero : 1;
            };
            uint8_t low;
        };
        uint8_t high;
    };
    uint16_t word;
} dmg_register_t;

typedef struct
{
    uint8_t delay;
    bool halted;
    bool stopped;
    dmg_register_t af;
    dmg_register_t bc;
    dmg_register_t de;
    dmg_register_t hl;
    dmg_register_t pc;
    dmg_register_t sp;
    struct
    {
        uint16_t address;
        uint8_t opcode;
    } instruction;
    struct
    {
        int8_t enable_delay;
        bool enabled;
        uint8_t enable;
        uint8_t flag;
    } interrupt;
} dmg_processor_t;

dmg_error_e dmg_processor_clock(dmg_handle_t const handle);
void dmg_processor_interrupt(dmg_handle_t const handle, dmg_interrupt_e interrupt);
uint8_t dmg_processor_read(dmg_handle_t const handle, uint16_t address);
void dmg_processor_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_PROCESSOR_H_ */
