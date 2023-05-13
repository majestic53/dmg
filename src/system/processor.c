/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

typedef void (*dmg_instruction_f)(dmg_t const dmg);

static void dmg_processor_instruction_adc(dmg_t const dmg)
{
    uint16_t sum;
    uint8_t carry, operand = 0;
    dmg->processor.delay = 4;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x88: /* B */
            operand = dmg->processor.bc.high;
            break;
        case 0x89: /* C */
            operand = dmg->processor.bc.low;
            break;
        case 0x8A: /* D */
            operand = dmg->processor.de.high;
            break;
        case 0x8B: /* E */
            operand = dmg->processor.de.low;
            break;
        case 0x8C: /* H */
            operand = dmg->processor.hl.high;
            break;
        case 0x8D: /* L */
            operand = dmg->processor.hl.low;
            break;
        case 0x8E: /* (HL) */
            dmg->processor.delay += 4;
            operand = dmg_system_read(dmg, dmg->processor.hl.word);
            break;
        case 0x8F: /* A */
            operand = dmg->processor.af.high;
            break;
        case 0xCE: /* # */
            dmg->processor.delay += 4;
            operand = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
    }
    carry = dmg->processor.af.carry;
    sum = dmg->processor.af.high + operand + carry;
    dmg->processor.af.carry = (sum > 0xFF);
    dmg->processor.af.half_carry = (((dmg->processor.af.high & 0x0F) + (operand & 0x0F) + carry) > 0x0F);
    dmg->processor.af.negative = false;
    dmg->processor.af.zero = !(sum & 0xFF);
    dmg->processor.af.high = sum;
}

static void dmg_processor_instruction_add(dmg_t const dmg)
{
    uint8_t operand = 0;
    uint16_t carry, sum;
    dmg->processor.delay = 4;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x80: /* B */
            operand = dmg->processor.bc.high;
            break;
        case 0x81: /* C */
            operand = dmg->processor.bc.low;
            break;
        case 0x82: /* D */
            operand = dmg->processor.de.high;
            break;
        case 0x83: /* E */
            operand = dmg->processor.de.low;
            break;
        case 0x84: /* H */
            operand = dmg->processor.hl.high;
            break;
        case 0x85: /* L */
            operand = dmg->processor.hl.low;
            break;
        case 0x86: /* (HL) */
            dmg->processor.delay += 4;
            operand = dmg_system_read(dmg, dmg->processor.hl.word);
            break;
        case 0x87: /* A */
            operand = dmg->processor.af.high;
            break;
        case 0xC6: /* # */
            dmg->processor.delay += 4;
            operand = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
    }
    sum = dmg->processor.af.high + operand;
    carry = dmg->processor.af.high ^ operand ^ sum;
    dmg->processor.af.carry = ((carry & 0x100) == 0x100);
    dmg->processor.af.half_carry = ((carry & 0x10) == 0x10);
    dmg->processor.af.negative = false;
    dmg->processor.af.zero = !(sum & 0xFF);
    dmg->processor.af.high = sum;
}

static void dmg_processor_instruction_add_hl(dmg_t const dmg)
{
    uint32_t carry, sum;
    uint16_t operand = 0;
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x09: /* BC */
            operand = dmg->processor.bc.word;
            break;
        case 0x19: /* DE */
            operand = dmg->processor.de.word;
            break;
        case 0x29: /* HL */
            operand = dmg->processor.hl.word;
            break;
        case 0x39: /* SP */
            operand = dmg->processor.sp.word;
            break;
    }
    sum = dmg->processor.hl.word + operand;
    carry = dmg->processor.hl.word ^ operand ^ sum;
    dmg->processor.af.carry = ((carry & 0x10000) == 0x10000);
    dmg->processor.af.half_carry = ((carry & 0x1000) == 0x1000);
    dmg->processor.af.negative = false;
    dmg->processor.hl.word = sum;
}

static void dmg_processor_instruction_add_sp(dmg_t const dmg)
{
    uint32_t carry, sum;
    int8_t operand = dmg_system_read(dmg, dmg->processor.pc.word++);
    dmg->processor.delay = 16;
    sum = dmg->processor.sp.word + operand;
    carry = dmg->processor.sp.word ^ operand ^ sum;
    dmg->processor.af.carry = ((carry & 0x100) == 0x100);
    dmg->processor.af.half_carry = ((carry & 0x10) == 0x10);
    dmg->processor.af.negative = false;
    dmg->processor.af.zero = false;
    dmg->processor.sp.word = sum;
}

static void dmg_processor_instruction_and(dmg_t const dmg)
{
    dmg->processor.delay = 4;
    switch (dmg->processor.instruction.opcode)
    {
        case 0xA0: /* B */
            dmg->processor.af.high &= dmg->processor.bc.high;
            break;
        case 0xA1: /* C */
            dmg->processor.af.high &= dmg->processor.bc.low;
            break;
        case 0xA2: /* D */
            dmg->processor.af.high &= dmg->processor.de.high;
            break;
        case 0xA3: /* E */
            dmg->processor.af.high &= dmg->processor.de.low;
            break;
        case 0xA4: /* H */
            dmg->processor.af.high &= dmg->processor.hl.high;
            break;
        case 0xA5: /* L */
            dmg->processor.af.high &= dmg->processor.hl.low;
            break;
        case 0xA6: /* (HL) */
            dmg->processor.delay += 4;
            dmg->processor.af.high &= dmg_system_read(dmg, dmg->processor.hl.word);
            break;
        case 0xA7: /* A */
            break;
        case 0xE6: /* # */
            dmg->processor.delay += 4;
            dmg->processor.af.high &= dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
    }
    dmg->processor.af.carry = false;
    dmg->processor.af.half_carry = true;
    dmg->processor.af.negative = false;
    dmg->processor.af.zero = !dmg->processor.af.high;
}

static void dmg_processor_instruction_bit(dmg_t const dmg)
{
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x40: case 0x48: case 0x50: case 0x58:
        case 0x60: case 0x68: case 0x70: case 0x78: /* B */
            dmg->processor.af.zero = !(dmg->processor.bc.high & (1 << ((dmg->processor.instruction.opcode - 0x40) / 8)));
            break;
        case 0x41: case 0x49: case 0x51: case 0x59:
        case 0x61: case 0x69: case 0x71: case 0x79: /* C */
            dmg->processor.af.zero = !(dmg->processor.bc.low & (1 << ((dmg->processor.instruction.opcode - 0x41) / 8)));
            break;
        case 0x42: case 0x4A: case 0x52: case 0x5A:
        case 0x62: case 0x6A: case 0x72: case 0x7A: /* D */
            dmg->processor.af.zero = !(dmg->processor.de.high & (1 << ((dmg->processor.instruction.opcode - 0x42) / 8)));
            break;
        case 0x43: case 0x4B: case 0x53: case 0x5B:
        case 0x63: case 0x6B: case 0x73: case 0x7B: /* E */
            dmg->processor.af.zero = !(dmg->processor.de.low & (1 << ((dmg->processor.instruction.opcode - 0x43) / 8)));
            break;
        case 0x44: case 0x4C: case 0x54: case 0x5C:
        case 0x64: case 0x6C: case 0x74: case 0x7C: /* H */
            dmg->processor.af.zero = !(dmg->processor.hl.high & (1 << ((dmg->processor.instruction.opcode - 0x44) / 8)));
            break;
        case 0x45: case 0x4D: case 0x55: case 0x5D:
        case 0x65: case 0x6D: case 0x75: case 0x7D: /* L */
            dmg->processor.af.zero = !(dmg->processor.hl.low & (1 << ((dmg->processor.instruction.opcode - 0x45) / 8)));
            break;
        case 0x46: case 0x4E: case 0x56: case 0x5E:
        case 0x66: case 0x6E: case 0x76: case 0x7E: /* (HL) */
            dmg->processor.delay += 4;
            dmg->processor.af.zero = !(dmg_system_read(dmg, dmg->processor.hl.word) & (1 << ((dmg->processor.instruction.opcode - 0x46) / 8)));
            break;
        case 0x47: case 0x4F: case 0x57: case 0x5F:
        case 0x67: case 0x6F: case 0x77: case 0x7F: /* A */
            dmg->processor.af.zero = !(dmg->processor.af.high & (1 << ((dmg->processor.instruction.opcode - 0x47) / 8)));
            break;
    }
    dmg->processor.af.half_carry = true;
    dmg->processor.af.negative = false;
}

static void dmg_processor_instruction_call(dmg_t const dmg)
{
    bool taken = false;
    dmg_register_t operand = {};
    dmg->processor.delay = 12;
    operand.low = dmg_system_read(dmg, dmg->processor.pc.word++);
    operand.high = dmg_system_read(dmg, dmg->processor.pc.word++);
    switch (dmg->processor.instruction.opcode)
    {
        case 0xC4: /* NZ */
            taken = !dmg->processor.af.zero;
            break;
        case 0xCC: /* Z */
            taken = dmg->processor.af.zero;
            break;
        case 0xCD: /* N */
            taken = true;
            break;
        case 0xD4: /* NC */
            taken = !dmg->processor.af.carry;
            break;
        case 0xDC: /* C */
            taken = dmg->processor.af.carry;
            break;
    }
    if (taken)
    {
        dmg->processor.delay += 12;
        dmg_system_write(dmg, --dmg->processor.sp.word, dmg->processor.pc.high);
        dmg_system_write(dmg, --dmg->processor.sp.word, dmg->processor.pc.low);
        dmg->processor.pc.word = operand.word;
    }
}

static void dmg_processor_instruction_ccf(dmg_t const dmg)
{
    dmg->processor.delay = 4;
    dmg->processor.af.carry = !dmg->processor.af.carry;
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
}

