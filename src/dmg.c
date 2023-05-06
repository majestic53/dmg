/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

dmg_error_e dmg_initialize(dmg_handle_t *handle, const dmg_data_t *const data, const dmg_output_f output, dmg_palette_e palette)
{
    if (!handle || (!*handle && !(*handle = dmg_allocate(sizeof (**handle)))))
    {
        return DMG_FAILURE;
    }
    return dmg_system_initialize(*handle, data, output, palette);
}

dmg_error_e dmg_input(dmg_handle_t const handle, uint8_t input, uint8_t *output)
{
    if (!handle)
    {
        return DMG_FAILURE;
    }
    return dmg_system_input(handle, input, output);
}

dmg_error_e dmg_load(dmg_handle_t const handle, const dmg_data_t *const data)
{
    if (!handle)
    {
        return DMG_FAILURE;
    }
    return dmg_system_load(handle, data);
}

dmg_error_e dmg_run(dmg_handle_t const handle)
{
    if (!handle)
    {
        return DMG_FAILURE;
    }
    return dmg_system_run(handle);
}

dmg_error_e dmg_save(dmg_handle_t const handle, dmg_data_t *const data)
{
    if (!handle)
    {
        return DMG_FAILURE;
    }
    return dmg_system_save(handle, data);
}

void dmg_uninitialize(dmg_handle_t *handle)
{
    if (handle && *handle)
    {
        dmg_system_uninitialize(*handle);
        dmg_free(*handle);
        *handle = NULL;
    }
}
