/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

static void dmg_audio_buffer_read(dmg_t const dmg, int16_t *buffer, uint32_t length)
{
    if (length)
    {
        for (uint32_t index = 0; index < length; ++index)
        {
            buffer[index] = dmg->audio.buffer.sample[dmg->audio.buffer.read];
            dmg->audio.buffer.read = (dmg->audio.buffer.read + 1) % 44100;
        }
        dmg->audio.buffer.full = false;
    }
}

static uint32_t dmg_audio_buffer_readable(dmg_t const dmg)
{
    uint32_t result = 0;
    if (dmg->audio.buffer.read < dmg->audio.buffer.write)
    {
        result = dmg->audio.buffer.write - dmg->audio.buffer.read;
    }
    else if (dmg->audio.buffer.read == dmg->audio.buffer.write)
    {
        result = dmg->audio.buffer.full ? 44100 : 0;
    }
    else
    {
        result = (44100 - dmg->audio.buffer.read) + dmg->audio.buffer.write;
    }
    return result;
}

static void dmg_audio_buffer_write(dmg_t const dmg, int16_t value)
{
    dmg->audio.buffer.sample[dmg->audio.buffer.write] = value;
    dmg->audio.buffer.write = (dmg->audio.buffer.write + 1) % 44100;
    dmg->audio.buffer.full = (dmg->audio.buffer.write == dmg->audio.buffer.read);
}

static bool dmg_audio_buffer_writable(dmg_t const dmg)
{
    return !dmg->audio.buffer.full;
}

static float dmg_audio_sample_channel_1(dmg_t const dmg)
{
    /* TODO: GENERATE CHANNEL 1 SAMPLE */
    return 0.f;
    /* ---- */
}

static float dmg_audio_sample_channel_2(dmg_t const dmg)
{
    /* TODO: GENERATE CHANNEL 2 SAMPLE */
    return 0.f;
    /* ---- */
}

static float dmg_audio_sample_channel_3(dmg_t const dmg)
{
    /* TODO: GENERATE CHANNEL 3 SAMPLE */
    return 0.f;
    /* ---- */
}

static float dmg_audio_sample_channel_4(dmg_t const dmg)
{
    /* TODO: GENERATE CHANNEL 4 SAMPLE */
    return 0.f;
    /* ---- */
}

static float dmg_audio_sample_mixer(dmg_t const dmg)
{
    uint8_t count = 0;
    float result = 0.f;
    if (dmg->audio.control.channel_1_enabled && (dmg->audio.mixer.right_channel_1 || dmg->audio.mixer.left_channel_1))
    {
        result += dmg_audio_sample_channel_1(dmg);
        ++count;
    }
    if (dmg->audio.control.channel_2_enabled && (dmg->audio.mixer.right_channel_2 || dmg->audio.mixer.left_channel_2))
    {
        result += dmg_audio_sample_channel_2(dmg);
        ++count;
    }
    if (dmg->audio.control.channel_3_enabled && (dmg->audio.mixer.right_channel_3 || dmg->audio.mixer.left_channel_3))
    {
        result += dmg_audio_sample_channel_3(dmg);
        ++count;
    }
    if (dmg->audio.control.channel_4_enabled && (dmg->audio.mixer.right_channel_4 || dmg->audio.mixer.left_channel_4))
    {
        result += dmg_audio_sample_channel_4(dmg);
        ++count;
    }
    if (count)
    {
        result /= (float)count;
    }
    return result;
}

static float dmg_audio_sample_volume(dmg_t const dmg)
{
    return (dmg->audio.volume.right_volume + dmg->audio.volume.left_volume + 2.f) / 16.f;
}

void dmg_audio_clock(dmg_t const dmg)
{
    if (!dmg->audio.delay)
    {
        if (dmg->audio.control.enabled && dmg_audio_buffer_writable(dmg))
        {
            float sample = dmg_audio_sample_mixer(dmg) * dmg_audio_sample_volume(dmg);
            dmg_audio_buffer_write(dmg, sample * INT16_MAX);
        }
        dmg->audio.delay = 96; /* 44.1KHz */
    }
    --dmg->audio.delay;
}

void dmg_audio_initialize(dmg_t const dmg)
{
    dmg->audio.silence = dmg_system_silence(dmg);
}

void dmg_audio_interrupt(dmg_t const dmg)
{
    if (!(dmg->audio.counter % 2))
    {
        /* TODO: STEP SOUND LENGTH */
    }
    if (!(dmg->audio.counter % 4))
    {
        /* TODO: STEP CHANNEL 1 FREQUENCY SWEEP */
    }
    if (!(dmg->audio.counter % 8))
    {
        /* TODO: STEP ENVELOPE SWEEP */
    }
    if (++dmg->audio.counter > 8)
    {
        dmg->audio.counter = 0;
    }
}

void dmg_audio_output(void *context, uint8_t *data, int length)
{
    int16_t *buffer = (int16_t *)data;
    uint32_t buffer_length = length / sizeof (int16_t);
    dmg_t const dmg = context;
    memset(data, dmg->audio.silence, length);
    if (dmg_audio_buffer_readable(dmg) >= buffer_length)
    {
        dmg_audio_buffer_read(dmg, buffer, buffer_length);
    }
}

