/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

dmg_error_e dmg_initialize(dmg_t *dmg, const dmg_data_t *const data, const dmg_output_f output)
{
    if (!dmg || (!*dmg && !(*dmg = calloc(1, sizeof (**dmg)))))
    {
        return DMG_FAILURE;
    }
    return dmg_system_initialize(*dmg, data, output);
}

dmg_error_e dmg_input(dmg_t const dmg, uint8_t input, uint8_t *output)
{
    if (!dmg)
    {
        return DMG_FAILURE;
    }
    return dmg_system_input(dmg, input, output);
}

dmg_error_e dmg_load(dmg_t const dmg, const dmg_data_t *const data)
{
    if (!dmg)
    {
        return DMG_FAILURE;
    }
    return dmg_system_load(dmg, data);
}

dmg_error_e dmg_run(dmg_t const dmg)
{
    if (!dmg)
    {
        return DMG_FAILURE;
    }
    return dmg_system_run(dmg);
}

dmg_error_e dmg_save(dmg_t const dmg, dmg_data_t *const data)
{
    if (!dmg)
    {
        return DMG_FAILURE;
    }
    return dmg_system_save(dmg, data);
}

void dmg_uninitialize(dmg_t *dmg)
{
    if (dmg && *dmg)
    {
        dmg_system_uninitialize(*dmg);
        free(*dmg);
        *dmg = NULL;
    }
}
