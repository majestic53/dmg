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

typedef dmg_error_e (*dmg_processor_f)(dmg_handle_t const handle);

static dmg_error_e dmg_processor_adc(dmg_handle_t const handle)
{
    uint16_t sum;
    uint8_t carry, operand = 0;
    handle->processor.delay = 4;
    switch (handle->processor.instruction.opcode)
    {
        case 0x88: /* B */
            operand = handle->processor.bc.high;
            break;
        case 0x89: /* C */
            operand = handle->processor.bc.low;
            break;
        case 0x8A: /* D */
            operand = handle->processor.de.high;
            break;
        case 0x8B: /* E */
            operand = handle->processor.de.low;
            break;
        case 0x8C: /* H */
            operand = handle->processor.hl.high;
            break;
        case 0x8D: /* L */
            operand = handle->processor.hl.low;
            break;
        case 0x8E: /* (HL) */
            handle->processor.delay += 4;
            operand = dmg_read(handle, handle->processor.hl.word);
            break;
        case 0x8F: /* A */
            operand = handle->processor.af.high;
            break;
        case 0xCE: /* # */
            handle->processor.delay += 4;
            operand = dmg_read(handle, handle->processor.pc.word++);
            break;
    }
    carry = handle->processor.af.carry;
    sum = handle->processor.af.high + operand + carry;
    handle->processor.af.carry = (sum > 0xFF);
    handle->processor.af.half_carry = (((handle->processor.af.high & 0x0F) + (operand & 0x0F) + carry) > 0x0F);
    handle->processor.af.negative = false;
    handle->processor.af.zero = !(sum & 0xFF);
    handle->processor.af.high = sum;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_add(dmg_handle_t const handle)
{
    uint8_t operand = 0;
    uint16_t carry, sum;
    handle->processor.delay = 4;
    switch (handle->processor.instruction.opcode)
    {
        case 0x80: /* B */
            operand = handle->processor.bc.high;
            break;
        case 0x81: /* C */
            operand = handle->processor.bc.low;
            break;
        case 0x82: /* D */
            operand = handle->processor.de.high;
            break;
        case 0x83: /* E */
            operand = handle->processor.de.low;
            break;
        case 0x84: /* H */
            operand = handle->processor.hl.high;
            break;
        case 0x85: /* L */
            operand = handle->processor.hl.low;
            break;
        case 0x86: /* (HL) */
            handle->processor.delay += 4;
            operand = dmg_read(handle, handle->processor.hl.word);
            break;
        case 0x87: /* A */
            operand = handle->processor.af.high;
            break;
        case 0xC6: /* # */
            handle->processor.delay += 4;
            operand = dmg_read(handle, handle->processor.pc.word++);
            break;
    }
    sum = handle->processor.af.high + operand;
    carry = handle->processor.af.high ^ operand ^ sum;
    handle->processor.af.carry = ((carry & 0x100) == 0x100);
    handle->processor.af.half_carry = ((carry & 0x10) == 0x10);
    handle->processor.af.negative = false;
    handle->processor.af.zero = !(sum & 0xFF);
    handle->processor.af.high = sum;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_add_hl(dmg_handle_t const handle)
{
    uint32_t carry, sum;
    uint16_t operand = 0;
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0x09: /* BC */
            operand = handle->processor.bc.word;
            break;
        case 0x19: /* DE */
            operand = handle->processor.de.word;
            break;
        case 0x29: /* HL */
            operand = handle->processor.hl.word;
            break;
        case 0x39: /* SP */
            operand = handle->processor.sp.word;
            break;
    }
    sum = handle->processor.hl.word + operand;
    carry = handle->processor.hl.word ^ operand ^ sum;
    handle->processor.af.carry = ((carry & 0x10000) == 0x10000);
    handle->processor.af.half_carry = ((carry & 0x1000) == 0x1000);
    handle->processor.af.negative = false;
    handle->processor.hl.word = sum;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_add_sp(dmg_handle_t const handle)
{
    uint32_t carry, sum;
    int8_t operand = dmg_read(handle, handle->processor.pc.word++);
    handle->processor.delay = 16;
    sum = handle->processor.sp.word + operand;
    carry = handle->processor.sp.word ^ operand ^ sum;
    handle->processor.af.carry = ((carry & 0x100) == 0x100);
    handle->processor.af.half_carry = ((carry & 0x10) == 0x10);
    handle->processor.af.negative = false;
    handle->processor.af.zero = false;
    handle->processor.sp.word = sum;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_and(dmg_handle_t const handle)
{
    handle->processor.delay = 4;
    switch (handle->processor.instruction.opcode)
    {
        case 0xA0: /* B */
            handle->processor.af.high &= handle->processor.bc.high;
            break;
        case 0xA1: /* C */
            handle->processor.af.high &= handle->processor.bc.low;
            break;
        case 0xA2: /* D */
            handle->processor.af.high &= handle->processor.de.high;
            break;
        case 0xA3: /* E */
            handle->processor.af.high &= handle->processor.de.low;
            break;
        case 0xA4: /* H */
            handle->processor.af.high &= handle->processor.hl.high;
            break;
        case 0xA5: /* L */
            handle->processor.af.high &= handle->processor.hl.low;
            break;
        case 0xA6: /* (HL) */
            handle->processor.delay += 4;
            handle->processor.af.high &= dmg_read(handle, handle->processor.hl.word);
            break;
        case 0xA7: /* A */
            break;
        case 0xE6: /* # */
            handle->processor.delay += 4;
            handle->processor.af.high &= dmg_read(handle, handle->processor.pc.word++);
            break;
    }
    handle->processor.af.carry = false;
    handle->processor.af.half_carry = true;
    handle->processor.af.negative = false;
    handle->processor.af.zero = !handle->processor.af.high;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_bit(dmg_handle_t const handle)
{
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0x40: case 0x48: case 0x50: case 0x58:
        case 0x60: case 0x68: case 0x70: case 0x78: /* B */
            handle->processor.af.zero = !(handle->processor.bc.high & (1 << ((handle->processor.instruction.opcode - 0x40) / 8)));
            break;
        case 0x41: case 0x49: case 0x51: case 0x59:
        case 0x61: case 0x69: case 0x71: case 0x79: /* C */
            handle->processor.af.zero = !(handle->processor.bc.low & (1 << ((handle->processor.instruction.opcode - 0x41) / 8)));
            break;
        case 0x42: case 0x4A: case 0x52: case 0x5A:
        case 0x62: case 0x6A: case 0x72: case 0x7A: /* D */
            handle->processor.af.zero = !(handle->processor.de.high & (1 << ((handle->processor.instruction.opcode - 0x42) / 8)));
            break;
        case 0x43: case 0x4B: case 0x53: case 0x5B:
        case 0x63: case 0x6B: case 0x73: case 0x7B: /* E */
            handle->processor.af.zero = !(handle->processor.de.low & (1 << ((handle->processor.instruction.opcode - 0x43) / 8)));
            break;
        case 0x44: case 0x4C: case 0x54: case 0x5C:
        case 0x64: case 0x6C: case 0x74: case 0x7C: /* H */
            handle->processor.af.zero = !(handle->processor.hl.high & (1 << ((handle->processor.instruction.opcode - 0x44) / 8)));
            break;
        case 0x45: case 0x4D: case 0x55: case 0x5D:
        case 0x65: case 0x6D: case 0x75: case 0x7D: /* L */
            handle->processor.af.zero = !(handle->processor.hl.low & (1 << ((handle->processor.instruction.opcode - 0x45) / 8)));
            break;
        case 0x46: case 0x4E: case 0x56: case 0x5E:
        case 0x66: case 0x6E: case 0x76: case 0x7E: /* (HL) */
            handle->processor.delay += 4;
            handle->processor.af.zero = !(dmg_read(handle, handle->processor.hl.word) & (1 << ((handle->processor.instruction.opcode - 0x46) / 8)));
            break;
        case 0x47: case 0x4F: case 0x57: case 0x5F:
        case 0x67: case 0x6F: case 0x77: case 0x7F: /* A */
            handle->processor.af.zero = !(handle->processor.af.high & (1 << ((handle->processor.instruction.opcode - 0x47) / 8)));
            break;
    }
    handle->processor.af.half_carry = true;
    handle->processor.af.negative = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_call(dmg_handle_t const handle)
{
    bool taken = false;
    dmg_register_t operand = {};
    handle->processor.delay = 12;
    operand.low = dmg_read(handle, handle->processor.pc.word++);
    operand.high = dmg_read(handle, handle->processor.pc.word++);
    switch (handle->processor.instruction.opcode)
    {
        case 0xC4: /* NZ */
            taken = !handle->processor.af.zero;
            break;
        case 0xCC: /* Z */
            taken = handle->processor.af.zero;
            break;
        case 0xCD: /* N */
            taken = true;
            break;
        case 0xD4: /* NC */
            taken = !handle->processor.af.carry;
            break;
        case 0xDC: /* C */
            taken = handle->processor.af.carry;
            break;
    }
    if (taken)
    {
        handle->processor.delay += 12;
        dmg_write(handle, --handle->processor.sp.word, handle->processor.pc.high);
        dmg_write(handle, --handle->processor.sp.word, handle->processor.pc.low);
        handle->processor.pc.word = operand.word;
    }
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_ccf(dmg_handle_t const handle)
{
    handle->processor.delay = 4;
    handle->processor.af.carry = !handle->processor.af.carry;
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_cp(dmg_handle_t const handle)
{
    uint8_t operand = 0;
    handle->processor.delay = 4;
    switch (handle->processor.instruction.opcode)
    {
        case 0xB8: /* B */
            operand = handle->processor.bc.high;
            break;
        case 0xB9: /* C */
            operand = handle->processor.bc.low;
            break;
        case 0xBA: /* D */
            operand = handle->processor.de.high;
            break;
        case 0xBB: /* E */
            operand = handle->processor.de.low;
            break;
        case 0xBC: /* H */
            operand = handle->processor.hl.high;
            break;
        case 0xBD: /* L */
            operand = handle->processor.hl.low;
            break;
        case 0xBE: /* (HL) */
            handle->processor.delay += 4;
            operand = dmg_read(handle, handle->processor.hl.word);
            break;
        case 0xBF: /* A */
            operand = handle->processor.af.high;
            break;
        case 0xFE: /* # */
            handle->processor.delay += 4;
            operand = dmg_read(handle, handle->processor.pc.word++);
            break;
    }
    handle->processor.af.carry = (handle->processor.af.high < operand);
    handle->processor.af.half_carry = ((handle->processor.af.high & 0x0F) < ((handle->processor.af.high - operand) & 0x0F));
    handle->processor.af.negative = true;
    handle->processor.af.zero = (handle->processor.af.high == operand);
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_cpl(dmg_handle_t const handle)
{
    handle->processor.delay = 4;
    handle->processor.af.high = ~handle->processor.af.high;
    handle->processor.af.half_carry = true;
    handle->processor.af.negative = true;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_daa(dmg_handle_t const handle)
{
    handle->processor.delay = 4;
    if (!handle->processor.af.negative)
    {
        if (handle->processor.af.carry || (handle->processor.af.high > 0x99))
        {
            handle->processor.af.high += 0x60;
            handle->processor.af.carry = true;
        }
        if (handle->processor.af.half_carry || ((handle->processor.af.high & 0x0F) > 0x09))
        {
            handle->processor.af.high += 0x06;
        }
    }
    else
    {
        if (handle->processor.af.carry)
        {
            handle->processor.af.high -= 0x60;
        }
        if (handle->processor.af.half_carry)
        {
            handle->processor.af.high -= 0x06;
        }
    }
    handle->processor.af.half_carry = false;
    handle->processor.af.zero = !handle->processor.af.high;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_di(dmg_handle_t const handle)
{
    handle->processor.delay = 4;
    handle->processor.interrupt.enable_delay = 0;
    handle->processor.interrupt.enabled = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_ei(dmg_handle_t const handle)
{
    handle->processor.delay = 4;
    if (!handle->processor.interrupt.enable_delay)
    {
        handle->processor.interrupt.enable_delay = 2;
    }
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_dec(dmg_handle_t const handle)
{
    uint8_t operand = 0;
    handle->processor.delay = 4;
    switch (handle->processor.instruction.opcode)
    {
        case 0x05: /* B */
            operand = --handle->processor.bc.high;
            break;
        case 0x0D: /* C */
            operand = --handle->processor.bc.low;
            break;
        case 0x15: /* D */
            operand = --handle->processor.de.high;
            break;
        case 0x1D: /* E */
            operand = --handle->processor.de.low;
            break;
        case 0x25: /* H */
            operand = --handle->processor.hl.high;
            break;
        case 0x2D: /* L */
            operand = --handle->processor.hl.low;
            break;
        case 0x35: /* (HL) */
            handle->processor.delay += 8;
            operand = dmg_read(handle, handle->processor.hl.word) - 1;
            dmg_write(handle, handle->processor.hl.word, operand);
            break;
        case 0x3D: /* A */
            operand = --handle->processor.af.high;
            break;
    }
    handle->processor.af.half_carry = ((operand & 0x0F) == 0x0F);
    handle->processor.af.negative = true;
    handle->processor.af.zero = !operand;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_dec_word(dmg_handle_t const handle)
{
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0x0B: /* BC */
            --handle->processor.bc.word;
            break;
        case 0x1B: /* DE */
            --handle->processor.de.word;
            break;
        case 0x2B: /* HL */
            --handle->processor.hl.word;
            break;
        case 0x3B: /* SP */
            --handle->processor.sp.word;
            break;
    }
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_halt(dmg_handle_t const handle)
{
    handle->processor.delay = 4;
    handle->processor.halted = true;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_inc(dmg_handle_t const handle)
{
    uint8_t operand = 0;
    handle->processor.delay = 4;
    switch (handle->processor.instruction.opcode)
    {
        case 0x04: /* B */
            operand = ++handle->processor.bc.high;
            break;
        case 0x0C: /* C */
            operand = ++handle->processor.bc.low;
            break;
        case 0x14: /* D */
            operand = ++handle->processor.de.high;
            break;
        case 0x1C: /* E */
            operand = ++handle->processor.de.low;
            break;
        case 0x24: /* H */
            operand = ++handle->processor.hl.high;
            break;
        case 0x2C: /* L */
            operand = ++handle->processor.hl.low;
            break;
        case 0x34: /* (HL) */
            handle->processor.delay += 8;
            operand = dmg_read(handle, handle->processor.hl.word) + 1;
            dmg_write(handle, handle->processor.hl.word, operand);
            break;
        case 0x3C: /* A */
            operand = ++handle->processor.af.high;
            break;
    }
    handle->processor.af.half_carry = !(operand & 0x0F);
    handle->processor.af.negative = false;
    handle->processor.af.zero = !operand;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_inc_word(dmg_handle_t const handle)
{
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0x03: /* BC */
            ++handle->processor.bc.word;
            break;
        case 0x13: /* DE */
            ++handle->processor.de.word;
            break;
        case 0x23: /* HL */
            ++handle->processor.hl.word;
            break;
        case 0x33: /* SP */
            ++handle->processor.sp.word;
            break;
    }
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_jp(dmg_handle_t const handle)
{
    bool taken = false;
    dmg_register_t operand = {};
    handle->processor.delay = 12;
    operand.low = dmg_read(handle, handle->processor.pc.word++);
    operand.high = dmg_read(handle, handle->processor.pc.word++);
    switch (handle->processor.instruction.opcode)
    {
        case 0xC2: /* NZ */
            taken = !handle->processor.af.zero;
            break;
        case 0xC3: /* N */
            taken = true;
            break;
        case 0xCA: /* Z */
            taken = handle->processor.af.zero;
            break;
        case 0xD2: /* NC */
            taken = !handle->processor.af.carry;
            break;
        case 0xDA: /* C */
            taken = handle->processor.af.carry;
            break;
    }
    if (taken)
    {
        handle->processor.delay += 4;
        handle->processor.pc.word = operand.word;
    }
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_jp_hl(dmg_handle_t const handle)
{
    handle->processor.delay = 4;
    handle->processor.pc.word = handle->processor.hl.word;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_jr(dmg_handle_t const handle)
{
    bool taken = false;
    int8_t operand = dmg_read(handle, handle->processor.pc.word++);
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0x18: /* N */
            taken = true;
            break;
        case 0x20: /* NZ */
            taken = !handle->processor.af.zero;
            break;
        case 0x28: /* Z */
            taken = handle->processor.af.zero;
            break;
        case 0x30: /* NC */
            taken = !handle->processor.af.carry;
            break;
        case 0x38: /* C */
            taken = handle->processor.af.carry;
            break;
    }
    if (taken)
    {
        handle->processor.delay += 4;
        handle->processor.pc.word += operand;
    }
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_ld(dmg_handle_t const handle)
{
    dmg_register_t operand = {};
    handle->processor.delay = 4;
    switch (handle->processor.instruction.opcode)
    {
        case 0x01: /* BC,## */
            handle->processor.delay += 8;
            handle->processor.bc.low = dmg_read(handle, handle->processor.pc.word++);
            handle->processor.bc.high = dmg_read(handle, handle->processor.pc.word++);
            break;
        case 0x02: /* (BC),A */
            handle->processor.delay += 4;
            dmg_write(handle, handle->processor.bc.word, handle->processor.af.high);
            break;
        case 0x06: /* B,# */
            handle->processor.delay += 4;
            handle->processor.bc.high = dmg_read(handle, handle->processor.pc.word++);
            break;
        case 0x08: /* (##),SP */
            handle->processor.delay += 16;
            operand.low = dmg_read(handle, handle->processor.pc.word++);
            operand.high = dmg_read(handle, handle->processor.pc.word++);
            dmg_write(handle, operand.word, handle->processor.sp.low);
            dmg_write(handle, operand.word + 1, handle->processor.sp.high);
            break;
        case 0x0A: /* A,(BC) */
            handle->processor.delay += 4;
            handle->processor.af.high = dmg_read(handle, handle->processor.bc.word);
            break;
        case 0x0E: /* C,# */
            handle->processor.delay += 4;
            handle->processor.bc.low = dmg_read(handle, handle->processor.pc.word++);
            break;
        case 0x11: /* DE,## */
            handle->processor.delay += 8;
            handle->processor.de.low = dmg_read(handle, handle->processor.pc.word++);
            handle->processor.de.high = dmg_read(handle, handle->processor.pc.word++);
            break;
        case 0x12: /* (DE),A */
            handle->processor.delay += 4;
            dmg_write(handle, handle->processor.de.word, handle->processor.af.high);
            break;
        case 0x16: /* D,# */
            handle->processor.delay += 4;
            handle->processor.de.high = dmg_read(handle, handle->processor.pc.word++);
            break;
        case 0x1A: /* A,(DE) */
            handle->processor.delay += 4;
            handle->processor.af.high = dmg_read(handle, handle->processor.de.word);
            break;
        case 0x1E: /* E,# */
            handle->processor.delay += 4;
            handle->processor.de.low = dmg_read(handle, handle->processor.pc.word++);
            break;
        case 0x21: /* HL,## */
            handle->processor.delay += 8;
            handle->processor.hl.low = dmg_read(handle, handle->processor.pc.word++);
            handle->processor.hl.high = dmg_read(handle, handle->processor.pc.word++);
            break;
        case 0x22: /* (HL+),A */
            handle->processor.delay += 4;
            dmg_write(handle, handle->processor.hl.word++, handle->processor.af.high);
            break;
        case 0x26: /* H,# */
            handle->processor.delay += 4;
            handle->processor.hl.high = dmg_read(handle, handle->processor.pc.word++);
            break;
        case 0x2A: /* A,(HL+) */
            handle->processor.delay += 4;
            handle->processor.af.high = dmg_read(handle, handle->processor.hl.word++);
            break;
        case 0x2E: /* L,# */
            handle->processor.delay += 4;
            handle->processor.hl.low = dmg_read(handle, handle->processor.pc.word++);
            break;
        case 0x31: /* SP,## */
            handle->processor.delay += 8;
            handle->processor.sp.low = dmg_read(handle, handle->processor.pc.word++);
            handle->processor.sp.high = dmg_read(handle, handle->processor.pc.word++);
            break;
        case 0x32: /* (HL-),A */
            handle->processor.delay += 4;
            dmg_write(handle, handle->processor.hl.word--, handle->processor.af.high);
            break;
        case 0x36: /* (HL),# */
            handle->processor.delay += 8;
            dmg_write(handle, handle->processor.hl.word, dmg_read(handle, handle->processor.pc.word++));
            break;
        case 0x3A: /* A,(HL-) */
            handle->processor.delay += 4;
            handle->processor.af.high = dmg_read(handle, handle->processor.hl.word--);
            break;
        case 0x3E: /* A,# */
            handle->processor.delay += 4;
            handle->processor.af.high = dmg_read(handle, handle->processor.pc.word++);
            break;
        case 0x40: /* B,B */
            break;
        case 0x41: /* B,C */
            handle->processor.bc.high = handle->processor.bc.low;
            break;
        case 0x42: /* B,D */
            handle->processor.bc.high = handle->processor.de.high;
            break;
        case 0x43: /* B,E */
            handle->processor.bc.high = handle->processor.de.low;
            break;
        case 0x44: /* B,H */
            handle->processor.bc.high = handle->processor.hl.high;
            break;
        case 0x45: /* B,L */
            handle->processor.bc.high = handle->processor.hl.low;
            break;
        case 0x46: /* B,(HL) */
            handle->processor.delay += 4;
            handle->processor.bc.high = dmg_read(handle, handle->processor.hl.word);
            break;
        case 0x47: /* B,A */
            handle->processor.bc.high = handle->processor.af.high;
            break;
        case 0x48: /* C,B */
            handle->processor.bc.low = handle->processor.bc.high;
            break;
        case 0x49: /* C,C */
            break;
        case 0x4A: /* C,D */
            handle->processor.bc.low = handle->processor.de.high;
            break;
        case 0x4B: /* C,E */
            handle->processor.bc.low = handle->processor.de.low;
            break;
        case 0x4C: /* C,H */
            handle->processor.bc.low = handle->processor.hl.high;
            break;
        case 0x4D: /* C,L */
            handle->processor.bc.low = handle->processor.hl.low;
            break;
        case 0x4E: /* C,(HL) */
            handle->processor.delay += 4;
            handle->processor.bc.low = dmg_read(handle, handle->processor.hl.word);
            break;
        case 0x4F: /* C,A */
            handle->processor.bc.low = handle->processor.af.high;
            break;
        case 0x50: /* D,B */
            handle->processor.de.high = handle->processor.bc.high;
            break;
        case 0x51: /* D,C */
            handle->processor.de.high = handle->processor.bc.low;
            break;
        case 0x52: /* D,D */
            break;
        case 0x53: /* D,E */
            handle->processor.de.high = handle->processor.de.low;
            break;
        case 0x54: /* D,H */
            handle->processor.de.high = handle->processor.hl.high;
            break;
        case 0x55: /* D,L */
            handle->processor.de.high = handle->processor.hl.low;
            break;
        case 0x56: /* D,(HL) */
            handle->processor.delay += 4;
            handle->processor.de.high = dmg_read(handle, handle->processor.hl.word);
            break;
        case 0x57: /* D,A */
            handle->processor.de.high = handle->processor.af.high;
            break;
        case 0x58: /* E,B */
            handle->processor.de.low = handle->processor.bc.high;
            break;
        case 0x59: /* E,C */
            handle->processor.de.low = handle->processor.bc.low;
            break;
        case 0x5A: /* E,D */
            handle->processor.de.low = handle->processor.de.high;
            break;
        case 0x5B: /* E,E */
            break;
        case 0x5C: /* E,H */
            handle->processor.de.low = handle->processor.hl.high;
            break;
        case 0x5D: /* E,L */
            handle->processor.de.low = handle->processor.hl.low;
            break;
        case 0x5E: /* E,(HL) */
            handle->processor.delay += 4;
            handle->processor.de.low = dmg_read(handle, handle->processor.hl.word);
            break;
        case 0x5F: /* E,A */
            handle->processor.de.low = handle->processor.af.high;
            break;
        case 0x60: /* H,B */
            handle->processor.hl.high = handle->processor.bc.high;
            break;
        case 0x61: /* H,C */
            handle->processor.hl.high = handle->processor.bc.low;
            break;
        case 0x62: /* H,D */
            handle->processor.hl.high = handle->processor.de.high;
            break;
        case 0x63: /* H,E */
            handle->processor.hl.high = handle->processor.de.low;
            break;
        case 0x64: /* H,H */
            break;
        case 0x65: /* H,L */
            handle->processor.hl.high = handle->processor.hl.low;
            break;
        case 0x66: /* H,(HL) */
            handle->processor.delay += 4;
            handle->processor.hl.high = dmg_read(handle, handle->processor.hl.word);
            break;
        case 0x67: /* H,A */
            handle->processor.hl.high = handle->processor.af.high;
            break;
        case 0x68: /* L,B */
            handle->processor.hl.low = handle->processor.bc.high;
            break;
        case 0x69: /* L,C */
            handle->processor.hl.low = handle->processor.bc.low;
            break;
        case 0x6A: /* L,D */
            handle->processor.hl.low = handle->processor.de.high;
            break;
        case 0x6B: /* L,E */
            handle->processor.hl.low = handle->processor.de.low;
            break;
        case 0x6C: /* L,H */
            handle->processor.hl.low = handle->processor.hl.high;
            break;
        case 0x6D: /* L,L */
            break;
        case 0x6E: /* L,(HL) */
            handle->processor.delay += 4;
            handle->processor.hl.low = dmg_read(handle, handle->processor.hl.word);
            break;
        case 0x6F: /* L,A */
            handle->processor.hl.low = handle->processor.af.high;
            break;
        case 0x70: /* (HL),B */
            handle->processor.delay += 4;
            dmg_write(handle, handle->processor.hl.word, handle->processor.bc.high);
            break;
        case 0x71: /* (HL),C */
            handle->processor.delay += 4;
            dmg_write(handle, handle->processor.hl.word, handle->processor.bc.low);
            break;
        case 0x72: /* (HL),D */
            handle->processor.delay += 4;
            dmg_write(handle, handle->processor.hl.word, handle->processor.de.high);
            break;
        case 0x73: /* (HL),E */
            handle->processor.delay += 4;
            dmg_write(handle, handle->processor.hl.word, handle->processor.de.low);
            break;
        case 0x74: /* (HL),H */
            handle->processor.delay += 4;
            dmg_write(handle, handle->processor.hl.word, handle->processor.hl.high);
            break;
        case 0x75: /* (HL),L */
            handle->processor.delay += 4;
            dmg_write(handle, handle->processor.hl.word, handle->processor.hl.low);
            break;
        case 0x77: /* (HL),A */
            handle->processor.delay += 4;
            dmg_write(handle, handle->processor.hl.word, handle->processor.af.high);
            break;
        case 0x78: /* A,B */
            handle->processor.af.high = handle->processor.bc.high;
            break;
        case 0x79: /* A,C */
            handle->processor.af.high = handle->processor.bc.low;
            break;
        case 0x7A: /* A,D */
            handle->processor.af.high = handle->processor.de.high;
            break;
        case 0x7B: /* A,E */
            handle->processor.af.high = handle->processor.de.low;
            break;
        case 0x7C: /* A,H */
            handle->processor.af.high = handle->processor.hl.high;
            break;
        case 0x7D: /* A,L */
            handle->processor.af.high = handle->processor.hl.low;
            break;
        case 0x7E: /* A,(HL) */
            handle->processor.delay += 4;
            handle->processor.af.high = dmg_read(handle, handle->processor.hl.word);
            break;
        case 0x7F: /* A,A */
            break;
        case 0xE0: /* (FF00+#),A */
            handle->processor.delay += 8;
            dmg_write(handle, 0xFF00 + dmg_read(handle, handle->processor.pc.word++), handle->processor.af.high);
            break;
        case 0xE2: /* (FF00+C),A */
            handle->processor.delay += 4;
            dmg_write(handle, 0xFF00 + handle->processor.bc.low, handle->processor.af.high);
            break;
        case 0xEA: /* (##),A */
            handle->processor.delay += 12;
            operand.low = dmg_read(handle, handle->processor.pc.word++);
            operand.high = dmg_read(handle, handle->processor.pc.word++);
            dmg_write(handle, operand.word, handle->processor.af.high);
            break;
        case 0xF0: /* A,(FF00+#) */
            handle->processor.delay += 8;
            handle->processor.af.high = dmg_read(handle, 0xFF00 + dmg_read(handle, handle->processor.pc.word++));
            break;
        case 0xF2: /* A,(FF00+C) */
            handle->processor.delay += 4;
            handle->processor.af.high = dmg_read(handle, 0xFF00 + handle->processor.bc.low);
            break;
        case 0xF9: /* SP,HL */
            handle->processor.delay += 4;
            handle->processor.sp.word = handle->processor.hl.word;
            break;
        case 0xFA: /* A,(##) */
            handle->processor.delay += 12;
            operand.low = dmg_read(handle, handle->processor.pc.word++);
            operand.high = dmg_read(handle, handle->processor.pc.word++);
            handle->processor.af.high = dmg_read(handle, operand.word);
            break;
    }
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_ld_hl(dmg_handle_t const handle)
{
    uint32_t carry, sum;
    int8_t operand = dmg_read(handle, handle->processor.pc.word++);
    handle->processor.delay = 12;
    sum = handle->processor.sp.word + operand;
    carry = handle->processor.sp.word ^ operand ^ sum;
    handle->processor.af.carry = ((carry & 0x100) == 0x100);
    handle->processor.af.half_carry = ((carry & 0x10) == 0x10);
    handle->processor.af.negative = false;
    handle->processor.af.zero = false;
    handle->processor.hl.word = sum;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_nop(dmg_handle_t const handle)
{
    handle->processor.delay = 4;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_or(dmg_handle_t const handle)
{
    handle->processor.delay = 4;
    switch (handle->processor.instruction.opcode)
    {
        case 0xB0: /* B */
            handle->processor.af.high |= handle->processor.bc.high;
            break;
        case 0xB1: /* C */
            handle->processor.af.high |= handle->processor.bc.low;
            break;
        case 0xB2: /* D */
            handle->processor.af.high |= handle->processor.de.high;
            break;
        case 0xB3: /* E */
            handle->processor.af.high |= handle->processor.de.low;
            break;
        case 0xB4: /* H */
            handle->processor.af.high |= handle->processor.hl.high;
            break;
        case 0xB5: /* L */
            handle->processor.af.high |= handle->processor.hl.low;
            break;
        case 0xB6: /* (HL) */
            handle->processor.delay += 4;
            handle->processor.af.high |= dmg_read(handle, handle->processor.hl.word);
            break;
        case 0xB7: /* A */
            break;
        case 0xF6: /* # */
            handle->processor.delay += 4;
            handle->processor.af.high |= dmg_read(handle, handle->processor.pc.word++);
            break;
    }
    handle->processor.af.carry = false;
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    handle->processor.af.zero = !handle->processor.af.high;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_pop(dmg_handle_t const handle)
{
    handle->processor.delay = 12;
    switch (handle->processor.instruction.opcode)
    {
        case 0xC1: /* BC */
            handle->processor.bc.low = dmg_read(handle, handle->processor.sp.word++);
            handle->processor.bc.high = dmg_read(handle, handle->processor.sp.word++);
            break;
        case 0xD1: /* DE */
            handle->processor.de.low = dmg_read(handle, handle->processor.sp.word++);
            handle->processor.de.high = dmg_read(handle, handle->processor.sp.word++);
            break;
        case 0xE1: /* HL */
            handle->processor.hl.low = dmg_read(handle, handle->processor.sp.word++);
            handle->processor.hl.high = dmg_read(handle, handle->processor.sp.word++);
            break;
        case 0xF1: /* AF */
            handle->processor.af.low = dmg_read(handle, handle->processor.sp.word++) & 0xF0;
            handle->processor.af.high = dmg_read(handle, handle->processor.sp.word++);
            break;
    }
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_push(dmg_handle_t const handle)
{
    handle->processor.delay = 16;
    switch (handle->processor.instruction.opcode)
    {
        case 0xC5: /* BC */
            dmg_write(handle, --handle->processor.sp.word, handle->processor.bc.high);
            dmg_write(handle, --handle->processor.sp.word, handle->processor.bc.low);
            break;
        case 0xD5: /* DE */
            dmg_write(handle, --handle->processor.sp.word, handle->processor.de.high);
            dmg_write(handle, --handle->processor.sp.word, handle->processor.de.low);
            break;
        case 0xE5: /* HL */
            dmg_write(handle, --handle->processor.sp.word, handle->processor.hl.high);
            dmg_write(handle, --handle->processor.sp.word, handle->processor.hl.low);
            break;
        case 0xF5: /* AF */
            dmg_write(handle, --handle->processor.sp.word, handle->processor.af.high);
            dmg_write(handle, --handle->processor.sp.word, handle->processor.af.low);
            break;
    }
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_res(dmg_handle_t const handle)
{
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0x80: case 0x88: case 0x90: case 0x98:
        case 0xA0: case 0xA8: case 0xB0: case 0xB8: /* B */
            handle->processor.bc.high &= ~(1 << ((handle->processor.instruction.opcode - 0x80) / 8));
            break;
        case 0x81: case 0x89: case 0x91: case 0x99:
        case 0xA1: case 0xA9: case 0xB1: case 0xB9: /* C */
            handle->processor.bc.low &= ~(1 << ((handle->processor.instruction.opcode - 0x81) / 8));
            break;
        case 0x82: case 0x8A: case 0x92: case 0x9A:
        case 0xA2: case 0xAA: case 0xB2: case 0xBA: /* D */
            handle->processor.de.high &= ~(1 << ((handle->processor.instruction.opcode - 0x82) / 8));
            break;
        case 0x83: case 0x8B: case 0x93: case 0x9B:
        case 0xA3: case 0xAB: case 0xB3: case 0xBB: /* E */
            handle->processor.de.low &= ~(1 << ((handle->processor.instruction.opcode - 0x83) / 8));
            break;
        case 0x84: case 0x8C: case 0x94: case 0x9C:
        case 0xA4: case 0xAC: case 0xB4: case 0xBC: /* H */
            handle->processor.hl.high &= ~(1 << ((handle->processor.instruction.opcode - 0x84) / 8));
            break;
        case 0x85: case 0x8D: case 0x95: case 0x9D:
        case 0xA5: case 0xAD: case 0xB5: case 0xBD: /* L */
            handle->processor.hl.low &= ~(1 << ((handle->processor.instruction.opcode - 0x85) / 8));
            break;
        case 0x86: case 0x8E: case 0x96: case 0x9E:
        case 0xA6: case 0xAE: case 0xB6: case 0xBE: /* (HL) */
            handle->processor.delay += 8;
            dmg_write(handle, handle->processor.hl.word, dmg_read(handle, handle->processor.hl.word)
                & ~(1 << ((handle->processor.instruction.opcode - 0x86) / 8)));
            break;
        case 0x87: case 0x8F: case 0x97: case 0x9F:
        case 0xA7: case 0xAF: case 0xB7: case 0xBF: /* A */
            handle->processor.af.high &= ~(1 << ((handle->processor.instruction.opcode - 0x87) / 8));
            break;
    }
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_ret(dmg_handle_t const handle)
{
    bool taken = false;
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0xC0: /* NZ */
            taken = !handle->processor.af.zero;
            break;
        case 0xC8: /* Z */
            taken = handle->processor.af.zero;
            break;
        case 0xC9: /* N */
            taken = true;
            break;
        case 0xD0: /* NC */
            taken = !handle->processor.af.carry;
            break;
        case 0xD8: /* C */
            taken = handle->processor.af.carry;
            break;
    }
    if (taken)
    {
        handle->processor.delay += (handle->processor.instruction.opcode == 0xC9) ? 8 : 12; /* N */
        handle->processor.pc.low = dmg_read(handle, handle->processor.sp.word++);
        handle->processor.pc.high = dmg_read(handle, handle->processor.sp.word++);
    }
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_reti(dmg_handle_t const handle)
{
    handle->processor.delay = 16;
    handle->processor.pc.low = dmg_read(handle, handle->processor.sp.word++);
    handle->processor.pc.high = dmg_read(handle, handle->processor.sp.word++);
    handle->processor.interrupt.enable_delay = 0;
    handle->processor.interrupt.enabled = true;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_rl(dmg_handle_t const handle)
{
    uint8_t carry = handle->processor.af.carry, operand = 0;
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0x10: /* B */
            handle->processor.af.carry = ((handle->processor.bc.high & 0x80) == 0x80);
            handle->processor.bc.high = (handle->processor.bc.high << 1) | carry;
            handle->processor.af.zero = !handle->processor.bc.high;
            break;
        case 0x11: /* C */
            handle->processor.af.carry = ((handle->processor.bc.low & 0x80) == 0x80);
            handle->processor.bc.low = (handle->processor.bc.low << 1) | carry;
            handle->processor.af.zero = !handle->processor.bc.low;
            break;
        case 0x12: /* D */
            handle->processor.af.carry = ((handle->processor.de.high & 0x80) == 0x80);
            handle->processor.de.high = (handle->processor.de.high << 1) | carry;
            handle->processor.af.zero = !handle->processor.de.high;
            break;
        case 0x13: /* E */
            handle->processor.af.carry = ((handle->processor.de.low & 0x80) == 0x80);
            handle->processor.de.low = (handle->processor.de.low << 1) | carry;
            handle->processor.af.zero = !handle->processor.de.low;
            break;
        case 0x14: /* H */
            handle->processor.af.carry = ((handle->processor.hl.high & 0x80) == 0x80);
            handle->processor.hl.high = (handle->processor.hl.high << 1) | carry;
            handle->processor.af.zero = !handle->processor.hl.high;
            break;
        case 0x15: /* L */
            handle->processor.af.carry = ((handle->processor.hl.low & 0x80) == 0x80);
            handle->processor.hl.low = (handle->processor.hl.low << 1) | carry;
            handle->processor.af.zero = !handle->processor.hl.low;
            break;
        case 0x16: /* (HL) */
            handle->processor.delay += 8;
            operand = dmg_read(handle, handle->processor.hl.word);
            handle->processor.af.carry = ((operand & 0x80) == 0x80);
            operand = (operand << 1) | carry;
            handle->processor.af.zero = !operand;
            dmg_write(handle, handle->processor.hl.word, operand);
            break;
        case 0x17: /* A */
            handle->processor.af.carry = ((handle->processor.af.high & 0x80) == 0x80);
            handle->processor.af.high = (handle->processor.af.high << 1) | carry;
            handle->processor.af.zero = !handle->processor.af.high;
            break;
    }
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_rla(dmg_handle_t const handle)
{
    uint8_t carry = handle->processor.af.carry;
    handle->processor.delay = 4;
    handle->processor.af.carry = ((handle->processor.af.high & 0x80) == 0x80);
    handle->processor.af.high = (handle->processor.af.high << 1) | carry;
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    handle->processor.af.zero = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_rlc(dmg_handle_t const handle)
{
    uint8_t operand = 0;
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0x00: /* B */
            handle->processor.af.carry = ((handle->processor.bc.high & 0x80) == 0x80);
            handle->processor.bc.high = (handle->processor.bc.high << 1) | handle->processor.af.carry;
            handle->processor.af.zero = !handle->processor.bc.high;
            break;
        case 0x01: /* C */
            handle->processor.af.carry = ((handle->processor.bc.low & 0x80) == 0x80);
            handle->processor.bc.low = (handle->processor.bc.low << 1) | handle->processor.af.carry;
            handle->processor.af.zero = !handle->processor.bc.low;
            break;
        case 0x02: /* D */
            handle->processor.af.carry = ((handle->processor.de.high & 0x80) == 0x80);
            handle->processor.de.high = (handle->processor.de.high << 1) | handle->processor.af.carry;
            handle->processor.af.zero = !handle->processor.de.high;
            break;
        case 0x03: /* E */
            handle->processor.af.carry = ((handle->processor.de.low & 0x80) == 0x80);
            handle->processor.de.low = (handle->processor.de.low << 1) | handle->processor.af.carry;
            handle->processor.af.zero = !handle->processor.de.low;
            break;
        case 0x04: /* H */
            handle->processor.af.carry = ((handle->processor.hl.high & 0x80) == 0x80);
            handle->processor.hl.high = (handle->processor.hl.high << 1) | handle->processor.af.carry;
            handle->processor.af.zero = !handle->processor.hl.high;
            break;
        case 0x05: /* L */
            handle->processor.af.carry = ((handle->processor.hl.low & 0x80) == 0x80);
            handle->processor.hl.low = (handle->processor.hl.low << 1) | handle->processor.af.carry;
            handle->processor.af.zero = !handle->processor.hl.low;
            break;
        case 0x06: /* (HL) */
            handle->processor.delay += 8;
            operand = dmg_read(handle, handle->processor.hl.word);
            handle->processor.af.carry = ((operand & 0x80) == 0x80);
            operand = (operand << 1) | handle->processor.af.carry;
            handle->processor.af.zero = !operand;
            dmg_write(handle, handle->processor.hl.word, operand);
            break;
        case 0x07: /* A */
            handle->processor.af.carry = ((handle->processor.af.high & 0x80) == 0x80);
            handle->processor.af.high = (handle->processor.af.high << 1) | handle->processor.af.carry;
            handle->processor.af.zero = !handle->processor.af.high;
            break;
    }
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_rlca(dmg_handle_t const handle)
{
    handle->processor.delay = 4;
    handle->processor.af.carry = ((handle->processor.af.high & 0x80) == 0x80);
    handle->processor.af.high = (handle->processor.af.high << 1) | handle->processor.af.carry;
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    handle->processor.af.zero = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_rr(dmg_handle_t const handle)
{
    uint8_t carry = handle->processor.af.carry, operand = 0;
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0x18: /* B */
            handle->processor.af.carry = handle->processor.bc.high & 1;
            handle->processor.bc.high = (handle->processor.bc.high >> 1) | (carry ? 0x80 : 0);
            handle->processor.af.zero = !handle->processor.bc.high;
            break;
        case 0x19: /* C */
            handle->processor.af.carry = handle->processor.bc.low & 1;
            handle->processor.bc.low = (handle->processor.bc.low >> 1) | (carry ? 0x80 : 0);
            handle->processor.af.zero = !handle->processor.bc.low;
            break;
        case 0x1A: /* D */
            handle->processor.af.carry = handle->processor.de.high & 1;
            handle->processor.de.high = (handle->processor.de.high >> 1) | (carry ? 0x80 : 0);
            handle->processor.af.zero = !handle->processor.de.high;
            break;
        case 0x1B: /* E */
            handle->processor.af.carry = handle->processor.de.low & 1;
            handle->processor.de.low = (handle->processor.de.low >> 1) | (carry ? 0x80 : 0);
            handle->processor.af.zero = !handle->processor.de.low;
            break;
        case 0x1C: /* H */
            handle->processor.af.carry = handle->processor.hl.high & 1;
            handle->processor.hl.high = (handle->processor.hl.high >> 1) | (carry ? 0x80 : 0);
            handle->processor.af.zero = !handle->processor.hl.high;
            break;
        case 0x1D: /* L */
            handle->processor.af.carry = handle->processor.hl.low & 1;
            handle->processor.hl.low = (handle->processor.hl.low >> 1) | (carry ? 0x80 : 0);
            handle->processor.af.zero = !handle->processor.hl.low;
            break;
        case 0x1E: /* (HL) */
            handle->processor.delay += 8;
            operand = dmg_read(handle, handle->processor.hl.word);
            handle->processor.af.carry = operand & 1;
            operand = (operand >> 1) | (carry ? 0x80 : 0);
            handle->processor.af.zero = !operand;
            dmg_write(handle, handle->processor.hl.word, operand);
            break;
        case 0x1F: /* A */
            handle->processor.af.carry = handle->processor.af.high & 1;
            handle->processor.af.high = (handle->processor.af.high >> 1) | (carry ? 0x80 : 0);
            handle->processor.af.zero = !handle->processor.af.high;
            break;
    }
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_rra(dmg_handle_t const handle)
{
    uint8_t carry = handle->processor.af.carry;
    handle->processor.delay = 4;
    handle->processor.af.carry = ((handle->processor.af.high & 1) == 1);
    handle->processor.af.high = (handle->processor.af.high >> 1) | (carry ? 0x80 : 0);
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    handle->processor.af.zero = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_rrc(dmg_handle_t const handle)
{
    uint8_t operand = 0;
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0x08: /* B */
            handle->processor.af.carry = handle->processor.bc.high & 1;
            handle->processor.bc.high = (handle->processor.bc.high >> 1) | (handle->processor.af.carry ? 0x80 : 0);
            handle->processor.af.zero = !handle->processor.bc.high;
            break;
        case 0x09: /* C */
            handle->processor.af.carry = handle->processor.bc.low & 1;
            handle->processor.bc.low = (handle->processor.bc.low >> 1) | (handle->processor.af.carry ? 0x80 : 0);
            handle->processor.af.zero = !handle->processor.bc.low;
            break;
        case 0x0A: /* D */
            handle->processor.af.carry = handle->processor.de.high & 1;
            handle->processor.de.high = (handle->processor.de.high >> 1) | (handle->processor.af.carry ? 0x80 : 0);
            handle->processor.af.zero = !handle->processor.de.high;
            break;
        case 0x0B: /* E */
            handle->processor.af.carry = handle->processor.de.low & 1;
            handle->processor.de.low = (handle->processor.de.low >> 1) | (handle->processor.af.carry ? 0x80 : 0);
            handle->processor.af.zero = !handle->processor.de.low;
            break;
        case 0x0C: /* H */
            handle->processor.af.carry = handle->processor.hl.high & 1;
            handle->processor.hl.high = (handle->processor.hl.high >> 1) | (handle->processor.af.carry ? 0x80 : 0);
            handle->processor.af.zero = !handle->processor.hl.high;
            break;
        case 0x0D: /* L */
            handle->processor.af.carry = handle->processor.hl.low & 1;
            handle->processor.hl.low = (handle->processor.hl.low >> 1) | (handle->processor.af.carry ? 0x80 : 0);
            handle->processor.af.zero = !handle->processor.hl.low;
            break;
        case 0x0E: /* (HL) */
            handle->processor.delay += 8;
            operand = dmg_read(handle, handle->processor.hl.word);
            handle->processor.af.carry = operand & 1;
            operand = (operand >> 1) | (handle->processor.af.carry ? 0x80 : 0);
            handle->processor.af.zero = !operand;
            dmg_write(handle, handle->processor.hl.word, operand);
            break;
        case 0x0F: /* A */
            handle->processor.af.carry = handle->processor.af.high & 1;
            handle->processor.af.high = (handle->processor.af.high >> 1) | (handle->processor.af.carry ? 0x80 : 0);
            handle->processor.af.zero = !handle->processor.af.high;
            break;
    }
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_rrca(dmg_handle_t const handle)
{
    handle->processor.delay = 4;
    handle->processor.af.carry = ((handle->processor.af.high & 1) == 1);
    handle->processor.af.high = (handle->processor.af.high >> 1) | (handle->processor.af.carry ? 0x80 : 0);
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    handle->processor.af.zero = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_rst(dmg_handle_t const handle)
{
    handle->processor.delay = 16;
    dmg_write(handle, --handle->processor.sp.word, handle->processor.pc.high);
    dmg_write(handle, --handle->processor.sp.word, handle->processor.pc.low);
    handle->processor.pc.word = handle->processor.instruction.opcode - 0xC7;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_sbc(dmg_handle_t const handle)
{
    uint16_t sum;
    uint8_t carry, operand = 0;
    handle->processor.delay = 4;
    switch (handle->processor.instruction.opcode)
    {
        case 0x98: /* B */
            operand = handle->processor.bc.high;
            break;
        case 0x99: /* C */
            operand = handle->processor.bc.low;
            break;
        case 0x9A: /* D */
            operand = handle->processor.de.high;
            break;
        case 0x9B: /* E */
            operand = handle->processor.de.low;
            break;
        case 0x9C: /* H */
            operand = handle->processor.hl.high;
            break;
        case 0x9D: /* L */
            operand = handle->processor.hl.low;
            break;
        case 0x9E: /* (HL) */
            handle->processor.delay += 4;
            operand = dmg_read(handle, handle->processor.hl.word);
            break;
        case 0x9F: /* A */
            operand = handle->processor.af.high;
            break;
        case 0xDE: /* # */
            handle->processor.delay += 4;
            operand = dmg_read(handle, handle->processor.pc.word++);
            break;
    }
    carry = handle->processor.af.carry;
    sum = handle->processor.af.high - operand - carry;
    handle->processor.af.carry = ((int16_t)sum < 0);
    handle->processor.af.half_carry = ((int16_t)((handle->processor.af.high & 0x0F) - (operand & 0x0F) - carry) < 0);
    handle->processor.af.negative = true;
    handle->processor.af.zero = !(sum & 0xFF);
    handle->processor.af.high = sum;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_scf(dmg_handle_t const handle)
{
    handle->processor.delay = 4;
    handle->processor.af.carry = true;
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_set(dmg_handle_t const handle)
{
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0xC0: case 0xC8: case 0xD0: case 0xD8:
        case 0xE0: case 0xE8: case 0xF0: case 0xF8: /* B */
            handle->processor.bc.high |= (1 << ((handle->processor.instruction.opcode - 0xC0) / 8));
            break;
        case 0xC1: case 0xC9: case 0xD1: case 0xD9:
        case 0xE1: case 0xE9: case 0xF1: case 0xF9: /* C */
            handle->processor.bc.low |= (1 << ((handle->processor.instruction.opcode - 0xC1) / 8));
            break;
        case 0xC2: case 0xCA: case 0xD2: case 0xDA:
        case 0xE2: case 0xEA: case 0xF2: case 0xFA: /* D */
            handle->processor.de.high |= (1 << ((handle->processor.instruction.opcode - 0xC2) / 8));
            break;
        case 0xC3: case 0xCB: case 0xD3: case 0xDB:
        case 0xE3: case 0xEB: case 0xF3: case 0xFB: /* E */
            handle->processor.de.low |= (1 << ((handle->processor.instruction.opcode - 0xC3) / 8));
            break;
        case 0xC4: case 0xCC: case 0xD4: case 0xDC:
        case 0xE4: case 0xEC: case 0xF4: case 0xFC: /* H */
            handle->processor.hl.high |= (1 << ((handle->processor.instruction.opcode - 0xC4) / 8));
            break;
        case 0xC5: case 0xCD: case 0xD5: case 0xDD:
        case 0xE5: case 0xED: case 0xF5: case 0xFD: /* L */
            handle->processor.hl.low |= (1 << ((handle->processor.instruction.opcode - 0xC5) / 8));
            break;
        case 0xC6: case 0xCE: case 0xD6: case 0xDE:
        case 0xE6: case 0xEE: case 0xF6: case 0xFE: /* (HL) */
            handle->processor.delay += 8;
            dmg_write(handle, handle->processor.hl.word, dmg_read(handle, handle->processor.hl.word)
                | (1 << ((handle->processor.instruction.opcode - 0xC6) / 8)));
            break;
        case 0xC7: case 0xCF: case 0xD7: case 0xDF:
        case 0xE7: case 0xEF: case 0xF7: case 0xFF: /* A */
            handle->processor.af.high |= (1 << ((handle->processor.instruction.opcode - 0xC7) / 8));
            break;
    }
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_sla(dmg_handle_t const handle)
{
    uint8_t operand = 0;
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0x20: /* B */
            handle->processor.af.carry = ((handle->processor.bc.high & 0x80) == 0x80);
            handle->processor.bc.high <<= 1;
            handle->processor.af.zero = !handle->processor.bc.high;
            break;
        case 0x21: /* C */
            handle->processor.af.carry = ((handle->processor.bc.low & 0x80) == 0x80);
            handle->processor.bc.low <<= 1;
            handle->processor.af.zero = !handle->processor.bc.low;
            break;
        case 0x22: /* D */
            handle->processor.af.carry = ((handle->processor.de.high & 0x80) == 0x80);
            handle->processor.de.high <<= 1;
            handle->processor.af.zero = !handle->processor.de.high;
            break;
        case 0x23: /* E */
            handle->processor.af.carry = ((handle->processor.de.low & 0x80) == 0x80);
            handle->processor.de.low <<= 1;
            handle->processor.af.zero = !handle->processor.de.low;
            break;
        case 0x24: /* H */
            handle->processor.af.carry = ((handle->processor.hl.high & 0x80) == 0x80);
            handle->processor.hl.high <<= 1;
            handle->processor.af.zero = !handle->processor.hl.high;
            break;
        case 0x25: /* L */
            handle->processor.af.carry = ((handle->processor.hl.low & 0x80) == 0x80);
            handle->processor.hl.low <<= 1;
            handle->processor.af.zero = !handle->processor.hl.low;
            break;
        case 0x26: /* (HL) */
            handle->processor.delay += 8;
            operand = dmg_read(handle, handle->processor.hl.word);
            handle->processor.af.carry = ((operand & 0x80) == 0x80);
            operand <<= 1;
            handle->processor.af.zero = !operand;
            dmg_write(handle, handle->processor.hl.word, operand);
            break;
        case 0x27: /* A */
            handle->processor.af.carry = ((handle->processor.af.high & 0x80) == 0x80);
            handle->processor.af.high <<= 1;
            handle->processor.af.zero = !handle->processor.af.high;
            break;
    }
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_sra(dmg_handle_t const handle)
{
    uint8_t operand = 0;
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0x28: /* B */
            handle->processor.af.carry = handle->processor.bc.high & 1;
            handle->processor.bc.high = (handle->processor.bc.high >> 1) | (handle->processor.bc.high & 0x80);
            handle->processor.af.zero = !handle->processor.bc.high;
            break;
        case 0x29: /* C */
            handle->processor.af.carry = handle->processor.bc.low & 1;
            handle->processor.bc.low = (handle->processor.bc.low >> 1) | (handle->processor.bc.low & 0x80);
            handle->processor.af.zero = !handle->processor.bc.low;
            break;
        case 0x2A: /* D */
            handle->processor.af.carry = handle->processor.de.high & 1;
            handle->processor.de.high = (handle->processor.de.high >> 1) | (handle->processor.de.high & 0x80);
            handle->processor.af.zero = !handle->processor.de.high;
            break;
        case 0x2B: /* E */
            handle->processor.af.carry = handle->processor.de.low & 1;
            handle->processor.de.low = (handle->processor.de.low >> 1) | (handle->processor.de.low & 0x80);
            handle->processor.af.zero = !handle->processor.de.low;
            break;
        case 0x2C: /* H */
            handle->processor.af.carry = handle->processor.hl.high & 1;
            handle->processor.hl.high = (handle->processor.hl.high >> 1) | (handle->processor.hl.high & 0x80);
            handle->processor.af.zero = !handle->processor.hl.high;
            break;
        case 0x2D: /* L */
            handle->processor.af.carry = handle->processor.hl.low & 1;
            handle->processor.hl.low = (handle->processor.hl.low >> 1) | (handle->processor.hl.low & 0x80);
            handle->processor.af.zero = !handle->processor.hl.low;
            break;
        case 0x2E: /* (HL) */
            handle->processor.delay += 8;
            operand = dmg_read(handle, handle->processor.hl.word);
            handle->processor.af.carry = operand & 1;
            operand = (operand >> 1) | (operand & 0x80);
            handle->processor.af.zero = !operand;
            dmg_write(handle, handle->processor.hl.word, operand);
            break;
        case 0x2F: /* A */
            handle->processor.af.carry = handle->processor.af.high & 1;
            handle->processor.af.high = (handle->processor.af.high >> 1) | (handle->processor.af.high & 0x80);
            handle->processor.af.zero = !handle->processor.af.high;
            break;
    }
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_srl(dmg_handle_t const handle)
{
    uint8_t operand = 0;
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0x38: /* B */
            handle->processor.af.carry = handle->processor.bc.high & 1;
            handle->processor.bc.high >>= 1;
            handle->processor.af.zero = !handle->processor.bc.high;
            break;
        case 0x39: /* C */
            handle->processor.af.carry = handle->processor.bc.low & 1;
            handle->processor.bc.low >>= 1;
            handle->processor.af.zero = !handle->processor.bc.low;
            break;
        case 0x3A: /* D */
            handle->processor.af.carry = handle->processor.de.high & 1;
            handle->processor.de.high >>= 1;
            handle->processor.af.zero = !handle->processor.de.high;
            break;
        case 0x3B: /* E */
            handle->processor.af.carry = handle->processor.de.low & 1;
            handle->processor.de.low >>= 1;
            handle->processor.af.zero = !handle->processor.de.low;
            break;
        case 0x3C: /* H */
            handle->processor.af.carry = handle->processor.hl.high & 1;
            handle->processor.hl.high >>= 1;
            handle->processor.af.zero = !handle->processor.hl.high;
            break;
        case 0x3D: /* L */
            handle->processor.af.carry = handle->processor.hl.low & 1;
            handle->processor.hl.low >>= 1;
            handle->processor.af.zero = !handle->processor.hl.low;
            break;
        case 0x3E: /* (HL) */
            handle->processor.delay += 8;
            operand = dmg_read(handle, handle->processor.hl.word);
            handle->processor.af.carry = operand & 1;
            operand >>= 1;
            handle->processor.af.zero = !operand;
            dmg_write(handle, handle->processor.hl.word, operand);
            break;
        case 0x3F: /* A */
            handle->processor.af.carry = handle->processor.af.high & 1;
            handle->processor.af.high >>= 1;
            handle->processor.af.zero = !handle->processor.af.high;
            break;
    }
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_stop(dmg_handle_t const handle)
{
    handle->processor.delay = 4;
    handle->processor.stopped = true;
    dmg_read(handle, handle->processor.pc.word++);
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_sub(dmg_handle_t const handle)
{
    uint8_t operand = 0;
    uint16_t carry, sum;
    handle->processor.delay = 4;
    switch (handle->processor.instruction.opcode)
    {
        case 0x90: /* B */
            operand = handle->processor.bc.high;
            break;
        case 0x91: /* C */
            operand = handle->processor.bc.low;
            break;
        case 0x92: /* D */
            operand = handle->processor.de.high;
            break;
        case 0x93: /* E */
            operand = handle->processor.de.low;
            break;
        case 0x94: /* H */
            operand = handle->processor.hl.high;
            break;
        case 0x95: /* L */
            operand = handle->processor.hl.low;
            break;
        case 0x96: /* (HL) */
            handle->processor.delay += 4;
            operand = dmg_read(handle, handle->processor.hl.word);
            break;
        case 0x97: /* A */
            operand = handle->processor.af.high;
            break;
        case 0xD6: /* # */
            handle->processor.delay += 4;
            operand = dmg_read(handle, handle->processor.pc.word++);
            break;
    }
    sum = handle->processor.af.high - operand;
    carry = handle->processor.af.high ^ operand ^ sum;
    handle->processor.af.carry = ((carry & 0x100) == 0x100);
    handle->processor.af.half_carry = ((carry & 0x10) == 0x10);
    handle->processor.af.negative = true;
    handle->processor.af.zero = !(sum & 0xFF);
    handle->processor.af.high = sum;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_swap(dmg_handle_t const handle)
{
    uint8_t operand = 0;
    handle->processor.delay = 8;
    switch (handle->processor.instruction.opcode)
    {
        case 0x30: /* B */
            handle->processor.bc.high = (handle->processor.bc.high << 4) | (handle->processor.bc.high >> 4);
            handle->processor.af.zero = !handle->processor.bc.high;
            break;
        case 0x31: /* C */
            handle->processor.bc.low = (handle->processor.bc.low << 4) | (handle->processor.bc.low >> 4);
            handle->processor.af.zero = !handle->processor.bc.low;
            break;
        case 0x32: /* D */
            handle->processor.de.high = (handle->processor.de.high << 4) | (handle->processor.de.high >> 4);
            handle->processor.af.zero = !handle->processor.de.high;
            break;
        case 0x33: /* E */
            handle->processor.de.low = (handle->processor.de.low << 4) | (handle->processor.de.low >> 4);
            handle->processor.af.zero = !handle->processor.de.low;
            break;
        case 0x34: /* H */
            handle->processor.hl.high = (handle->processor.hl.high << 4) | (handle->processor.hl.high >> 4);
            handle->processor.af.zero = !handle->processor.hl.high;
            break;
        case 0x35: /* L */
            handle->processor.hl.low = (handle->processor.hl.low << 4) | (handle->processor.hl.low >> 4);
            handle->processor.af.zero = !handle->processor.hl.low;
            break;
        case 0x36: /* (HL) */
            handle->processor.delay += 8;
            operand = dmg_read(handle, handle->processor.hl.word);
            operand = (operand << 4) | (operand >> 4);
            handle->processor.af.zero = !operand;
            dmg_write(handle, handle->processor.hl.word, operand);
            break;
        case 0x37: /* A */
            handle->processor.af.high = (handle->processor.af.high << 4) | (handle->processor.af.high >> 4);
            handle->processor.af.zero = !handle->processor.af.high;
            break;
    }
    handle->processor.af.carry = false;
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_xor(dmg_handle_t const handle)
{
    handle->processor.delay = 4;
    switch (handle->processor.instruction.opcode)
    {
        case 0xA8: /* B */
            handle->processor.af.high ^= handle->processor.bc.high;
            break;
        case 0xA9: /* C */
            handle->processor.af.high ^= handle->processor.bc.low;
            break;
        case 0xAA: /* D */
            handle->processor.af.high ^= handle->processor.de.high;
            break;
        case 0xAB: /* E */
            handle->processor.af.high ^= handle->processor.de.low;
            break;
        case 0xAC: /* H */
            handle->processor.af.high ^= handle->processor.hl.high;
            break;
        case 0xAD: /* L */
            handle->processor.af.high ^= handle->processor.hl.low;
            break;
        case 0xAE: /* (HL) */
            handle->processor.delay += 4;
            handle->processor.af.high ^= dmg_read(handle, handle->processor.hl.word);
            break;
        case 0xAF: /* A */
            handle->processor.af.high = 0;
            break;
        case 0xEE: /* # */
            handle->processor.delay += 4;
            handle->processor.af.high ^= dmg_read(handle, handle->processor.pc.word++);
            break;
    }
    handle->processor.af.carry = false;
    handle->processor.af.half_carry = false;
    handle->processor.af.negative = false;
    handle->processor.af.zero = !handle->processor.af.high;
    return DMG_SUCCESS;
}

static dmg_error_e dmg_processor_xxx(dmg_handle_t const handle)
{
    return DMG_ERROR(handle, "Invalid opcode -- [%04X] %02X", handle->processor.instruction.address, handle->processor.instruction.opcode);
}

static const dmg_processor_f INSTRUCTION[] =
{
    /* 00 */
    dmg_processor_nop, dmg_processor_ld, dmg_processor_ld, dmg_processor_inc_word,
    dmg_processor_inc, dmg_processor_dec, dmg_processor_ld, dmg_processor_rlca,
    /* 08 */
    dmg_processor_ld, dmg_processor_add_hl, dmg_processor_ld, dmg_processor_dec_word,
    dmg_processor_inc, dmg_processor_dec, dmg_processor_ld, dmg_processor_rrca,
    /* 10 */
    dmg_processor_stop, dmg_processor_ld, dmg_processor_ld, dmg_processor_inc_word,
    dmg_processor_inc, dmg_processor_dec, dmg_processor_ld, dmg_processor_rla,
    /* 18 */
    dmg_processor_jr, dmg_processor_add_hl, dmg_processor_ld, dmg_processor_dec_word,
    dmg_processor_inc, dmg_processor_dec, dmg_processor_ld, dmg_processor_rra,
    /* 20 */
    dmg_processor_jr, dmg_processor_ld, dmg_processor_ld, dmg_processor_inc_word,
    dmg_processor_inc, dmg_processor_dec, dmg_processor_ld, dmg_processor_daa,
    /* 28 */
    dmg_processor_jr, dmg_processor_add_hl, dmg_processor_ld, dmg_processor_dec_word,
    dmg_processor_inc, dmg_processor_dec, dmg_processor_ld, dmg_processor_cpl,
    /* 30 */
    dmg_processor_jr, dmg_processor_ld, dmg_processor_ld, dmg_processor_inc_word,
    dmg_processor_inc, dmg_processor_dec, dmg_processor_ld, dmg_processor_scf,
    /* 38 */
    dmg_processor_jr, dmg_processor_add_hl, dmg_processor_ld, dmg_processor_dec_word,
    dmg_processor_inc, dmg_processor_dec, dmg_processor_ld, dmg_processor_ccf,
    /* 40 */
    dmg_processor_ld, dmg_processor_ld, dmg_processor_ld, dmg_processor_ld,
    dmg_processor_ld, dmg_processor_ld, dmg_processor_ld, dmg_processor_ld,
    /* 48 */
    dmg_processor_ld, dmg_processor_ld, dmg_processor_ld, dmg_processor_ld,
    dmg_processor_ld, dmg_processor_ld, dmg_processor_ld, dmg_processor_ld,
    /* 50 */
    dmg_processor_ld, dmg_processor_ld, dmg_processor_ld, dmg_processor_ld,
    dmg_processor_ld, dmg_processor_ld, dmg_processor_ld, dmg_processor_ld,
    /* 58 */
    dmg_processor_ld, dmg_processor_ld, dmg_processor_ld, dmg_processor_ld,
    dmg_processor_ld, dmg_processor_ld, dmg_processor_ld, dmg_processor_ld,
    /* 60 */
    dmg_processor_ld, dmg_processor_ld, dmg_processor_ld, dmg_processor_ld,
    dmg_processor_ld, dmg_processor_ld, dmg_processor_ld, dmg_processor_ld,
    /* 68 */
    dmg_processor_ld, dmg_processor_ld, dmg_processor_ld, dmg_processor_ld,
    dmg_processor_ld, dmg_processor_ld, dmg_processor_ld, dmg_processor_ld,
    /* 70 */
    dmg_processor_ld, dmg_processor_ld, dmg_processor_ld, dmg_processor_ld,
    dmg_processor_ld, dmg_processor_ld, dmg_processor_halt, dmg_processor_ld,
    /* 78 */
    dmg_processor_ld, dmg_processor_ld, dmg_processor_ld, dmg_processor_ld,
    dmg_processor_ld, dmg_processor_ld, dmg_processor_ld, dmg_processor_ld,
    /* 80 */
    dmg_processor_add, dmg_processor_add, dmg_processor_add, dmg_processor_add,
    dmg_processor_add, dmg_processor_add, dmg_processor_add, dmg_processor_add,
    /* 88 */
    dmg_processor_adc, dmg_processor_adc, dmg_processor_adc, dmg_processor_adc,
    dmg_processor_adc, dmg_processor_adc, dmg_processor_adc, dmg_processor_adc,
    /* 90 */
    dmg_processor_sub, dmg_processor_sub, dmg_processor_sub, dmg_processor_sub,
    dmg_processor_sub, dmg_processor_sub, dmg_processor_sub, dmg_processor_sub,
    /* 98 */
    dmg_processor_sbc, dmg_processor_sbc, dmg_processor_sbc, dmg_processor_sbc,
    dmg_processor_sbc, dmg_processor_sbc, dmg_processor_sbc, dmg_processor_sbc,
    /* A0 */
    dmg_processor_and, dmg_processor_and, dmg_processor_and, dmg_processor_and,
    dmg_processor_and, dmg_processor_and, dmg_processor_and, dmg_processor_and,
    /* A8 */
    dmg_processor_xor, dmg_processor_xor, dmg_processor_xor, dmg_processor_xor,
    dmg_processor_xor, dmg_processor_xor, dmg_processor_xor, dmg_processor_xor,
    /* B0 */
    dmg_processor_or, dmg_processor_or, dmg_processor_or, dmg_processor_or,
    dmg_processor_or, dmg_processor_or, dmg_processor_or, dmg_processor_or,
    /* B8 */
    dmg_processor_cp, dmg_processor_cp, dmg_processor_cp, dmg_processor_cp,
    dmg_processor_cp, dmg_processor_cp, dmg_processor_cp, dmg_processor_cp,
    /* C0 */
    dmg_processor_ret, dmg_processor_pop, dmg_processor_jp, dmg_processor_jp,
    dmg_processor_call, dmg_processor_push, dmg_processor_add, dmg_processor_rst,
    /* C8 */
    dmg_processor_ret, dmg_processor_ret, dmg_processor_jp, dmg_processor_xxx,
    dmg_processor_call, dmg_processor_call, dmg_processor_adc, dmg_processor_rst,
    /* D0 */
    dmg_processor_ret, dmg_processor_pop, dmg_processor_jp, dmg_processor_xxx,
    dmg_processor_call, dmg_processor_push, dmg_processor_sub, dmg_processor_rst,
    /* D8 */
    dmg_processor_ret, dmg_processor_reti, dmg_processor_jp, dmg_processor_xxx,
    dmg_processor_call, dmg_processor_xxx, dmg_processor_sbc, dmg_processor_rst,
    /* E0 */
    dmg_processor_ld, dmg_processor_pop, dmg_processor_ld, dmg_processor_xxx,
    dmg_processor_xxx, dmg_processor_push, dmg_processor_and, dmg_processor_rst,
    /* E8 */
    dmg_processor_add_sp, dmg_processor_jp_hl, dmg_processor_ld, dmg_processor_xxx,
    dmg_processor_xxx, dmg_processor_xxx, dmg_processor_xor, dmg_processor_rst,
    /* F0 */
    dmg_processor_ld, dmg_processor_pop, dmg_processor_ld, dmg_processor_di,
    dmg_processor_xxx, dmg_processor_push, dmg_processor_or, dmg_processor_rst,
    /* F8 */
    dmg_processor_ld_hl, dmg_processor_ld, dmg_processor_ld, dmg_processor_ei,
    dmg_processor_xxx, dmg_processor_xxx, dmg_processor_cp, dmg_processor_rst,
    /* CB 00 */
    dmg_processor_rlc, dmg_processor_rlc, dmg_processor_rlc, dmg_processor_rlc,
    dmg_processor_rlc, dmg_processor_rlc, dmg_processor_rlc, dmg_processor_rlc,
    /* CB 08 */
    dmg_processor_rrc, dmg_processor_rrc, dmg_processor_rrc, dmg_processor_rrc,
    dmg_processor_rrc, dmg_processor_rrc, dmg_processor_rrc, dmg_processor_rrc,
    /* CB 10 */
    dmg_processor_rl, dmg_processor_rl, dmg_processor_rl, dmg_processor_rl,
    dmg_processor_rl, dmg_processor_rl, dmg_processor_rl, dmg_processor_rl,
    /* CB 18 */
    dmg_processor_rr, dmg_processor_rr, dmg_processor_rr, dmg_processor_rr,
    dmg_processor_rr, dmg_processor_rr, dmg_processor_rr, dmg_processor_rr,
    /* CB 20 */
    dmg_processor_sla, dmg_processor_sla, dmg_processor_sla, dmg_processor_sla,
    dmg_processor_sla, dmg_processor_sla, dmg_processor_sla, dmg_processor_sla,
    /* CB 28 */
    dmg_processor_sra, dmg_processor_sra, dmg_processor_sra, dmg_processor_sra,
    dmg_processor_sra, dmg_processor_sra, dmg_processor_sra, dmg_processor_sra,
    /* CB 30 */
    dmg_processor_swap, dmg_processor_swap, dmg_processor_swap, dmg_processor_swap,
    dmg_processor_swap, dmg_processor_swap, dmg_processor_swap, dmg_processor_swap,
    /* CB 38 */
    dmg_processor_srl, dmg_processor_srl, dmg_processor_srl, dmg_processor_srl,
    dmg_processor_srl, dmg_processor_srl, dmg_processor_srl, dmg_processor_srl,
    /* CB 40 */
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    /* CB 48 */
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    /* CB 50 */
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    /* CB 58 */
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    /* CB 60 */
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    /* CB 68 */
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    /* CB 70 */
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    /* CB 78 */
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    dmg_processor_bit, dmg_processor_bit, dmg_processor_bit, dmg_processor_bit,
    /* CB 80 */
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    /* CB 88 */
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    /* CB 90 */
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    /* CB 98 */
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    /* CB A0 */
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    /* CB A8 */
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    /* CB B0 */
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    /* CB B8 */
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    dmg_processor_res, dmg_processor_res, dmg_processor_res, dmg_processor_res,
    /* CB C0 */
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    /* CB C8 */
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    /* CB D0 */
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    /* CB D8 */
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    /* CB E0 */
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    /* CB E8 */
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    /* CB F0 */
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    /* CB F8 */
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    dmg_processor_set, dmg_processor_set, dmg_processor_set, dmg_processor_set,
    };

static dmg_error_e dmg_processor_execute(dmg_handle_t const handle)
{
    bool extended;
    handle->processor.instruction.address = handle->processor.pc.word;
    handle->processor.instruction.opcode = dmg_read(handle, handle->processor.pc.word++);
    if ((extended = (handle->processor.instruction.opcode == 0xCB)))
    {
        handle->processor.instruction.opcode = dmg_read(handle, handle->processor.pc.word++);
    }
    return INSTRUCTION[extended ? handle->processor.instruction.opcode + 256 : handle->processor.instruction.opcode](handle);
}

static dmg_error_e dmg_processor_service(dmg_handle_t const handle)
{
    dmg_interrupt_e interrupt;
    for (interrupt = 0; interrupt < DMG_INTERRUPT_MAX; ++interrupt)
    {
        uint8_t mask = 1 << interrupt;
        if (handle->processor.interrupt.enable & handle->processor.interrupt.flag & mask)
        {
            dmg_write(handle, --handle->processor.sp.word, handle->processor.pc.high);
            dmg_write(handle, --handle->processor.sp.word, handle->processor.pc.low);
            handle->processor.pc.word = (0x0008 * interrupt) + 0x0040;
            handle->processor.interrupt.enabled = false;
            handle->processor.interrupt.flag &= ~mask;
            handle->processor.delay = 20;
            return DMG_SUCCESS;
        }
    }
    return DMG_ERROR(handle, "Invalid interrupt -- %02X", interrupt);
}

dmg_error_e dmg_processor_clock(dmg_handle_t const handle)
{
    dmg_error_e result = DMG_SUCCESS;
    if (!handle->processor.delay)
    {
        if (handle->processor.interrupt.enable_delay && !--handle->processor.interrupt.enable_delay)
        {
            handle->processor.interrupt.enabled = true;
        }
        if (handle->processor.interrupt.enable & handle->processor.interrupt.flag & 0x1F)
        {
            handle->processor.halted = false;
            if (handle->processor.interrupt.enabled)
            {
                result = dmg_processor_service(handle);
            }
            else if (!handle->processor.halted && !handle->processor.stopped)
            {
                result = dmg_processor_execute(handle);
            }
            else
            {
                handle->processor.delay = 4;
            }
        }
        else if (!handle->processor.halted && !handle->processor.stopped)
        {
            result = dmg_processor_execute(handle);
        }
        else
        {
            handle->processor.delay = 4;
        }
    }
    --handle->processor.delay;
    return result;
}

void dmg_processor_interrupt(dmg_handle_t const handle, dmg_interrupt_e interrupt)
{
    dmg_processor_write(handle, 0xFF0F, handle->processor.interrupt.flag | (1 << interrupt));
}

uint8_t dmg_processor_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0xFF0F: /* IF */
            result = handle->processor.interrupt.flag;
            break;
        case 0xFFFF: /* IE */
            result = handle->processor.interrupt.enable;
            break;
        default:
            break;
    }
    return result;
}

void dmg_processor_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0xFF0F: /* IF */
            handle->processor.interrupt.flag = 0xE0 | value;
            if (handle->processor.interrupt.flag & (1 << DMG_INTERRUPT_INPUT))
            {
                handle->processor.stopped = false;
            }
            break;
        case 0xFFFF: /* IE */
            handle->processor.interrupt.enable = value;
            break;
        default:
            break;
    }
}
