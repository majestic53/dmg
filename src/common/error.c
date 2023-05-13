/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

const char *dmg_error(dmg_t const dmg)
{
    if (!dmg)
    {
        return "Invalid dmg";
    }
    if (!strlen(dmg->error))
    {
        return "No error";
    }
    return dmg->error;
}

dmg_error_e dmg_error_set(dmg_t const dmg, const char *file, uint32_t line, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    vsnprintf(dmg->error, sizeof (dmg->error), format, arguments);
    va_end(arguments);
#ifndef NDEBUG
    snprintf(dmg->error + strlen(dmg->error), sizeof (dmg->error) - strlen(dmg->error), " (%s:%u)", file, line);
#endif /* NDEBUG */
    return DMG_FAILURE;
}
