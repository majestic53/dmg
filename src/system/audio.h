/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_AUDIO_H_
#define DMG_AUDIO_H_

#include <common.h>

typedef union
{
    struct
    {
        uint8_t slope : 3;
        uint8_t decreasing : 1;
        uint8_t pace : 3;
    };
    uint8_t raw;
} dmg_audio_nr10_t;

typedef union
{
    struct
    {
        uint8_t timer : 6;
        uint8_t duty : 2;
    };
    uint8_t raw;
} dmg_audio_nr11_t;

typedef union
{
    struct
    {
        uint8_t pace : 3;
        uint8_t decreasing : 1;
        uint8_t volume : 4;
    };
    uint8_t raw;
} dmg_audio_nr12_t;

typedef union
{
    struct
    {
        uint8_t : 6;
        uint8_t enabled : 1;
        uint8_t triggered : 1;
    };
    uint8_t raw;
} dmg_audio_nr14_t;

typedef union
{
    struct
    {
        uint8_t timer : 6;
        uint8_t duty : 2;
    };
    uint8_t raw;
} dmg_audio_nr21_t;

typedef union
{
    struct
    {
        uint8_t pace : 3;
        uint8_t decreasing : 1;
        uint8_t volume : 4;
    };
    uint8_t raw;
} dmg_audio_nr22_t;

typedef union
{
    struct
    {
        uint8_t : 6;
        uint8_t enabled : 1;
        uint8_t triggered : 1;
    };
    uint8_t raw;
} dmg_audio_nr24_t;

typedef union
{
    struct
    {
        /* TODO: POPULATE NR30 BITFIELD */
    };
    uint8_t raw;
} dmg_audio_nr30_t;

typedef union
{
    struct
    {
        /* TODO: POPULATE NR31 BITFIELD */
    };
    uint8_t raw;
} dmg_audio_nr31_t;

typedef union
{
    struct
    {
        /* TODO: POPULATE NR32 BITFIELD */
    };
    uint8_t raw;
} dmg_audio_nr32_t;

typedef union
{
    struct
    {
        /* TODO: POPULATE NR34 BITFIELD */
    };
    uint8_t raw;
} dmg_audio_nr34_t;

typedef union
{
    struct
    {
        /* TODO: POPULATE NR41 BITFIELD */
    };
    uint8_t raw;
} dmg_audio_nr41_t;

typedef union
{
    struct
    {
        /* TODO: POPULATE NR42 BITFIELD */
    };
    uint8_t raw;
} dmg_audio_nr42_t;

typedef union
{
    struct
    {
        /* TODO: POPULATE NR43 BITFIELD */
    };
    uint8_t raw;
} dmg_audio_nr43_t;

typedef union
{
    struct
    {
        /* TODO: POPULATE NR44 BITFIELD */
    };
    uint8_t raw;
} dmg_audio_nr44_t;

typedef struct
{
    uint8_t counter;
    uint8_t delay;
    uint8_t silence;
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
        dmg_audio_nr10_t sweep;
        dmg_audio_nr11_t length;
        dmg_audio_nr12_t envelope;
        union {
            struct {
                uint8_t low;
                dmg_audio_nr14_t high;
            };
            uint16_t frequency : 11;
        };
    } channel_1;
    struct
    {
        dmg_audio_nr21_t length;
        dmg_audio_nr22_t envelope;
        union {
            struct {
                uint8_t low;
                dmg_audio_nr24_t high;
            };
            uint16_t frequency : 11;
        };
    } channel_2;
    struct
    {
        dmg_audio_nr30_t control;
        dmg_audio_nr31_t length;
        dmg_audio_nr32_t wave;
        union {
            struct {
                uint8_t low;
                dmg_audio_nr34_t high;
            };
            uint16_t frequency : 11;
        };
    } channel_3;
    struct
    {
        dmg_audio_nr41_t length;
        dmg_audio_nr42_t envelope;
        dmg_audio_nr43_t divider;
        dmg_audio_nr44_t counter;
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

void dmg_audio_clock(dmg_t const dmg);
void dmg_audio_initialize(dmg_t const dmg);
void dmg_audio_interrupt(dmg_t const dmg);
void dmg_audio_output(void *context, uint8_t *data, int length);
uint8_t dmg_audio_read(dmg_t const dmg, uint16_t address);
void dmg_audio_write(dmg_t const dmg, uint16_t address, uint8_t value);

#endif /* DMG_AUDIO_H_ */