static void dmg_processor_instruction_cp(dmg_t const dmg)
{
    uint8_t operand = 0;
    dmg->processor.delay = 4;
    switch (dmg->processor.instruction.opcode)
    {
        case 0xB8: /* B */
            operand = dmg->processor.bc.high;
            break;
        case 0xB9: /* C */
            operand = dmg->processor.bc.low;
            break;
        case 0xBA: /* D */
            operand = dmg->processor.de.high;
            break;
        case 0xBB: /* E */
            operand = dmg->processor.de.low;
            break;
        case 0xBC: /* H */
            operand = dmg->processor.hl.high;
            break;
        case 0xBD: /* L */
            operand = dmg->processor.hl.low;
            break;
        case 0xBE: /* (HL) */
            dmg->processor.delay += 4;
            operand = dmg_system_read(dmg, dmg->processor.hl.word);
            break;
        case 0xBF: /* A */
            operand = dmg->processor.af.high;
            break;
        case 0xFE: /* # */
            dmg->processor.delay += 4;
            operand = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
    }
    dmg->processor.af.carry = (dmg->processor.af.high < operand);
    dmg->processor.af.half_carry = ((dmg->processor.af.high & 0x0F) < ((dmg->processor.af.high - operand) & 0x0F));
    dmg->processor.af.negative = true;
    dmg->processor.af.zero = (dmg->processor.af.high == operand);
}

static void dmg_processor_instruction_cpl(dmg_t const dmg)
{
    dmg->processor.delay = 4;
    dmg->processor.af.high = ~dmg->processor.af.high;
    dmg->processor.af.half_carry = true;
    dmg->processor.af.negative = true;
}

static void dmg_processor_instruction_daa(dmg_t const dmg)
{
    dmg->processor.delay = 4;
    if (!dmg->processor.af.negative)
    {
        if (dmg->processor.af.carry || (dmg->processor.af.high > 0x99))
        {
            dmg->processor.af.high += 0x60;
            dmg->processor.af.carry = true;
        }
        if (dmg->processor.af.half_carry || ((dmg->processor.af.high & 0x0F) > 0x09))
        {
            dmg->processor.af.high += 0x06;
        }
    }
    else
    {
        if (dmg->processor.af.carry)
        {
            dmg->processor.af.high -= 0x60;
        }
        if (dmg->processor.af.half_carry)
        {
            dmg->processor.af.high -= 0x06;
        }
    }
    dmg->processor.af.half_carry = false;
    dmg->processor.af.zero = !dmg->processor.af.high;
}

static void dmg_processor_instruction_di(dmg_t const dmg)
{
    dmg->processor.delay = 4;
    dmg->processor.interrupt.enable_delay = 0;
    dmg->processor.interrupt.enabled = false;
}

static void dmg_processor_instruction_ei(dmg_t const dmg)
{
    dmg->processor.delay = 4;
    if (!dmg->processor.interrupt.enable_delay)
    {
        dmg->processor.interrupt.enable_delay = 2;
    }
}

static void dmg_processor_instruction_dec(dmg_t const dmg)
{
    uint8_t operand = 0;
    dmg->processor.delay = 4;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x05: /* B */
            operand = --dmg->processor.bc.high;
            break;
        case 0x0D: /* C */
            operand = --dmg->processor.bc.low;
            break;
        case 0x15: /* D */
            operand = --dmg->processor.de.high;
            break;
        case 0x1D: /* E */
            operand = --dmg->processor.de.low;
            break;
        case 0x25: /* H */
            operand = --dmg->processor.hl.high;
            break;
        case 0x2D: /* L */
            operand = --dmg->processor.hl.low;
            break;
        case 0x35: /* (HL) */
            dmg->processor.delay += 8;
            operand = dmg_system_read(dmg, dmg->processor.hl.word) - 1;
            dmg_system_write(dmg, dmg->processor.hl.word, operand);
            break;
        case 0x3D: /* A */
            operand = --dmg->processor.af.high;
            break;
    }
    dmg->processor.af.half_carry = ((operand & 0x0F) == 0x0F);
    dmg->processor.af.negative = true;
    dmg->processor.af.zero = !operand;
}

static void dmg_processor_instruction_dec_word(dmg_t const dmg)
{
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x0B: /* BC */
            --dmg->processor.bc.word;
            break;
        case 0x1B: /* DE */
            --dmg->processor.de.word;
            break;
        case 0x2B: /* HL */
            --dmg->processor.hl.word;
            break;
        case 0x3B: /* SP */
            --dmg->processor.sp.word;
            break;
    }
}

static void dmg_processor_instruction_halt(dmg_t const dmg)
{
    dmg->processor.delay = 4;
    dmg->processor.halted = true;
}

static void dmg_processor_instruction_inc(dmg_t const dmg)
{
    uint8_t operand = 0;
    dmg->processor.delay = 4;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x04: /* B */
            operand = ++dmg->processor.bc.high;
            break;
        case 0x0C: /* C */
            operand = ++dmg->processor.bc.low;
            break;
        case 0x14: /* D */
            operand = ++dmg->processor.de.high;
            break;
        case 0x1C: /* E */
            operand = ++dmg->processor.de.low;
            break;
        case 0x24: /* H */
            operand = ++dmg->processor.hl.high;
            break;
        case 0x2C: /* L */
            operand = ++dmg->processor.hl.low;
            break;
        case 0x34: /* (HL) */
            dmg->processor.delay += 8;
            operand = dmg_system_read(dmg, dmg->processor.hl.word) + 1;
            dmg_system_write(dmg, dmg->processor.hl.word, operand);
            break;
        case 0x3C: /* A */
            operand = ++dmg->processor.af.high;
            break;
    }
    dmg->processor.af.half_carry = !(operand & 0x0F);
    dmg->processor.af.negative = false;
    dmg->processor.af.zero = !operand;
}

static void dmg_processor_instruction_inc_word(dmg_t const dmg)
{
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x03: /* BC */
            ++dmg->processor.bc.word;
            break;
        case 0x13: /* DE */
            ++dmg->processor.de.word;
            break;
        case 0x23: /* HL */
            ++dmg->processor.hl.word;
            break;
        case 0x33: /* SP */
            ++dmg->processor.sp.word;
            break;
    }
}

static void dmg_processor_instruction_jp(dmg_t const dmg)
{
    bool taken = false;
    dmg_register_t operand = {};
    dmg->processor.delay = 12;
    operand.low = dmg_system_read(dmg, dmg->processor.pc.word++);
    operand.high = dmg_system_read(dmg, dmg->processor.pc.word++);
    switch (dmg->processor.instruction.opcode)
    {
        case 0xC2: /* NZ */
            taken = !dmg->processor.af.zero;
            break;
        case 0xC3: /* N */
            taken = true;
            break;
        case 0xCA: /* Z */
            taken = dmg->processor.af.zero;
            break;
        case 0xD2: /* NC */
            taken = !dmg->processor.af.carry;
            break;
        case 0xDA: /* C */
            taken = dmg->processor.af.carry;
            break;
    }
    if (taken)
    {
        dmg->processor.delay += 4;
        dmg->processor.pc.word = operand.word;
    }
}

static void dmg_processor_instruction_jp_hl(dmg_t const dmg)
{
    dmg->processor.delay = 4;
    dmg->processor.pc.word = dmg->processor.hl.word;
}

static void dmg_processor_instruction_jr(dmg_t const dmg)
{
    bool taken = false;
    int8_t operand = dmg_system_read(dmg, dmg->processor.pc.word++);
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x18: /* N */
            taken = true;
            break;
        case 0x20: /* NZ */
            taken = !dmg->processor.af.zero;
            break;
        case 0x28: /* Z */
            taken = dmg->processor.af.zero;
            break;
        case 0x30: /* NC */
            taken = !dmg->processor.af.carry;
            break;
        case 0x38: /* C */
            taken = dmg->processor.af.carry;
            break;
    }
    if (taken)
    {
        dmg->processor.delay += 4;
        dmg->processor.pc.word += operand;
    }
}

