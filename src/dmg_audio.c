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

static void dmg_audio_buffer_read(dmg_handle_t const handle, int16_t *buffer, uint32_t length)
{
    if (length)
    {
        for (uint32_t index = 0; index < length; ++index)
        {
            buffer[index] = handle->audio.buffer.sample[handle->audio.buffer.read];
            handle->audio.buffer.read = (handle->audio.buffer.read + 1) % 44100;
        }
        handle->audio.buffer.full = false;
    }
}

static uint32_t dmg_audio_buffer_readable(dmg_handle_t const handle)
{
    uint32_t result = 0;
    if (handle->audio.buffer.read < handle->audio.buffer.write)
    {
        result = handle->audio.buffer.write - handle->audio.buffer.read;
    }
    else if (handle->audio.buffer.read == handle->audio.buffer.write)
    {
        result = handle->audio.buffer.full ? 44100 : 0;
    }
    else
    {
        result = (44100 - handle->audio.buffer.read) + handle->audio.buffer.write;
    }
    return result;
}

static void dmg_audio_buffer_write(dmg_handle_t const handle, int16_t value)
{
    handle->audio.buffer.sample[handle->audio.buffer.write] = value;
    handle->audio.buffer.write = (handle->audio.buffer.write + 1) % 44100;
    handle->audio.buffer.full = (handle->audio.buffer.write == handle->audio.buffer.read);
}

static bool dmg_audio_buffer_writable(dmg_handle_t const handle)
{
    return !handle->audio.buffer.full;
}

static float dmg_audio_sample_channel_1(dmg_handle_t const handle)
{
    /* TODO */
    (void)handle;
    return 0.f;
    /* ---- */
}

static float dmg_audio_sample_channel_2(dmg_handle_t const handle)
{
    /* TODO */
    (void)handle;
    return 0.f;
    /* ---- */
}

static float dmg_audio_sample_channel_3(dmg_handle_t const handle)
{
    /* TODO */
    (void)handle;
    return 0.f;
    /* ---- */
}

static float dmg_audio_sample_channel_4(dmg_handle_t const handle)
{
    /* TODO */
    (void)handle;
    return 0.f;
    /* ---- */
}

static float dmg_audio_sample_mixer(dmg_handle_t const handle)
{
    uint8_t count = 0;
    float result = 0.f;
    if (handle->audio.control.channel_1_enabled && (handle->audio.mixer.right_channel_1 || handle->audio.mixer.left_channel_1))
    {
        result += dmg_audio_sample_channel_1(handle);
        ++count;
    }
    if (handle->audio.control.channel_2_enabled && (handle->audio.mixer.right_channel_2 || handle->audio.mixer.left_channel_2))
    {
        result += dmg_audio_sample_channel_2(handle);
        ++count;
    }
    if (handle->audio.control.channel_3_enabled && (handle->audio.mixer.right_channel_3 || handle->audio.mixer.left_channel_3))
    {
        result += dmg_audio_sample_channel_3(handle);
        ++count;
    }
    if (handle->audio.control.channel_4_enabled && (handle->audio.mixer.right_channel_4 || handle->audio.mixer.left_channel_4))
    {
        result += dmg_audio_sample_channel_4(handle);
        ++count;
    }
    if (count)
    {
        result /= (float)count;
    }
    return result;
}

static float dmg_audio_sample_volume(dmg_handle_t const handle)
{
    return (handle->audio.volume.right_volume + handle->audio.volume.left_volume + 2.f) / 16.f;
}

void dmg_audio_clock(dmg_handle_t const handle)
{
    /* TODO: HANDLE SYSTEM CLOCK */
    if (!handle->audio.delay)
    {
        if (handle->audio.control.enabled && dmg_audio_buffer_writable(handle))
        {
            float sample = dmg_audio_sample_mixer(handle) * dmg_audio_sample_volume(handle);
            dmg_audio_buffer_write(handle, sample * INT16_MAX);
        }
        handle->audio.delay = 96;
    }
    --handle->audio.delay;
}

void dmg_audio_interrupt(dmg_handle_t const handle)
{
    /* TODO: HANDLE TIMER CLOCK (APU DIV) */
    (void)handle;
    /* ---- */
}

