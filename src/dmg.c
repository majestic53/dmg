/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <system.h>

static dmg_error_e dmg_clock(dmg_handle_t const handle)
{
    dmg_error_e result;
    dmg_audio_clock(handle);
    dmg_serial_clock(handle);
    dmg_timer_clock(handle);
    if ((result = dmg_processor_clock(handle)) == DMG_SUCCESS)
    {
        result = dmg_video_clock(handle);
    }
    return result;
}

dmg_error_e dmg_initialize(dmg_handle_t *handle, const dmg_data_t *const data, const dmg_output_f output)
{
    dmg_error_e result;
    if (!handle || (!*handle && !(*handle = calloc(1, sizeof (**handle)))))
    {
        return DMG_FAILURE;
    }
    if ((*handle)->initialized)
    {
        return DMG_ERROR(*handle, "System reinitialized");
    }
    if ((result = dmg_memory_initialize(*handle, data)) != DMG_SUCCESS)
    {
        return result;
    }
    if ((result = dmg_serial_initialize(*handle, output)) != DMG_SUCCESS)
    {
        return result;
    }
    if ((result = dmg_service_initialize(*handle)) != DMG_SUCCESS)
    {
        return result;
    }
    dmg_audio_initialize(*handle);
    (*handle)->initialized = true;
    return result;
}

dmg_error_e dmg_input(dmg_handle_t const handle, uint8_t input, uint8_t *output)
{
    if (!handle)
    {
        return DMG_FAILURE;
    }
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    if (!output)
    {
        return DMG_ERROR(handle, "Invalid output -- %p", output);
    }
    *output = dmg_serial_input(handle, input);
    return DMG_SUCCESS;
}

dmg_error_e dmg_load(dmg_handle_t const handle, const dmg_data_t *const data)
{
    if (!handle)
    {
        return DMG_FAILURE;
    }
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    return dmg_cartridge_load(handle, data);
}

uint8_t dmg_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x8000 ... 0x9FFF: /* VIDEO */
        case 0xFE00 ... 0xFE9F:
        case 0xFF40 ... 0xFF4B:
            result = dmg_video_read(handle, address);
            break;
        case 0xFF00: /* INPUT */
            result = dmg_input_read(handle, address);
            break;
        case 0xFF01 ... 0xFF02: /* SERIAL */
            result = dmg_serial_read(handle, address);
            break;
        case 0xFF04 ... 0xFF07: /* TIMER */
            result = dmg_timer_read(handle, address);
            break;
        case 0xFF0F: /* PROCESSOR */
        case 0xFFFF:
            result = dmg_processor_read(handle, address);
            break;
        case 0xFF10 ... 0xFF14: /* AUDIO */
        case 0xFF16 ... 0xFF19:
        case 0xFF1A ... 0xFF1E:
        case 0xFF20 ... 0xFF26:
        case 0xFF30 ... 0xFF3F:
            result = dmg_audio_read(handle, address);
            break;
        default: /* MEMORY */
            result = dmg_memory_read(handle, address);
            break;
    }
    return result;
}

dmg_error_e dmg_run(dmg_handle_t const handle)
{
    if (!handle)
    {
        return DMG_FAILURE;
    }
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    while (dmg_service_poll(handle))
    {
        dmg_error_e result;
        while ((result = dmg_clock(handle)) != DMG_COMPLETE)
        {
            if (result == DMG_FAILURE)
            {
                return result;
            }
        }
        if ((result = dmg_service_sync(handle)) != DMG_SUCCESS)
        {
            return result;
        }
    }
    return DMG_SUCCESS;
}

dmg_error_e dmg_save(dmg_handle_t const handle, dmg_data_t *const data)
{
    if (!handle)
    {
        return DMG_FAILURE;
    }
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    return dmg_cartridge_save(handle, data);
}

void dmg_uninitialize(dmg_handle_t *handle)
{
    if (handle && *handle)
    {
        (*handle)->initialized = false;
        dmg_service_uninitialize(*handle);
        dmg_cartridge_uninitialize(*handle);
        free(*handle);
        *handle = NULL;
    }
}

void dmg_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0x8000 ... 0x9FFF: /* VIDEO */
        case 0xFE00 ... 0xFE9F:
        case 0xFF40 ... 0xFF4B:
            dmg_video_write(handle, address, value);
            break;
        case 0xFF00: /* INPUT */
            dmg_input_write(handle, address, value);
            break;
        case 0xFF01 ... 0xFF02: /* SERIAL */
            dmg_serial_write(handle, address, value);
            break;
        case 0xFF04 ... 0xFF07: /* TIMER */
            dmg_timer_write(handle, address, value);
            break;
        case 0xFF0F: /* PROCESSOR */
        case 0xFFFF:
            dmg_processor_write(handle, address, value);
            break;
        case 0xFF10 ... 0xFF14: /* AUDIO */
        case 0xFF16 ... 0xFF19:
        case 0xFF1A ... 0xFF1E:
        case 0xFF20 ... 0xFF26:
        case 0xFF30 ... 0xFF3F:
            dmg_audio_write(handle, address, value);
            break;
        default: /* MEMORY */
            dmg_memory_write(handle, address, value);
            break;
    }
}