static void dmg_processor_instruction_ld(dmg_t const dmg)
{
    dmg_register_t operand = {};
    dmg->processor.delay = 4;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x01: /* BC,## */
            dmg->processor.delay += 8;
            dmg->processor.bc.low = dmg_system_read(dmg, dmg->processor.pc.word++);
            dmg->processor.bc.high = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
        case 0x02: /* (BC),A */
            dmg->processor.delay += 4;
            dmg_system_write(dmg, dmg->processor.bc.word, dmg->processor.af.high);
            break;
        case 0x06: /* B,# */
            dmg->processor.delay += 4;
            dmg->processor.bc.high = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
        case 0x08: /* (##),SP */
            dmg->processor.delay += 16;
            operand.low = dmg_system_read(dmg, dmg->processor.pc.word++);
            operand.high = dmg_system_read(dmg, dmg->processor.pc.word++);
            dmg_system_write(dmg, operand.word, dmg->processor.sp.low);
            dmg_system_write(dmg, operand.word + 1, dmg->processor.sp.high);
            break;
        case 0x0A: /* A,(BC) */
            dmg->processor.delay += 4;
            dmg->processor.af.high = dmg_system_read(dmg, dmg->processor.bc.word);
            break;
        case 0x0E: /* C,# */
            dmg->processor.delay += 4;
            dmg->processor.bc.low = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
        case 0x11: /* DE,## */
            dmg->processor.delay += 8;
            dmg->processor.de.low = dmg_system_read(dmg, dmg->processor.pc.word++);
            dmg->processor.de.high = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
        case 0x12: /* (DE),A */
            dmg->processor.delay += 4;
            dmg_system_write(dmg, dmg->processor.de.word, dmg->processor.af.high);
            break;
        case 0x16: /* D,# */
            dmg->processor.delay += 4;
            dmg->processor.de.high = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
        case 0x1A: /* A,(DE) */
            dmg->processor.delay += 4;
            dmg->processor.af.high = dmg_system_read(dmg, dmg->processor.de.word);
            break;
        case 0x1E: /* E,# */
            dmg->processor.delay += 4;
            dmg->processor.de.low = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
        case 0x21: /* HL,## */
            dmg->processor.delay += 8;
            dmg->processor.hl.low = dmg_system_read(dmg, dmg->processor.pc.word++);
            dmg->processor.hl.high = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
        case 0x22: /* (HL+),A */
            dmg->processor.delay += 4;
            dmg_system_write(dmg, dmg->processor.hl.word++, dmg->processor.af.high);
            break;
        case 0x26: /* H,# */
            dmg->processor.delay += 4;
            dmg->processor.hl.high = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
        case 0x2A: /* A,(HL+) */
            dmg->processor.delay += 4;
            dmg->processor.af.high = dmg_system_read(dmg, dmg->processor.hl.word++);
            break;
        case 0x2E: /* L,# */
            dmg->processor.delay += 4;
            dmg->processor.hl.low = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
        case 0x31: /* SP,## */
            dmg->processor.delay += 8;
            dmg->processor.sp.low = dmg_system_read(dmg, dmg->processor.pc.word++);
            dmg->processor.sp.high = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
        case 0x32: /* (HL-),A */
            dmg->processor.delay += 4;
            dmg_system_write(dmg, dmg->processor.hl.word--, dmg->processor.af.high);
            break;
        case 0x36: /* (HL),# */
            dmg->processor.delay += 8;
            dmg_system_write(dmg, dmg->processor.hl.word, dmg_system_read(dmg, dmg->processor.pc.word++));
            break;
        case 0x3A: /* A,(HL-) */
            dmg->processor.delay += 4;
            dmg->processor.af.high = dmg_system_read(dmg, dmg->processor.hl.word--);
            break;
        case 0x3E: /* A,# */
            dmg->processor.delay += 4;
            dmg->processor.af.high = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
        case 0x40: /* B,B */
            break;
        case 0x41: /* B,C */
            dmg->processor.bc.high = dmg->processor.bc.low;
            break;
        case 0x42: /* B,D */
            dmg->processor.bc.high = dmg->processor.de.high;
            break;
        case 0x43: /* B,E */
            dmg->processor.bc.high = dmg->processor.de.low;
            break;
        case 0x44: /* B,H */
            dmg->processor.bc.high = dmg->processor.hl.high;
            break;
        case 0x45: /* B,L */
            dmg->processor.bc.high = dmg->processor.hl.low;
            break;
        case 0x46: /* B,(HL) */
            dmg->processor.delay += 4;
            dmg->processor.bc.high = dmg_system_read(dmg, dmg->processor.hl.word);
            break;
        case 0x47: /* B,A */
            dmg->processor.bc.high = dmg->processor.af.high;
            break;
        case 0x48: /* C,B */
            dmg->processor.bc.low = dmg->processor.bc.high;
            break;
        case 0x49: /* C,C */
            break;
        case 0x4A: /* C,D */
            dmg->processor.bc.low = dmg->processor.de.high;
            break;
        case 0x4B: /* C,E */
            dmg->processor.bc.low = dmg->processor.de.low;
            break;
        case 0x4C: /* C,H */
            dmg->processor.bc.low = dmg->processor.hl.high;
            break;
        case 0x4D: /* C,L */
            dmg->processor.bc.low = dmg->processor.hl.low;
            break;
        case 0x4E: /* C,(HL) */
            dmg->processor.delay += 4;
            dmg->processor.bc.low = dmg_system_read(dmg, dmg->processor.hl.word);
            break;
        case 0x4F: /* C,A */
            dmg->processor.bc.low = dmg->processor.af.high;
            break;
        case 0x50: /* D,B */
            dmg->processor.de.high = dmg->processor.bc.high;
            break;
        case 0x51: /* D,C */
            dmg->processor.de.high = dmg->processor.bc.low;
            break;
        case 0x52: /* D,D */
            break;
        case 0x53: /* D,E */
            dmg->processor.de.high = dmg->processor.de.low;
            break;
        case 0x54: /* D,H */
            dmg->processor.de.high = dmg->processor.hl.high;
            break;
        case 0x55: /* D,L */
            dmg->processor.de.high = dmg->processor.hl.low;
            break;
        case 0x56: /* D,(HL) */
            dmg->processor.delay += 4;
            dmg->processor.de.high = dmg_system_read(dmg, dmg->processor.hl.word);
            break;
        case 0x57: /* D,A */
            dmg->processor.de.high = dmg->processor.af.high;
            break;
        case 0x58: /* E,B */
            dmg->processor.de.low = dmg->processor.bc.high;
            break;
        case 0x59: /* E,C */
            dmg->processor.de.low = dmg->processor.bc.low;
            break;
        case 0x5A: /* E,D */
            dmg->processor.de.low = dmg->processor.de.high;
            break;
        case 0x5B: /* E,E */
            break;
        case 0x5C: /* E,H */
            dmg->processor.de.low = dmg->processor.hl.high;
            break;
        case 0x5D: /* E,L */
            dmg->processor.de.low = dmg->processor.hl.low;
            break;
        case 0x5E: /* E,(HL) */
            dmg->processor.delay += 4;
            dmg->processor.de.low = dmg_system_read(dmg, dmg->processor.hl.word);
            break;
        case 0x5F: /* E,A */
            dmg->processor.de.low = dmg->processor.af.high;
            break;
        case 0x60: /* H,B */
            dmg->processor.hl.high = dmg->processor.bc.high;
            break;
        case 0x61: /* H,C */
            dmg->processor.hl.high = dmg->processor.bc.low;
            break;
        case 0x62: /* H,D */
            dmg->processor.hl.high = dmg->processor.de.high;
            break;
        case 0x63: /* H,E */
            dmg->processor.hl.high = dmg->processor.de.low;
            break;
        case 0x64: /* H,H */
            break;
        case 0x65: /* H,L */
            dmg->processor.hl.high = dmg->processor.hl.low;
            break;
        case 0x66: /* H,(HL) */
            dmg->processor.delay += 4;
            dmg->processor.hl.high = dmg_system_read(dmg, dmg->processor.hl.word);
            break;
        case 0x67: /* H,A */
            dmg->processor.hl.high = dmg->processor.af.high;
            break;
        case 0x68: /* L,B */
            dmg->processor.hl.low = dmg->processor.bc.high;
            break;
        case 0x69: /* L,C */
            dmg->processor.hl.low = dmg->processor.bc.low;
            break;
        case 0x6A: /* L,D */
            dmg->processor.hl.low = dmg->processor.de.high;
            break;
        case 0x6B: /* L,E */
            dmg->processor.hl.low = dmg->processor.de.low;
            break;
        case 0x6C: /* L,H */
            dmg->processor.hl.low = dmg->processor.hl.high;
            break;
        case 0x6D: /* L,L */
            break;
        case 0x6E: /* L,(HL) */
            dmg->processor.delay += 4;
            dmg->processor.hl.low = dmg_system_read(dmg, dmg->processor.hl.word);
            break;
        case 0x6F: /* L,A */
            dmg->processor.hl.low = dmg->processor.af.high;
            break;
        case 0x70: /* (HL),B */
            dmg->processor.delay += 4;
            dmg_system_write(dmg, dmg->processor.hl.word, dmg->processor.bc.high);
            break;
        case 0x71: /* (HL),C */
            dmg->processor.delay += 4;
            dmg_system_write(dmg, dmg->processor.hl.word, dmg->processor.bc.low);
            break;
        case 0x72: /* (HL),D */
            dmg->processor.delay += 4;
            dmg_system_write(dmg, dmg->processor.hl.word, dmg->processor.de.high);
            break;
        case 0x73: /* (HL),E */
            dmg->processor.delay += 4;
            dmg_system_write(dmg, dmg->processor.hl.word, dmg->processor.de.low);
            break;
        case 0x74: /* (HL),H */
            dmg->processor.delay += 4;
            dmg_system_write(dmg, dmg->processor.hl.word, dmg->processor.hl.high);
            break;
        case 0x75: /* (HL),L */
            dmg->processor.delay += 4;
            dmg_system_write(dmg, dmg->processor.hl.word, dmg->processor.hl.low);
            break;
        case 0x77: /* (HL),A */
            dmg->processor.delay += 4;
            dmg_system_write(dmg, dmg->processor.hl.word, dmg->processor.af.high);
            break;
        case 0x78: /* A,B */
            dmg->processor.af.high = dmg->processor.bc.high;
            break;
        case 0x79: /* A,C */
            dmg->processor.af.high = dmg->processor.bc.low;
            break;
        case 0x7A: /* A,D */
            dmg->processor.af.high = dmg->processor.de.high;
            break;
        case 0x7B: /* A,E */
            dmg->processor.af.high = dmg->processor.de.low;
            break;
        case 0x7C: /* A,H */
            dmg->processor.af.high = dmg->processor.hl.high;
            break;
        case 0x7D: /* A,L */
            dmg->processor.af.high = dmg->processor.hl.low;
            break;
        case 0x7E: /* A,(HL) */
            dmg->processor.delay += 4;
            dmg->processor.af.high = dmg_system_read(dmg, dmg->processor.hl.word);
            break;
        case 0x7F: /* A,A */
            break;
        case 0xE0: /* (FF00+#),A */
            dmg->processor.delay += 8;
            dmg_system_write(dmg, 0xFF00 + dmg_system_read(dmg, dmg->processor.pc.word++), dmg->processor.af.high);
            break;
        case 0xE2: /* (FF00+C),A */
            dmg->processor.delay += 4;
            dmg_system_write(dmg, 0xFF00 + dmg->processor.bc.low, dmg->processor.af.high);
            break;
        case 0xEA: /* (##),A */
            dmg->processor.delay += 12;
            operand.low = dmg_system_read(dmg, dmg->processor.pc.word++);
            operand.high = dmg_system_read(dmg, dmg->processor.pc.word++);
            dmg_system_write(dmg, operand.word, dmg->processor.af.high);
            break;
        case 0xF0: /* A,(FF00+#) */
            dmg->processor.delay += 8;
            dmg->processor.af.high = dmg_system_read(dmg, 0xFF00 + dmg_system_read(dmg, dmg->processor.pc.word++));
            break;
        case 0xF2: /* A,(FF00+C) */
            dmg->processor.delay += 4;
            dmg->processor.af.high = dmg_system_read(dmg, 0xFF00 + dmg->processor.bc.low);
            break;
        case 0xF9: /* SP,HL */
            dmg->processor.delay += 4;
            dmg->processor.sp.word = dmg->processor.hl.word;
            break;
        case 0xFA: /* A,(##) */
            dmg->processor.delay += 12;
            operand.low = dmg_system_read(dmg, dmg->processor.pc.word++);
            operand.high = dmg_system_read(dmg, dmg->processor.pc.word++);
            dmg->processor.af.high = dmg_system_read(dmg, operand.word);
            break;
    }
}