uint8_t dmg_audio_read(dmg_t const dmg, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0xFF10: /* NR10 */
            result = dmg->audio.channel_1.sweep.raw;
            break;
        case 0xFF11: /* NR11 */
            result = dmg->audio.channel_1.length.raw & 0xC0;
            break;
        case 0xFF12: /* NR12 */
            result = dmg->audio.channel_1.envelope.raw;
            break;
        case 0xFF14: /* NR14 */
            result = dmg->audio.channel_1.high.raw & 0x40;
            break;
        case 0xFF16: /* NR21 */
            result = dmg->audio.channel_2.length.raw & 0xC0;
            break;
        case 0xFF17: /* NR22 */
            result = dmg->audio.channel_2.envelope.raw;
            break;
        case 0xFF19: /* NR24 */
            result = dmg->audio.channel_2.high.raw & 0x40;
            break;
        case 0xFF1A: /* NR30 */
            result = dmg->audio.channel_3.control.raw & 0x80;
            break;
        case 0xFF1B: /* NR31 */
            result = dmg->audio.channel_3.length.raw;
            break;
        case 0xFF1C: /* NR32 */
            result = dmg->audio.channel_3.wave.raw & 0x60;
            break;
        case 0xFF1E: /* NR34 */
            result = dmg->audio.channel_3.high.raw & 0x40;
            break;
        case 0xFF20: /* NR41 */
            result = dmg->audio.channel_4.length.raw;
            break;
        case 0xFF21: /* NR42 */
            result = dmg->audio.channel_4.envelope.raw;
            break;
        case 0xFF22: /* NR43 */
            result = dmg->audio.channel_4.divider.raw;
            break;
        case 0xFF23: /* NR44 */
            result = dmg->audio.channel_4.counter.raw & 0x40;
            break;
        case 0xFF24: /* NR50 */
            result = dmg->audio.volume.raw;
            break;
        case 0xFF25: /* NR51 */
            result = dmg->audio.mixer.raw;
            break;
        case 0xFF26: /* NR52 */
            result = dmg->audio.control.raw;
            break;
        case 0xFF30 ... 0xFF3F: /* WAVE RAM */
            result = dmg->audio.ram[address - 0xFF30] & 0x0F;
            break;
        default:
            break;
    }
    return result;
}

void dmg_audio_write(dmg_t const dmg, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0xFF10: /* NR10 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_1.sweep.raw = value;
            }
            break;
        case 0xFF11: /* NR11 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_1.length.raw = value;
            }
            break;
        case 0xFF12: /* NR12 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_1.envelope.raw = value;
            }
            break;
        case 0xFF13: /* NR13 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_1.low = value;
            }
            break;
        case 0xFF14: /* NR14 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_1.high.raw = value;
                if (dmg->audio.channel_1.high.triggered)
                {
                    dmg->audio.control.channel_1_enabled = true;
                }
            }
            break;
        case 0xFF16: /* NR21 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_2.length.raw = value;
            }
            break;
        case 0xFF17: /* NR22 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_2.envelope.raw = value;
            }
            break;
        case 0xFF18: /* NR23 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_2.low = value;
            }
            break;
        case 0xFF19: /* NR24 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_2.high.raw = value;
                if (dmg->audio.channel_2.high.triggered)
                {
                    dmg->audio.control.channel_2_enabled = true;
                }
            }
            break;
        case 0xFF1A: /* NR30 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_3.control.raw = value;
            }
            break;
        case 0xFF1B: /* NR31 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_3.length.raw = value;
            }
            break;
        case 0xFF1C: /* NR32 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_3.wave.raw = value;
            }
            break;
        case 0xFF1D: /* NR33 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_3.low = value;
            }
            break;
        case 0xFF1E: /* NR34 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_3.high.raw = value;
            }
            break;
        case 0xFF20: /* NR41 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_4.length.raw = value;
            }
            break;
        case 0xFF21: /* NR42 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_4.envelope.raw = value;
            }
            break;
        case 0xFF22: /* NR43 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_4.divider.raw = value;
            }
            break;
        case 0xFF23: /* NR44 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.channel_4.counter.raw = value;
            }
            break;
        case 0xFF24: /* NR50 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.volume.raw = value;
            }
            break;
        case 0xFF25: /* NR51 */
            if (dmg->audio.control.enabled)
            {
                dmg->audio.mixer.raw = value;
            }
            break;
        case 0xFF26: /* NR52 */
            dmg->audio.control.raw = value & 0x80;
            if (!dmg->audio.control.enabled)
            {
                memset(&dmg->audio.channel_1, 0, sizeof (dmg->audio.channel_1));
                memset(&dmg->audio.channel_2, 0, sizeof (dmg->audio.channel_2));
                memset(&dmg->audio.channel_3, 0, sizeof (dmg->audio.channel_3));
                memset(&dmg->audio.channel_4, 0, sizeof (dmg->audio.channel_4));
                dmg->audio.control.raw = 0;
                dmg->audio.mixer.raw = 0;
                dmg->audio.volume.raw = 0;
            }
            break;
        case 0xFF30 ... 0xFF3F: /* WAVE RAM */
            dmg->audio.ram[address - 0xFF30] = value & 0x0F;
            break;
        default:
            break;
    }
}
