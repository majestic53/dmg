/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_PROCESSOR_H_
#define DMG_PROCESSOR_H_

#include <stdbool.h>
#include <common.h>

typedef enum
{
    DMG_INTERRUPT_VBLANK = 0,
    DMG_INTERRUPT_LCDC,
    DMG_INTERRUPT_TIMER,
    DMG_INTERRUPT_SERIAL,
    DMG_INTERRUPT_INPUT,
    DMG_INTERRUPT_MAX,
} dmg_interrupt_t;

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

dmg_error_t dmg_processor_clock(dmg_handle_t const handle);
void dmg_processor_interrupt(dmg_handle_t const handle, dmg_interrupt_t interrupt);
uint8_t dmg_processor_read(dmg_handle_t const handle, uint16_t address);
void dmg_processor_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_PROCESSOR_H_ */