static void dmg_processor_instruction_ld_hl(dmg_t const dmg)
{
    uint32_t carry, sum;
    int8_t operand = dmg_system_read(dmg, dmg->processor.pc.word++);
    dmg->processor.delay = 12;
    sum = dmg->processor.sp.word + operand;
    carry = dmg->processor.sp.word ^ operand ^ sum;
    dmg->processor.af.carry = ((carry & 0x100) == 0x100);
    dmg->processor.af.half_carry = ((carry & 0x10) == 0x10);
    dmg->processor.af.negative = false;
    dmg->processor.af.zero = false;
    dmg->processor.hl.word = sum;
}

static void dmg_processor_instruction_nop(dmg_t const dmg)
{
    dmg->processor.delay = 4;
}

static void dmg_processor_instruction_or(dmg_t const dmg)
{
    dmg->processor.delay = 4;
    switch (dmg->processor.instruction.opcode)
    {
        case 0xB0: /* B */
            dmg->processor.af.high |= dmg->processor.bc.high;
            break;
        case 0xB1: /* C */
            dmg->processor.af.high |= dmg->processor.bc.low;
            break;
        case 0xB2: /* D */
            dmg->processor.af.high |= dmg->processor.de.high;
            break;
        case 0xB3: /* E */
            dmg->processor.af.high |= dmg->processor.de.low;
            break;
        case 0xB4: /* H */
            dmg->processor.af.high |= dmg->processor.hl.high;
            break;
        case 0xB5: /* L */
            dmg->processor.af.high |= dmg->processor.hl.low;
            break;
        case 0xB6: /* (HL) */
            dmg->processor.delay += 4;
            dmg->processor.af.high |= dmg_system_read(dmg, dmg->processor.hl.word);
            break;
        case 0xB7: /* A */
            break;
        case 0xF6: /* # */
            dmg->processor.delay += 4;
            dmg->processor.af.high |= dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
    }
    dmg->processor.af.carry = false;
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
    dmg->processor.af.zero = !dmg->processor.af.high;
}

static void dmg_processor_instruction_pop(dmg_t const dmg)
{
    dmg->processor.delay = 12;
    switch (dmg->processor.instruction.opcode)
    {
        case 0xC1: /* BC */
            dmg->processor.bc.low = dmg_system_read(dmg, dmg->processor.sp.word++);
            dmg->processor.bc.high = dmg_system_read(dmg, dmg->processor.sp.word++);
            break;
        case 0xD1: /* DE */
            dmg->processor.de.low = dmg_system_read(dmg, dmg->processor.sp.word++);
            dmg->processor.de.high = dmg_system_read(dmg, dmg->processor.sp.word++);
            break;
        case 0xE1: /* HL */
            dmg->processor.hl.low = dmg_system_read(dmg, dmg->processor.sp.word++);
            dmg->processor.hl.high = dmg_system_read(dmg, dmg->processor.sp.word++);
            break;
        case 0xF1: /* AF */
            dmg->processor.af.low = dmg_system_read(dmg, dmg->processor.sp.word++) & 0xF0;
            dmg->processor.af.high = dmg_system_read(dmg, dmg->processor.sp.word++);
            break;
    }
}

static void dmg_processor_instruction_push(dmg_t const dmg)
{
    dmg->processor.delay = 16;
    switch (dmg->processor.instruction.opcode)
    {
        case 0xC5: /* BC */
            dmg_system_write(dmg, --dmg->processor.sp.word, dmg->processor.bc.high);
            dmg_system_write(dmg, --dmg->processor.sp.word, dmg->processor.bc.low);
            break;
        case 0xD5: /* DE */
            dmg_system_write(dmg, --dmg->processor.sp.word, dmg->processor.de.high);
            dmg_system_write(dmg, --dmg->processor.sp.word, dmg->processor.de.low);
            break;
        case 0xE5: /* HL */
            dmg_system_write(dmg, --dmg->processor.sp.word, dmg->processor.hl.high);
            dmg_system_write(dmg, --dmg->processor.sp.word, dmg->processor.hl.low);
            break;
        case 0xF5: /* AF */
            dmg_system_write(dmg, --dmg->processor.sp.word, dmg->processor.af.high);
            dmg_system_write(dmg, --dmg->processor.sp.word, dmg->processor.af.low);
            break;
    }
}

static void dmg_processor_instruction_res(dmg_t const dmg)
{
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x80: case 0x88: case 0x90: case 0x98:
        case 0xA0: case 0xA8: case 0xB0: case 0xB8: /* B */
            dmg->processor.bc.high &= ~(1 << ((dmg->processor.instruction.opcode - 0x80) / 8));
            break;
        case 0x81: case 0x89: case 0x91: case 0x99:
        case 0xA1: case 0xA9: case 0xB1: case 0xB9: /* C */
            dmg->processor.bc.low &= ~(1 << ((dmg->processor.instruction.opcode - 0x81) / 8));
            break;
        case 0x82: case 0x8A: case 0x92: case 0x9A:
        case 0xA2: case 0xAA: case 0xB2: case 0xBA: /* D */
            dmg->processor.de.high &= ~(1 << ((dmg->processor.instruction.opcode - 0x82) / 8));
            break;
        case 0x83: case 0x8B: case 0x93: case 0x9B:
        case 0xA3: case 0xAB: case 0xB3: case 0xBB: /* E */
            dmg->processor.de.low &= ~(1 << ((dmg->processor.instruction.opcode - 0x83) / 8));
            break;
        case 0x84: case 0x8C: case 0x94: case 0x9C:
        case 0xA4: case 0xAC: case 0xB4: case 0xBC: /* H */
            dmg->processor.hl.high &= ~(1 << ((dmg->processor.instruction.opcode - 0x84) / 8));
            break;
        case 0x85: case 0x8D: case 0x95: case 0x9D:
        case 0xA5: case 0xAD: case 0xB5: case 0xBD: /* L */
            dmg->processor.hl.low &= ~(1 << ((dmg->processor.instruction.opcode - 0x85) / 8));
            break;
        case 0x86: case 0x8E: case 0x96: case 0x9E:
        case 0xA6: case 0xAE: case 0xB6: case 0xBE: /* (HL) */
            dmg->processor.delay += 8;
            dmg_system_write(dmg, dmg->processor.hl.word, dmg_system_read(dmg, dmg->processor.hl.word)
                & ~(1 << ((dmg->processor.instruction.opcode - 0x86) / 8)));
            break;
        case 0x87: case 0x8F: case 0x97: case 0x9F:
        case 0xA7: case 0xAF: case 0xB7: case 0xBF: /* A */
            dmg->processor.af.high &= ~(1 << ((dmg->processor.instruction.opcode - 0x87) / 8));
            break;
    }
}

static void dmg_processor_instruction_ret(dmg_t const dmg)
{
    bool taken = false;
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0xC0: /* NZ */
            taken = !dmg->processor.af.zero;
            break;
        case 0xC8: /* Z */
            taken = dmg->processor.af.zero;
            break;
        case 0xC9: /* N */
            taken = true;
            break;
        case 0xD0: /* NC */
            taken = !dmg->processor.af.carry;
            break;
        case 0xD8: /* C */
            taken = dmg->processor.af.carry;
            break;
    }
    if (taken)
    {
        dmg->processor.delay += (dmg->processor.instruction.opcode == 0xC9) ? 8 : 12; /* N */
        dmg->processor.pc.low = dmg_system_read(dmg, dmg->processor.sp.word++);
        dmg->processor.pc.high = dmg_system_read(dmg, dmg->processor.sp.word++);
    }
}

static void dmg_processor_instruction_reti(dmg_t const dmg)
{
    dmg->processor.delay = 16;
    dmg->processor.pc.low = dmg_system_read(dmg, dmg->processor.sp.word++);
    dmg->processor.pc.high = dmg_system_read(dmg, dmg->processor.sp.word++);
    dmg->processor.interrupt.enable_delay = 0;
    dmg->processor.interrupt.enabled = true;
}

static void dmg_processor_instruction_rl(dmg_t const dmg)
{
    uint8_t carry = dmg->processor.af.carry, operand = 0;
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x10: /* B */
            dmg->processor.af.carry = ((dmg->processor.bc.high & 0x80) == 0x80);
            dmg->processor.bc.high = (dmg->processor.bc.high << 1) | carry;
            dmg->processor.af.zero = !dmg->processor.bc.high;
            break;
        case 0x11: /* C */
            dmg->processor.af.carry = ((dmg->processor.bc.low & 0x80) == 0x80);
            dmg->processor.bc.low = (dmg->processor.bc.low << 1) | carry;
            dmg->processor.af.zero = !dmg->processor.bc.low;
            break;
        case 0x12: /* D */
            dmg->processor.af.carry = ((dmg->processor.de.high & 0x80) == 0x80);
            dmg->processor.de.high = (dmg->processor.de.high << 1) | carry;
            dmg->processor.af.zero = !dmg->processor.de.high;
            break;
        case 0x13: /* E */
            dmg->processor.af.carry = ((dmg->processor.de.low & 0x80) == 0x80);
            dmg->processor.de.low = (dmg->processor.de.low << 1) | carry;
            dmg->processor.af.zero = !dmg->processor.de.low;
            break;
        case 0x14: /* H */
            dmg->processor.af.carry = ((dmg->processor.hl.high & 0x80) == 0x80);
            dmg->processor.hl.high = (dmg->processor.hl.high << 1) | carry;
            dmg->processor.af.zero = !dmg->processor.hl.high;
            break;
        case 0x15: /* L */
            dmg->processor.af.carry = ((dmg->processor.hl.low & 0x80) == 0x80);
            dmg->processor.hl.low = (dmg->processor.hl.low << 1) | carry;
            dmg->processor.af.zero = !dmg->processor.hl.low;
            break;
        case 0x16: /* (HL) */
            dmg->processor.delay += 8;
            operand = dmg_system_read(dmg, dmg->processor.hl.word);
            dmg->processor.af.carry = ((operand & 0x80) == 0x80);
            operand = (operand << 1) | carry;
            dmg->processor.af.zero = !operand;
            dmg_system_write(dmg, dmg->processor.hl.word, operand);
            break;
        case 0x17: /* A */
            dmg->processor.af.carry = ((dmg->processor.af.high & 0x80) == 0x80);
            dmg->processor.af.high = (dmg->processor.af.high << 1) | carry;
            dmg->processor.af.zero = !dmg->processor.af.high;
            break;
    }
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
}