uint8_t dmg_audio_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0xFF10: /* NR10 */
            result = handle->audio.channel_1.sweep.raw;
            break;
        case 0xFF11: /* NR11 */
            result = handle->audio.channel_1.length.raw & 0xC0;
            break;
        case 0xFF12: /* NR12 */
            result = handle->audio.channel_1.envelope.raw;
            break;
        case 0xFF14: /* NR14 */
            result = handle->audio.channel_1.frequency.high.raw & 0x40;
            break;
        case 0xFF16: /* NR21 */
            result = handle->audio.channel_2.length.raw & 0xC0;
            break;
        case 0xFF17: /* NR22 */
            result = handle->audio.channel_2.envelope.raw;
            break;
        case 0xFF19: /* NR24 */
            result = handle->audio.channel_2.frequency.high.raw & 0x40;
            break;
        case 0xFF1A: /* NR30 */
            result = handle->audio.channel_3.control.raw & 0x80;
            break;
        case 0xFF1B: /* NR31 */
            result = handle->audio.channel_3.length.raw;
            break;
        case 0xFF1C: /* NR32 */
            result = handle->audio.channel_3.wave.raw & 0x60;
            break;
        case 0xFF1E: /* NR34 */
            result = handle->audio.channel_3.frequency.high.raw & 0x40;
            break;
        case 0xFF20: /* NR41 */
            result = handle->audio.channel_4.length.raw;
            break;
        case 0xFF21: /* NR42 */
            result = handle->audio.channel_4.envelope.raw;
            break;
        case 0xFF22: /* NR43 */
            result = handle->audio.channel_4.divider.raw;
            break;
        case 0xFF23: /* NR44 */
            result = handle->audio.channel_4.counter.raw & 0x40;
            break;
        case 0xFF24: /* NR50 */
            result = handle->audio.volume.raw;
            break;
        case 0xFF25: /* NR51 */
            result = handle->audio.mixer.raw;
            break;
        case 0xFF26: /* NR52 */
            result = handle->audio.control.raw;
            break;
        case 0xFF30 ... 0xFF3F: /* WAVE RAM */
            result = handle->audio.ram[address - 0xFF30] & 0x0F;
            break;
        default:
            break;
    }
    return result;
}

void dmg_audio_update(void *context, uint8_t *data, int length)
{
    int16_t *buffer = (int16_t *)data;
    uint32_t buffer_length = length / sizeof(int16_t);
    dmg_handle_t const handle = context;
    memset(data, dmg_get_silence(handle), length);
    if (dmg_audio_buffer_readable(handle) >= buffer_length)
    {
        dmg_audio_buffer_read(handle, buffer, buffer_length);
    }
}

void dmg_audio_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0xFF10: /* NR10 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_1.sweep.raw = value;
            }
            break;
        case 0xFF11: /* NR11 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_1.length.raw = value;
            }
            break;
        case 0xFF12: /* NR12 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_1.envelope.raw = value;
            }
            break;
        case 0xFF13: /* NR13 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_1.frequency.low = value;
            }
            break;
        case 0xFF14: /* NR14 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_1.frequency.high.raw = value;
            }
            break;
        case 0xFF16: /* NR21 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_2.length.raw = value;
            }
            break;
        case 0xFF17: /* NR22 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_2.envelope.raw = value;
            }
            break;
        case 0xFF18: /* NR23 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_2.frequency.low = value;
            }
            break;
        case 0xFF19: /* NR24 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_2.frequency.high.raw = value;
            }
            break;
        case 0xFF1A: /* NR30 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_3.control.raw = value;
            }
            break;
        case 0xFF1B: /* NR31 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_3.length.raw = value;
            }
            break;
        case 0xFF1C: /* NR32 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_3.wave.raw = value;
            }
            break;
        case 0xFF1D: /* NR33 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_3.frequency.low = value;
            }
            break;
        case 0xFF1E: /* NR34 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_3.frequency.high.raw = value;
            }
            break;
        case 0xFF20: /* NR41 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_4.length.raw = value;
            }
            break;
        case 0xFF21: /* NR42 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_4.envelope.raw = value;
            }
            break;
        case 0xFF22: /* NR43 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_4.divider.raw = value;
            }
            break;
        case 0xFF23: /* NR44 */
            if (handle->audio.control.enabled)
            {
                handle->audio.channel_4.counter.raw = value;
            }
            break;
        case 0xFF24: /* NR50 */
            if (handle->audio.control.enabled)
            {
                handle->audio.volume.raw = value;
            }
            break;
        case 0xFF25: /* NR51 */
            if (handle->audio.control.enabled)
            {
                handle->audio.mixer.raw = value;
            }
            break;
        case 0xFF26: /* NR52 */
            handle->audio.control.raw = value & 0x80;
            if (!handle->audio.control.enabled)
            {
                memset(&handle->audio.channel_1, 0, sizeof(handle->audio.channel_1));
                memset(&handle->audio.channel_2, 0, sizeof(handle->audio.channel_2));
                memset(&handle->audio.channel_3, 0, sizeof(handle->audio.channel_3));
                memset(&handle->audio.channel_4, 0, sizeof(handle->audio.channel_4));
                handle->audio.control.raw = 0;
                handle->audio.mixer.raw = 0;
                handle->audio.volume.raw = 0;
            }
            break;
        case 0xFF30 ... 0xFF3F: /* WAVE RAM */
            handle->audio.ram[address - 0xFF30] = value & 0x0F;
            break;
        default:
            break;
    }
}
