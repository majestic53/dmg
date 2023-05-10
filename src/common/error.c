/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

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

dmg_error_t dmg_error_set(dmg_handle_t const handle, const char *file, uint32_t line, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    vsnprintf(handle->error, sizeof (handle->error), format, arguments);
    va_end(arguments);
#ifndef NDEBUG
    snprintf(handle->error + strlen(handle->error), sizeof (handle->error) - strlen(handle->error), " (%s:%u)", file, line);
#endif /* NDEBUG */
    return DMG_FAILURE;
}