static void dmg_processor_instruction_rla(dmg_t const dmg)
{
    uint8_t carry = dmg->processor.af.carry;
    dmg->processor.delay = 4;
    dmg->processor.af.carry = ((dmg->processor.af.high & 0x80) == 0x80);
    dmg->processor.af.high = (dmg->processor.af.high << 1) | carry;
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
    dmg->processor.af.zero = false;
}

static void dmg_processor_instruction_rlc(dmg_t const dmg)
{
    uint8_t operand = 0;
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x00: /* B */
            dmg->processor.af.carry = ((dmg->processor.bc.high & 0x80) == 0x80);
            dmg->processor.bc.high = (dmg->processor.bc.high << 1) | dmg->processor.af.carry;
            dmg->processor.af.zero = !dmg->processor.bc.high;
            break;
        case 0x01: /* C */
            dmg->processor.af.carry = ((dmg->processor.bc.low & 0x80) == 0x80);
            dmg->processor.bc.low = (dmg->processor.bc.low << 1) | dmg->processor.af.carry;
            dmg->processor.af.zero = !dmg->processor.bc.low;
            break;
        case 0x02: /* D */
            dmg->processor.af.carry = ((dmg->processor.de.high & 0x80) == 0x80);
            dmg->processor.de.high = (dmg->processor.de.high << 1) | dmg->processor.af.carry;
            dmg->processor.af.zero = !dmg->processor.de.high;
            break;
        case 0x03: /* E */
            dmg->processor.af.carry = ((dmg->processor.de.low & 0x80) == 0x80);
            dmg->processor.de.low = (dmg->processor.de.low << 1) | dmg->processor.af.carry;
            dmg->processor.af.zero = !dmg->processor.de.low;
            break;
        case 0x04: /* H */
            dmg->processor.af.carry = ((dmg->processor.hl.high & 0x80) == 0x80);
            dmg->processor.hl.high = (dmg->processor.hl.high << 1) | dmg->processor.af.carry;
            dmg->processor.af.zero = !dmg->processor.hl.high;
            break;
        case 0x05: /* L */
            dmg->processor.af.carry = ((dmg->processor.hl.low & 0x80) == 0x80);
            dmg->processor.hl.low = (dmg->processor.hl.low << 1) | dmg->processor.af.carry;
            dmg->processor.af.zero = !dmg->processor.hl.low;
            break;
        case 0x06: /* (HL) */
            dmg->processor.delay += 8;
            operand = dmg_system_read(dmg, dmg->processor.hl.word);
            dmg->processor.af.carry = ((operand & 0x80) == 0x80);
            operand = (operand << 1) | dmg->processor.af.carry;
            dmg->processor.af.zero = !operand;
            dmg_system_write(dmg, dmg->processor.hl.word, operand);
            break;
        case 0x07: /* A */
            dmg->processor.af.carry = ((dmg->processor.af.high & 0x80) == 0x80);
            dmg->processor.af.high = (dmg->processor.af.high << 1) | dmg->processor.af.carry;
            dmg->processor.af.zero = !dmg->processor.af.high;
            break;
    }
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
}

static void dmg_processor_instruction_rlca(dmg_t const dmg)
{
    dmg->processor.delay = 4;
    dmg->processor.af.carry = ((dmg->processor.af.high & 0x80) == 0x80);
    dmg->processor.af.high = (dmg->processor.af.high << 1) | dmg->processor.af.carry;
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
    dmg->processor.af.zero = false;
}

static void dmg_processor_instruction_rr(dmg_t const dmg)
{
    uint8_t carry = dmg->processor.af.carry, operand = 0;
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x18: /* B */
            dmg->processor.af.carry = dmg->processor.bc.high & 1;
            dmg->processor.bc.high = (dmg->processor.bc.high >> 1) | (carry ? 0x80 : 0);
            dmg->processor.af.zero = !dmg->processor.bc.high;
            break;
        case 0x19: /* C */
            dmg->processor.af.carry = dmg->processor.bc.low & 1;
            dmg->processor.bc.low = (dmg->processor.bc.low >> 1) | (carry ? 0x80 : 0);
            dmg->processor.af.zero = !dmg->processor.bc.low;
            break;
        case 0x1A: /* D */
            dmg->processor.af.carry = dmg->processor.de.high & 1;
            dmg->processor.de.high = (dmg->processor.de.high >> 1) | (carry ? 0x80 : 0);
            dmg->processor.af.zero = !dmg->processor.de.high;
            break;
        case 0x1B: /* E */
            dmg->processor.af.carry = dmg->processor.de.low & 1;
            dmg->processor.de.low = (dmg->processor.de.low >> 1) | (carry ? 0x80 : 0);
            dmg->processor.af.zero = !dmg->processor.de.low;
            break;
        case 0x1C: /* H */
            dmg->processor.af.carry = dmg->processor.hl.high & 1;
            dmg->processor.hl.high = (dmg->processor.hl.high >> 1) | (carry ? 0x80 : 0);
            dmg->processor.af.zero = !dmg->processor.hl.high;
            break;
        case 0x1D: /* L */
            dmg->processor.af.carry = dmg->processor.hl.low & 1;
            dmg->processor.hl.low = (dmg->processor.hl.low >> 1) | (carry ? 0x80 : 0);
            dmg->processor.af.zero = !dmg->processor.hl.low;
            break;
        case 0x1E: /* (HL) */
            dmg->processor.delay += 8;
            operand = dmg_system_read(dmg, dmg->processor.hl.word);
            dmg->processor.af.carry = operand & 1;
            operand = (operand >> 1) | (carry ? 0x80 : 0);
            dmg->processor.af.zero = !operand;
            dmg_system_write(dmg, dmg->processor.hl.word, operand);
            break;
        case 0x1F: /* A */
            dmg->processor.af.carry = dmg->processor.af.high & 1;
            dmg->processor.af.high = (dmg->processor.af.high >> 1) | (carry ? 0x80 : 0);
            dmg->processor.af.zero = !dmg->processor.af.high;
            break;
    }
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
}

static void dmg_processor_instruction_rra(dmg_t const dmg)
{
    uint8_t carry = dmg->processor.af.carry;
    dmg->processor.delay = 4;
    dmg->processor.af.carry = ((dmg->processor.af.high & 1) == 1);
    dmg->processor.af.high = (dmg->processor.af.high >> 1) | (carry ? 0x80 : 0);
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
    dmg->processor.af.zero = false;
}

static void dmg_processor_instruction_rrc(dmg_t const dmg)
{
    uint8_t operand = 0;
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x08: /* B */
            dmg->processor.af.carry = dmg->processor.bc.high & 1;
            dmg->processor.bc.high = (dmg->processor.bc.high >> 1) | (dmg->processor.af.carry ? 0x80 : 0);
            dmg->processor.af.zero = !dmg->processor.bc.high;
            break;
        case 0x09: /* C */
            dmg->processor.af.carry = dmg->processor.bc.low & 1;
            dmg->processor.bc.low = (dmg->processor.bc.low >> 1) | (dmg->processor.af.carry ? 0x80 : 0);
            dmg->processor.af.zero = !dmg->processor.bc.low;
            break;
        case 0x0A: /* D */
            dmg->processor.af.carry = dmg->processor.de.high & 1;
            dmg->processor.de.high = (dmg->processor.de.high >> 1) | (dmg->processor.af.carry ? 0x80 : 0);
            dmg->processor.af.zero = !dmg->processor.de.high;
            break;
        case 0x0B: /* E */
            dmg->processor.af.carry = dmg->processor.de.low & 1;
            dmg->processor.de.low = (dmg->processor.de.low >> 1) | (dmg->processor.af.carry ? 0x80 : 0);
            dmg->processor.af.zero = !dmg->processor.de.low;
            break;
        case 0x0C: /* H */
            dmg->processor.af.carry = dmg->processor.hl.high & 1;
            dmg->processor.hl.high = (dmg->processor.hl.high >> 1) | (dmg->processor.af.carry ? 0x80 : 0);
            dmg->processor.af.zero = !dmg->processor.hl.high;
            break;
        case 0x0D: /* L */
            dmg->processor.af.carry = dmg->processor.hl.low & 1;
            dmg->processor.hl.low = (dmg->processor.hl.low >> 1) | (dmg->processor.af.carry ? 0x80 : 0);
            dmg->processor.af.zero = !dmg->processor.hl.low;
            break;
        case 0x0E: /* (HL) */
            dmg->processor.delay += 8;
            operand = dmg_system_read(dmg, dmg->processor.hl.word);
            dmg->processor.af.carry = operand & 1;
            operand = (operand >> 1) | (dmg->processor.af.carry ? 0x80 : 0);
            dmg->processor.af.zero = !operand;
            dmg_system_write(dmg, dmg->processor.hl.word, operand);
            break;
        case 0x0F: /* A */
            dmg->processor.af.carry = dmg->processor.af.high & 1;
            dmg->processor.af.high = (dmg->processor.af.high >> 1) | (dmg->processor.af.carry ? 0x80 : 0);
            dmg->processor.af.zero = !dmg->processor.af.high;
            break;
    }
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
}

static void dmg_processor_instruction_rrca(dmg_t const dmg)
{
    dmg->processor.delay = 4;
    dmg->processor.af.carry = ((dmg->processor.af.high & 1) == 1);
    dmg->processor.af.high = (dmg->processor.af.high >> 1) | (dmg->processor.af.carry ? 0x80 : 0);
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
    dmg->processor.af.zero = false;
}

static void dmg_processor_instruction_rst(dmg_t const dmg)
{
    dmg->processor.delay = 16;
    dmg_system_write(dmg, --dmg->processor.sp.word, dmg->processor.pc.high);
    dmg_system_write(dmg, --dmg->processor.sp.word, dmg->processor.pc.low);
    dmg->processor.pc.word = dmg->processor.instruction.opcode - 0xC7;
}

