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

#ifndef DMG_AUDIO_H_
#define DMG_AUDIO_H_

#include <stdbool.h>
#include <dmg.h>

typedef union
{
    struct
    {
        uint8_t slope : 3;
        uint8_t decreasing : 1;
        uint8_t pace : 3;
    };
    uint8_t raw;
} dmg_audio_nr10;

typedef union
{
    struct
    {
        uint8_t timer : 6;
        uint8_t duty : 2;
    };
    uint8_t raw;
} dmg_audio_nr11;

typedef union
{
    struct
    {
        uint8_t pace : 3;
        uint8_t decreasing : 1;
        uint8_t volume : 4;
    };
    uint8_t raw;
} dmg_audio_nr12;

typedef union
{
    struct
    {
        uint8_t : 6;
        uint8_t enabled : 1;
        uint8_t triggered : 1;
    };
    uint8_t raw;
} dmg_audio_nr14;

typedef union
{
    struct
    {
        /* TODO */
    };
    uint8_t raw;
} dmg_audio_nr21;

typedef union
{
    struct
    {
        /* TODO */
    };
    uint8_t raw;
} dmg_audio_nr22;

typedef union
{
    struct
    {
        /* TODO */
    };
    uint8_t raw;
} dmg_audio_nr24;

typedef union
{
    struct
    {
        /* TODO */
    };
    uint8_t raw;
} dmg_audio_nr30;

typedef union
{
    struct
    {
        /* TODO */
    };
    uint8_t raw;
} dmg_audio_nr31;

typedef union
{
    struct
    {
        /* TODO */
    };
    uint8_t raw;
} dmg_audio_nr32;

typedef union
{
    struct
    {
        /* TODO */
    };
    uint8_t raw;
} dmg_audio_nr34;

typedef union
{
    struct
    {
        /* TODO */
    };
    uint8_t raw;
} dmg_audio_nr41;

typedef union
{
    struct
    {
        /* TODO */
    };
    uint8_t raw;
} dmg_audio_nr42;

typedef union
{
    struct
    {
        /* TODO */
    };
    uint8_t raw;
} dmg_audio_nr43;

typedef union
{
    struct
    {
        /* TODO */
    };
    uint8_t raw;
} dmg_audio_nr44;

typedef struct
{
    uint8_t delay;
    uint8_t ram[16];
    struct
    {
        bool full;
        uint32_t read;
        uint32_t write;
        int16_t sample[44100];
    } buffer;
    struct
    {
        dmg_audio_nr10 sweep;
        dmg_audio_nr11 length;
        dmg_audio_nr12 envelope;
        union {
            struct {
                uint8_t low;
                dmg_audio_nr14 high;
            };
            uint16_t raw : 11;
        } frequency;
    } channel_1;
    struct
    {
        dmg_audio_nr21 length;
        dmg_audio_nr22 envelope;
        union {
            struct {
                uint8_t low;
                dmg_audio_nr24 high;
            };
            uint16_t raw : 11;
        } frequency;
    } channel_2;
    struct
    {
        dmg_audio_nr30 control;
        dmg_audio_nr31 length;
        dmg_audio_nr32 wave;
        union {
            struct {
                uint8_t low;
                dmg_audio_nr34 high;
            };
            uint16_t raw : 11;
        } frequency;
    } channel_3;
    struct
    {
        dmg_audio_nr41 length;
        dmg_audio_nr42 envelope;
        dmg_audio_nr43 divider;
        dmg_audio_nr44 counter;
    } channel_4;
    union
    {
        struct
        {
            uint8_t channel_1_enabled : 1;
            uint8_t channel_2_enabled : 1;
            uint8_t channel_3_enabled : 1;
            uint8_t channel_4_enabled : 1;
            uint8_t : 3;
            uint8_t enabled : 1;
        };
        uint8_t raw;
    } control;
    union
    {
        struct
        {
            uint8_t right_channel_1 : 1;
            uint8_t right_channel_2 : 1;
            uint8_t right_channel_3 : 1;
            uint8_t right_channel_4 : 1;
            uint8_t left_channel_1 : 1;
            uint8_t left_channel_2 : 1;
            uint8_t left_channel_3 : 1;
            uint8_t left_channel_4 : 1;
        };
        uint8_t raw;
    } mixer;
    union
    {
        struct
        {
            uint8_t right_volume : 3;
            uint8_t right_vin_enabled : 1;
            uint8_t left_volume : 3;
            uint8_t left_vin_enabled : 1;
        };
        uint8_t raw;
    } volume;
} dmg_audio_t;

void dmg_audio_clock(dmg_handle_t const handle);
void dmg_audio_interrupt(dmg_handle_t const handle);
void dmg_audio_output(void *context, uint8_t *data, int length);
uint8_t dmg_audio_read(dmg_handle_t const handle, uint16_t address);
void dmg_audio_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_AUDIO_H_ */
