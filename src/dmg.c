/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

#define DMG_MAJOR 0
#define DMG_MINOR 1
#define DMG_PATCH 0x6268fd2

static const dmg_version_t VERSION =
{
    DMG_MAJOR, DMG_MINOR, DMG_PATCH
};

static dmg_error_t dmg_clock(dmg_handle_t const handle)
{
    dmg_error_t result;
    dmg_audio_clock(handle);
    dmg_serial_clock(handle);
    dmg_timer_clock(handle);
    if ((result = dmg_processor_clock(handle)) == DMG_SUCCESS)
    {
        result = dmg_video_clock(handle);
    }
    return result;
}

const char *dmg_error(dmg_handle_t const handle)
{
    if (!handle)
    {
        return "Invalid handle";
    }
    if (!strlen(handle->error))
    {
        return "No error";
    }
    return handle->error;
}

dmg_error_t dmg_initialize(dmg_handle_t *handle, const dmg_data_t *const data, const dmg_output_t output)
{
    dmg_error_t result;
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

dmg_error_t dmg_input(dmg_handle_t const handle, uint8_t input, uint8_t *output)
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

dmg_error_t dmg_load(dmg_handle_t const handle, const dmg_data_t *const data)
{
    if (!handle)
    {
        return DMG_FAILURE;
    }
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    return dmg_memory_load(handle, data);
}

dmg_error_t dmg_run(dmg_handle_t const handle)
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
        dmg_error_t result;
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

dmg_error_t dmg_save(dmg_handle_t const handle, dmg_data_t *const data)
{
    if (!handle)
    {
        return DMG_FAILURE;
    }
    if (!handle->initialized)
    {
        return DMG_ERROR(handle, "System uninitialized");
    }
    return dmg_memory_save(handle, data);
}

void dmg_uninitialize(dmg_handle_t *handle)
{
    if (handle && *handle)
    {
        (*handle)->initialized = false;
        dmg_service_uninitialize(*handle);
        dmg_memory_uninitialize(*handle);
        free(*handle);
        *handle = NULL;
    }
}

const dmg_version_t *dmg_version(void)
{
    return &VERSION;
}