static void dmg_processor_instruction_sbc(dmg_t const dmg)
{
    uint16_t sum;
    uint8_t carry, operand = 0;
    dmg->processor.delay = 4;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x98: /* B */
            operand = dmg->processor.bc.high;
            break;
        case 0x99: /* C */
            operand = dmg->processor.bc.low;
            break;
        case 0x9A: /* D */
            operand = dmg->processor.de.high;
            break;
        case 0x9B: /* E */
            operand = dmg->processor.de.low;
            break;
        case 0x9C: /* H */
            operand = dmg->processor.hl.high;
            break;
        case 0x9D: /* L */
            operand = dmg->processor.hl.low;
            break;
        case 0x9E: /* (HL) */
            dmg->processor.delay += 4;
            operand = dmg_system_read(dmg, dmg->processor.hl.word);
            break;
        case 0x9F: /* A */
            operand = dmg->processor.af.high;
            break;
        case 0xDE: /* # */
            dmg->processor.delay += 4;
            operand = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
    }
    carry = dmg->processor.af.carry;
    sum = dmg->processor.af.high - operand - carry;
    dmg->processor.af.carry = ((int16_t)sum < 0);
    dmg->processor.af.half_carry = ((int16_t)((dmg->processor.af.high & 0x0F) - (operand & 0x0F) - carry) < 0);
    dmg->processor.af.negative = true;
    dmg->processor.af.zero = !(sum & 0xFF);
    dmg->processor.af.high = sum;
}

static void dmg_processor_instruction_scf(dmg_t const dmg)
{
    dmg->processor.delay = 4;
    dmg->processor.af.carry = true;
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
}

static void dmg_processor_instruction_set(dmg_t const dmg)
{
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0xC0: case 0xC8: case 0xD0: case 0xD8:
        case 0xE0: case 0xE8: case 0xF0: case 0xF8: /* B */
            dmg->processor.bc.high |= (1 << ((dmg->processor.instruction.opcode - 0xC0) / 8));
            break;
        case 0xC1: case 0xC9: case 0xD1: case 0xD9:
        case 0xE1: case 0xE9: case 0xF1: case 0xF9: /* C */
            dmg->processor.bc.low |= (1 << ((dmg->processor.instruction.opcode - 0xC1) / 8));
            break;
        case 0xC2: case 0xCA: case 0xD2: case 0xDA:
        case 0xE2: case 0xEA: case 0xF2: case 0xFA: /* D */
            dmg->processor.de.high |= (1 << ((dmg->processor.instruction.opcode - 0xC2) / 8));
            break;
        case 0xC3: case 0xCB: case 0xD3: case 0xDB:
        case 0xE3: case 0xEB: case 0xF3: case 0xFB: /* E */
            dmg->processor.de.low |= (1 << ((dmg->processor.instruction.opcode - 0xC3) / 8));
            break;
        case 0xC4: case 0xCC: case 0xD4: case 0xDC:
        case 0xE4: case 0xEC: case 0xF4: case 0xFC: /* H */
            dmg->processor.hl.high |= (1 << ((dmg->processor.instruction.opcode - 0xC4) / 8));
            break;
        case 0xC5: case 0xCD: case 0xD5: case 0xDD:
        case 0xE5: case 0xED: case 0xF5: case 0xFD: /* L */
            dmg->processor.hl.low |= (1 << ((dmg->processor.instruction.opcode - 0xC5) / 8));
            break;
        case 0xC6: case 0xCE: case 0xD6: case 0xDE:
        case 0xE6: case 0xEE: case 0xF6: case 0xFE: /* (HL) */
            dmg->processor.delay += 8;
            dmg_system_write(dmg, dmg->processor.hl.word, dmg_system_read(dmg, dmg->processor.hl.word)
                | (1 << ((dmg->processor.instruction.opcode - 0xC6) / 8)));
            break;
        case 0xC7: case 0xCF: case 0xD7: case 0xDF:
        case 0xE7: case 0xEF: case 0xF7: case 0xFF: /* A */
            dmg->processor.af.high |= (1 << ((dmg->processor.instruction.opcode - 0xC7) / 8));
            break;
    }
}

static void dmg_processor_instruction_sla(dmg_t const dmg)
{
    uint8_t operand = 0;
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x20: /* B */
            dmg->processor.af.carry = ((dmg->processor.bc.high & 0x80) == 0x80);
            dmg->processor.bc.high <<= 1;
            dmg->processor.af.zero = !dmg->processor.bc.high;
            break;
        case 0x21: /* C */
            dmg->processor.af.carry = ((dmg->processor.bc.low & 0x80) == 0x80);
            dmg->processor.bc.low <<= 1;
            dmg->processor.af.zero = !dmg->processor.bc.low;
            break;
        case 0x22: /* D */
            dmg->processor.af.carry = ((dmg->processor.de.high & 0x80) == 0x80);
            dmg->processor.de.high <<= 1;
            dmg->processor.af.zero = !dmg->processor.de.high;
            break;
        case 0x23: /* E */
            dmg->processor.af.carry = ((dmg->processor.de.low & 0x80) == 0x80);
            dmg->processor.de.low <<= 1;
            dmg->processor.af.zero = !dmg->processor.de.low;
            break;
        case 0x24: /* H */
            dmg->processor.af.carry = ((dmg->processor.hl.high & 0x80) == 0x80);
            dmg->processor.hl.high <<= 1;
            dmg->processor.af.zero = !dmg->processor.hl.high;
            break;
        case 0x25: /* L */
            dmg->processor.af.carry = ((dmg->processor.hl.low & 0x80) == 0x80);
            dmg->processor.hl.low <<= 1;
            dmg->processor.af.zero = !dmg->processor.hl.low;
            break;
        case 0x26: /* (HL) */
            dmg->processor.delay += 8;
            operand = dmg_system_read(dmg, dmg->processor.hl.word);
            dmg->processor.af.carry = ((operand & 0x80) == 0x80);
            operand <<= 1;
            dmg->processor.af.zero = !operand;
            dmg_system_write(dmg, dmg->processor.hl.word, operand);
            break;
        case 0x27: /* A */
            dmg->processor.af.carry = ((dmg->processor.af.high & 0x80) == 0x80);
            dmg->processor.af.high <<= 1;
            dmg->processor.af.zero = !dmg->processor.af.high;
            break;
    }
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
}

static void dmg_processor_instruction_sra(dmg_t const dmg)
{
    uint8_t operand = 0;
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x28: /* B */
            dmg->processor.af.carry = dmg->processor.bc.high & 1;
            dmg->processor.bc.high = (dmg->processor.bc.high >> 1) | (dmg->processor.bc.high & 0x80);
            dmg->processor.af.zero = !dmg->processor.bc.high;
            break;
        case 0x29: /* C */
            dmg->processor.af.carry = dmg->processor.bc.low & 1;
            dmg->processor.bc.low = (dmg->processor.bc.low >> 1) | (dmg->processor.bc.low & 0x80);
            dmg->processor.af.zero = !dmg->processor.bc.low;
            break;
        case 0x2A: /* D */
            dmg->processor.af.carry = dmg->processor.de.high & 1;
            dmg->processor.de.high = (dmg->processor.de.high >> 1) | (dmg->processor.de.high & 0x80);
            dmg->processor.af.zero = !dmg->processor.de.high;
            break;
        case 0x2B: /* E */
            dmg->processor.af.carry = dmg->processor.de.low & 1;
            dmg->processor.de.low = (dmg->processor.de.low >> 1) | (dmg->processor.de.low & 0x80);
            dmg->processor.af.zero = !dmg->processor.de.low;
            break;
        case 0x2C: /* H */
            dmg->processor.af.carry = dmg->processor.hl.high & 1;
            dmg->processor.hl.high = (dmg->processor.hl.high >> 1) | (dmg->processor.hl.high & 0x80);
            dmg->processor.af.zero = !dmg->processor.hl.high;
            break;
        case 0x2D: /* L */
            dmg->processor.af.carry = dmg->processor.hl.low & 1;
            dmg->processor.hl.low = (dmg->processor.hl.low >> 1) | (dmg->processor.hl.low & 0x80);
            dmg->processor.af.zero = !dmg->processor.hl.low;
            break;
        case 0x2E: /* (HL) */
            dmg->processor.delay += 8;
            operand = dmg_system_read(dmg, dmg->processor.hl.word);
            dmg->processor.af.carry = operand & 1;
            operand = (operand >> 1) | (operand & 0x80);
            dmg->processor.af.zero = !operand;
            dmg_system_write(dmg, dmg->processor.hl.word, operand);
            break;
        case 0x2F: /* A */
            dmg->processor.af.carry = dmg->processor.af.high & 1;
            dmg->processor.af.high = (dmg->processor.af.high >> 1) | (dmg->processor.af.high & 0x80);
            dmg->processor.af.zero = !dmg->processor.af.high;
            break;
    }
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
}

static void dmg_processor_instruction_srl(dmg_t const dmg)
{
    uint8_t operand = 0;
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x38: /* B */
            dmg->processor.af.carry = dmg->processor.bc.high & 1;
            dmg->processor.bc.high >>= 1;
            dmg->processor.af.zero = !dmg->processor.bc.high;
            break;
        case 0x39: /* C */
            dmg->processor.af.carry = dmg->processor.bc.low & 1;
            dmg->processor.bc.low >>= 1;
            dmg->processor.af.zero = !dmg->processor.bc.low;
            break;
        case 0x3A: /* D */
            dmg->processor.af.carry = dmg->processor.de.high & 1;
            dmg->processor.de.high >>= 1;
            dmg->processor.af.zero = !dmg->processor.de.high;
            break;
        case 0x3B: /* E */
            dmg->processor.af.carry = dmg->processor.de.low & 1;
            dmg->processor.de.low >>= 1;
            dmg->processor.af.zero = !dmg->processor.de.low;
            break;
        case 0x3C: /* H */
            dmg->processor.af.carry = dmg->processor.hl.high & 1;
            dmg->processor.hl.high >>= 1;
            dmg->processor.af.zero = !dmg->processor.hl.high;
            break;
        case 0x3D: /* L */
            dmg->processor.af.carry = dmg->processor.hl.low & 1;
            dmg->processor.hl.low >>= 1;
            dmg->processor.af.zero = !dmg->processor.hl.low;
            break;
        case 0x3E: /* (HL) */
            dmg->processor.delay += 8;
            operand = dmg_system_read(dmg, dmg->processor.hl.word);
            dmg->processor.af.carry = operand & 1;
            operand >>= 1;
            dmg->processor.af.zero = !operand;
            dmg_system_write(dmg, dmg->processor.hl.word, operand);
            break;
        case 0x3F: /* A */
            dmg->processor.af.carry = dmg->processor.af.high & 1;
            dmg->processor.af.high >>= 1;
            dmg->processor.af.zero = !dmg->processor.af.high;
            break;
    }
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
}

static void dmg_processor_instruction_stop(dmg_t const dmg)
{
    dmg->processor.delay = 4;
    dmg->processor.stopped = true;
    dmg_system_read(dmg, dmg->processor.pc.word++);
}

static void dmg_processor_instruction_sub(dmg_t const dmg)
{
    uint8_t operand = 0;
    uint16_t carry, sum;
    dmg->processor.delay = 4;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x90: /* B */
            operand = dmg->processor.bc.high;
            break;
        case 0x91: /* C */
            operand = dmg->processor.bc.low;
            break;
        case 0x92: /* D */
            operand = dmg->processor.de.high;
            break;
        case 0x93: /* E */
            operand = dmg->processor.de.low;
            break;
        case 0x94: /* H */
            operand = dmg->processor.hl.high;
            break;
        case 0x95: /* L */
            operand = dmg->processor.hl.low;
            break;
        case 0x96: /* (HL) */
            dmg->processor.delay += 4;
            operand = dmg_system_read(dmg, dmg->processor.hl.word);
            break;
        case 0x97: /* A */
            operand = dmg->processor.af.high;
            break;
        case 0xD6: /* # */
            dmg->processor.delay += 4;
            operand = dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
    }
    sum = dmg->processor.af.high - operand;
    carry = dmg->processor.af.high ^ operand ^ sum;
    dmg->processor.af.carry = ((carry & 0x100) == 0x100);
    dmg->processor.af.half_carry = ((carry & 0x10) == 0x10);
    dmg->processor.af.negative = true;
    dmg->processor.af.zero = !(sum & 0xFF);
    dmg->processor.af.high = sum;
}

static void dmg_processor_instruction_swap(dmg_t const dmg)
{
    uint8_t operand = 0;
    dmg->processor.delay = 8;
    switch (dmg->processor.instruction.opcode)
    {
        case 0x30: /* B */
            dmg->processor.bc.high = (dmg->processor.bc.high << 4) | (dmg->processor.bc.high >> 4);
            dmg->processor.af.zero = !dmg->processor.bc.high;
            break;
        case 0x31: /* C */
            dmg->processor.bc.low = (dmg->processor.bc.low << 4) | (dmg->processor.bc.low >> 4);
            dmg->processor.af.zero = !dmg->processor.bc.low;
            break;
        case 0x32: /* D */
            dmg->processor.de.high = (dmg->processor.de.high << 4) | (dmg->processor.de.high >> 4);
            dmg->processor.af.zero = !dmg->processor.de.high;
            break;
        case 0x33: /* E */
            dmg->processor.de.low = (dmg->processor.de.low << 4) | (dmg->processor.de.low >> 4);
            dmg->processor.af.zero = !dmg->processor.de.low;
            break;
        case 0x34: /* H */
            dmg->processor.hl.high = (dmg->processor.hl.high << 4) | (dmg->processor.hl.high >> 4);
            dmg->processor.af.zero = !dmg->processor.hl.high;
            break;
        case 0x35: /* L */
            dmg->processor.hl.low = (dmg->processor.hl.low << 4) | (dmg->processor.hl.low >> 4);
            dmg->processor.af.zero = !dmg->processor.hl.low;
            break;
        case 0x36: /* (HL) */
            dmg->processor.delay += 8;
            operand = dmg_system_read(dmg, dmg->processor.hl.word);
            operand = (operand << 4) | (operand >> 4);
            dmg->processor.af.zero = !operand;
            dmg_system_write(dmg, dmg->processor.hl.word, operand);
            break;
        case 0x37: /* A */
            dmg->processor.af.high = (dmg->processor.af.high << 4) | (dmg->processor.af.high >> 4);
            dmg->processor.af.zero = !dmg->processor.af.high;
            break;
    }
    dmg->processor.af.carry = false;
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
}

static void dmg_processor_instruction_xor(dmg_t const dmg)
{
    dmg->processor.delay = 4;
    switch (dmg->processor.instruction.opcode)
    {
        case 0xA8: /* B */
            dmg->processor.af.high ^= dmg->processor.bc.high;
            break;
        case 0xA9: /* C */
            dmg->processor.af.high ^= dmg->processor.bc.low;
            break;
        case 0xAA: /* D */
            dmg->processor.af.high ^= dmg->processor.de.high;
            break;
        case 0xAB: /* E */
            dmg->processor.af.high ^= dmg->processor.de.low;
            break;
        case 0xAC: /* H */
            dmg->processor.af.high ^= dmg->processor.hl.high;
            break;
        case 0xAD: /* L */
            dmg->processor.af.high ^= dmg->processor.hl.low;
            break;
        case 0xAE: /* (HL) */
            dmg->processor.delay += 4;
            dmg->processor.af.high ^= dmg_system_read(dmg, dmg->processor.hl.word);
            break;
        case 0xAF: /* A */
            dmg->processor.af.high = 0;
            break;
        case 0xEE: /* # */
            dmg->processor.delay += 4;
            dmg->processor.af.high ^= dmg_system_read(dmg, dmg->processor.pc.word++);
            break;
    }
    dmg->processor.af.carry = false;
    dmg->processor.af.half_carry = false;
    dmg->processor.af.negative = false;
    dmg->processor.af.zero = !dmg->processor.af.high;
}

static void dmg_processor_instruction_xxx(dmg_t const dmg)
{
#ifndef NDEBUG
    fprintf(stderr, "Invalid opcode -- [%04X] %02X", dmg->processor.instruction.address, dmg->processor.instruction.opcode);
#endif /* NDEBUG */
}

static const dmg_instruction_f INSTRUCTION[] =
{
    /* 00 */
    dmg_processor_instruction_nop, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_inc_word,
    dmg_processor_instruction_inc, dmg_processor_instruction_dec, dmg_processor_instruction_ld, dmg_processor_instruction_rlca,
    /* 08 */
    dmg_processor_instruction_ld, dmg_processor_instruction_add_hl, dmg_processor_instruction_ld, dmg_processor_instruction_dec_word,
    dmg_processor_instruction_inc, dmg_processor_instruction_dec, dmg_processor_instruction_ld, dmg_processor_instruction_rrca,
    /* 10 */
    dmg_processor_instruction_stop, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_inc_word,
    dmg_processor_instruction_inc, dmg_processor_instruction_dec, dmg_processor_instruction_ld, dmg_processor_instruction_rla,
    /* 18 */
    dmg_processor_instruction_jr, dmg_processor_instruction_add_hl, dmg_processor_instruction_ld, dmg_processor_instruction_dec_word,
    dmg_processor_instruction_inc, dmg_processor_instruction_dec, dmg_processor_instruction_ld, dmg_processor_instruction_rra,
    /* 20 */
    dmg_processor_instruction_jr, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_inc_word,
    dmg_processor_instruction_inc, dmg_processor_instruction_dec, dmg_processor_instruction_ld, dmg_processor_instruction_daa,
    /* 28 */
    dmg_processor_instruction_jr, dmg_processor_instruction_add_hl, dmg_processor_instruction_ld, dmg_processor_instruction_dec_word,
    dmg_processor_instruction_inc, dmg_processor_instruction_dec, dmg_processor_instruction_ld, dmg_processor_instruction_cpl,
    /* 30 */
    dmg_processor_instruction_jr, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_inc_word,
    dmg_processor_instruction_inc, dmg_processor_instruction_dec, dmg_processor_instruction_ld, dmg_processor_instruction_scf,
    /* 38 */
    dmg_processor_instruction_jr, dmg_processor_instruction_add_hl, dmg_processor_instruction_ld, dmg_processor_instruction_dec_word,
    dmg_processor_instruction_inc, dmg_processor_instruction_dec, dmg_processor_instruction_ld, dmg_processor_instruction_ccf,
    /* 40 */
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld,
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld,
    /* 48 */
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld,
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld,
    /* 50 */
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld,
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld,
    /* 58 */
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld,
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld,
    /* 60 */
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld,
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld,
    /* 68 */
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld,
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld,
    /* 70 */
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld,
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_halt, dmg_processor_instruction_ld,
    /* 78 */
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld,
    dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ld,
    /* 80 */
    dmg_processor_instruction_add, dmg_processor_instruction_add, dmg_processor_instruction_add, dmg_processor_instruction_add,
    dmg_processor_instruction_add, dmg_processor_instruction_add, dmg_processor_instruction_add, dmg_processor_instruction_add,
    /* 88 */
    dmg_processor_instruction_adc, dmg_processor_instruction_adc, dmg_processor_instruction_adc, dmg_processor_instruction_adc,
    dmg_processor_instruction_adc, dmg_processor_instruction_adc, dmg_processor_instruction_adc, dmg_processor_instruction_adc,
    /* 90 */
    dmg_processor_instruction_sub, dmg_processor_instruction_sub, dmg_processor_instruction_sub, dmg_processor_instruction_sub,
    dmg_processor_instruction_sub, dmg_processor_instruction_sub, dmg_processor_instruction_sub, dmg_processor_instruction_sub,
    /* 98 */
    dmg_processor_instruction_sbc, dmg_processor_instruction_sbc, dmg_processor_instruction_sbc, dmg_processor_instruction_sbc,
    dmg_processor_instruction_sbc, dmg_processor_instruction_sbc, dmg_processor_instruction_sbc, dmg_processor_instruction_sbc,
    /* A0 */
    dmg_processor_instruction_and, dmg_processor_instruction_and, dmg_processor_instruction_and, dmg_processor_instruction_and,
    dmg_processor_instruction_and, dmg_processor_instruction_and, dmg_processor_instruction_and, dmg_processor_instruction_and,
    /* A8 */
    dmg_processor_instruction_xor, dmg_processor_instruction_xor, dmg_processor_instruction_xor, dmg_processor_instruction_xor,
    dmg_processor_instruction_xor, dmg_processor_instruction_xor, dmg_processor_instruction_xor, dmg_processor_instruction_xor,
    /* B0 */
    dmg_processor_instruction_or, dmg_processor_instruction_or, dmg_processor_instruction_or, dmg_processor_instruction_or,
    dmg_processor_instruction_or, dmg_processor_instruction_or, dmg_processor_instruction_or, dmg_processor_instruction_or,
    /* B8 */
    dmg_processor_instruction_cp, dmg_processor_instruction_cp, dmg_processor_instruction_cp, dmg_processor_instruction_cp,
    dmg_processor_instruction_cp, dmg_processor_instruction_cp, dmg_processor_instruction_cp, dmg_processor_instruction_cp,
    /* C0 */
    dmg_processor_instruction_ret, dmg_processor_instruction_pop, dmg_processor_instruction_jp, dmg_processor_instruction_jp,
    dmg_processor_instruction_call, dmg_processor_instruction_push, dmg_processor_instruction_add, dmg_processor_instruction_rst,
    /* C8 */
    dmg_processor_instruction_ret, dmg_processor_instruction_ret, dmg_processor_instruction_jp, dmg_processor_instruction_xxx,
    dmg_processor_instruction_call, dmg_processor_instruction_call, dmg_processor_instruction_adc, dmg_processor_instruction_rst,
    /* D0 */
    dmg_processor_instruction_ret, dmg_processor_instruction_pop, dmg_processor_instruction_jp, dmg_processor_instruction_xxx,
    dmg_processor_instruction_call, dmg_processor_instruction_push, dmg_processor_instruction_sub, dmg_processor_instruction_rst,
    /* D8 */
    dmg_processor_instruction_ret, dmg_processor_instruction_reti, dmg_processor_instruction_jp, dmg_processor_instruction_xxx,
    dmg_processor_instruction_call, dmg_processor_instruction_xxx, dmg_processor_instruction_sbc, dmg_processor_instruction_rst,
    /* E0 */
    dmg_processor_instruction_ld, dmg_processor_instruction_pop, dmg_processor_instruction_ld, dmg_processor_instruction_xxx,
    dmg_processor_instruction_xxx, dmg_processor_instruction_push, dmg_processor_instruction_and, dmg_processor_instruction_rst,
    /* E8 */
    dmg_processor_instruction_add_sp, dmg_processor_instruction_jp_hl, dmg_processor_instruction_ld, dmg_processor_instruction_xxx,
    dmg_processor_instruction_xxx, dmg_processor_instruction_xxx, dmg_processor_instruction_xor, dmg_processor_instruction_rst,
    /* F0 */
    dmg_processor_instruction_ld, dmg_processor_instruction_pop, dmg_processor_instruction_ld, dmg_processor_instruction_di,
    dmg_processor_instruction_xxx, dmg_processor_instruction_push, dmg_processor_instruction_or, dmg_processor_instruction_rst,
    /* F8 */
    dmg_processor_instruction_ld_hl, dmg_processor_instruction_ld, dmg_processor_instruction_ld, dmg_processor_instruction_ei,
    dmg_processor_instruction_xxx, dmg_processor_instruction_xxx, dmg_processor_instruction_cp, dmg_processor_instruction_rst,
    /* CB 00 */
    dmg_processor_instruction_rlc, dmg_processor_instruction_rlc, dmg_processor_instruction_rlc, dmg_processor_instruction_rlc,
    dmg_processor_instruction_rlc, dmg_processor_instruction_rlc, dmg_processor_instruction_rlc, dmg_processor_instruction_rlc,
    /* CB 08 */
    dmg_processor_instruction_rrc, dmg_processor_instruction_rrc, dmg_processor_instruction_rrc, dmg_processor_instruction_rrc,
    dmg_processor_instruction_rrc, dmg_processor_instruction_rrc, dmg_processor_instruction_rrc, dmg_processor_instruction_rrc,
    /* CB 10 */
    dmg_processor_instruction_rl, dmg_processor_instruction_rl, dmg_processor_instruction_rl, dmg_processor_instruction_rl,
    dmg_processor_instruction_rl, dmg_processor_instruction_rl, dmg_processor_instruction_rl, dmg_processor_instruction_rl,
    /* CB 18 */
    dmg_processor_instruction_rr, dmg_processor_instruction_rr, dmg_processor_instruction_rr, dmg_processor_instruction_rr,
    dmg_processor_instruction_rr, dmg_processor_instruction_rr, dmg_processor_instruction_rr, dmg_processor_instruction_rr,
    /* CB 20 */
    dmg_processor_instruction_sla, dmg_processor_instruction_sla, dmg_processor_instruction_sla, dmg_processor_instruction_sla,
    dmg_processor_instruction_sla, dmg_processor_instruction_sla, dmg_processor_instruction_sla, dmg_processor_instruction_sla,
    /* CB 28 */
    dmg_processor_instruction_sra, dmg_processor_instruction_sra, dmg_processor_instruction_sra, dmg_processor_instruction_sra,
    dmg_processor_instruction_sra, dmg_processor_instruction_sra, dmg_processor_instruction_sra, dmg_processor_instruction_sra,
    /* CB 30 */
    dmg_processor_instruction_swap, dmg_processor_instruction_swap, dmg_processor_instruction_swap, dmg_processor_instruction_swap,
    dmg_processor_instruction_swap, dmg_processor_instruction_swap, dmg_processor_instruction_swap, dmg_processor_instruction_swap,
    /* CB 38 */
    dmg_processor_instruction_srl, dmg_processor_instruction_srl, dmg_processor_instruction_srl, dmg_processor_instruction_srl,
    dmg_processor_instruction_srl, dmg_processor_instruction_srl, dmg_processor_instruction_srl, dmg_processor_instruction_srl,
    /* CB 40 */
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    /* CB 48 */
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    /* CB 50 */
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    /* CB 58 */
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    /* CB 60 */
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    /* CB 68 */
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    /* CB 70 */
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    /* CB 78 */
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit, dmg_processor_instruction_bit,
    /* CB 80 */
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    /* CB 88 */
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    /* CB 90 */
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    /* CB 98 */
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    /* CB A0 */
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    /* CB A8 */
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    /* CB B0 */
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    /* CB B8 */
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res, dmg_processor_instruction_res,
    /* CB C0 */
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    /* CB C8 */
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    /* CB D0 */
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    /* CB D8 */
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    /* CB E0 */
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    /* CB E8 */
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    /* CB F0 */
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    /* CB F8 */
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set, dmg_processor_instruction_set,
    };

static void dmg_processor_execute(dmg_t const dmg)
{
    bool extended;
    dmg->processor.instruction.address = dmg->processor.pc.word;
    dmg->processor.instruction.opcode = dmg_system_read(dmg, dmg->processor.pc.word++);
    if ((extended = (dmg->processor.instruction.opcode == 0xCB)))
    {
        dmg->processor.instruction.opcode = dmg_system_read(dmg, dmg->processor.pc.word++);
    }
    INSTRUCTION[extended ? dmg->processor.instruction.opcode + 256 : dmg->processor.instruction.opcode](dmg);
}

static void dmg_processor_service(dmg_t const dmg)
{
    dmg_interrupt_e interrupt;
    for (interrupt = 0; interrupt < DMG_INTERRUPT_MAX; ++interrupt)
    {
        uint8_t mask = 1 << interrupt;
        if (dmg->processor.interrupt.enable & dmg->processor.interrupt.flag & mask)
        {
            dmg_system_write(dmg, --dmg->processor.sp.word, dmg->processor.pc.high);
            dmg_system_write(dmg, --dmg->processor.sp.word, dmg->processor.pc.low);
            dmg->processor.pc.word = (0x0008 * interrupt) + 0x0040;
            dmg->processor.interrupt.enabled = false;
            dmg->processor.interrupt.flag &= ~mask;
            dmg->processor.delay = 20;
            break;
        }
    }
}

void dmg_processor_clock(dmg_t const dmg)
{
    if (!dmg->processor.delay)
    {
        if (dmg->processor.interrupt.enable_delay && !--dmg->processor.interrupt.enable_delay)
        {
            dmg->processor.interrupt.enabled = true;
        }
        if (dmg->processor.interrupt.enable & dmg->processor.interrupt.flag & 0x1F)
        {
            dmg->processor.halted = false;
            if (dmg->processor.interrupt.enabled)
            {
                dmg_processor_service(dmg);
            }
            else if (!dmg->processor.halted && !dmg->processor.stopped)
            {
                dmg_processor_execute(dmg);
            }
            else
            {
                dmg->processor.delay = 4;
            }
        }
        else if (!dmg->processor.halted && !dmg->processor.stopped)
        {
            dmg_processor_execute(dmg);
        }
        else
        {
            dmg->processor.delay = 4;
        }
    }
    --dmg->processor.delay;
}

void dmg_processor_interrupt(dmg_t const dmg, dmg_interrupt_e interrupt)
{
    dmg_processor_write(dmg, 0xFF0F, dmg->processor.interrupt.flag | (1 << interrupt));
}

uint8_t dmg_processor_read(dmg_t const dmg, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0xFF0F: /* IF */
            result = dmg->processor.interrupt.flag;
            break;
        case 0xFFFF: /* IE */
            result = dmg->processor.interrupt.enable;
            break;
        default:
            break;
    }
    return result;
}

void dmg_processor_write(dmg_t const dmg, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0xFF0F: /* IF */
            dmg->processor.interrupt.flag = 0xE0 | value;
            if (dmg->processor.interrupt.flag & (1 << DMG_INTERRUPT_INPUT))
            {
                dmg->processor.stopped = false;
            }
            break;
        case 0xFFFF: /* IE */
            dmg->processor.interrupt.enable = value;
            break;
        default:
            break;
    }
}
