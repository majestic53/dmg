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

#include <system.h>

static const dmg_version_t VERSION =
{
    DMG_MAJOR, DMG_MINOR, DMG_PATCH
};

const char *dmg_get_error(dmg_handle_t const handle)
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

const dmg_version_t *dmg_get_version(void)
{
    return &VERSION;
}

dmg_error_e dmg_initialize(dmg_handle_t *handle, const dmg_data_t *const data)
{
    if (!handle || (!*handle && !(*handle = calloc(1, sizeof(**handle)))))
    {
        return EXIT_FAILURE;
    }
    return dmg_system_initialize(*handle, data);
}

dmg_error_e dmg_load(dmg_handle_t const handle, const dmg_data_t *const data)
{
    if (!handle)
    {
        return EXIT_FAILURE;
    }
    return dmg_system_load(handle, data);
}

dmg_error_e dmg_run(dmg_handle_t const handle)
{
    if (!handle)
    {
        return EXIT_FAILURE;
    }
    return dmg_system_run(handle);
}

dmg_error_e dmg_save(dmg_handle_t const handle, dmg_data_t *const data)
{
    if (!handle)
    {
        return EXIT_FAILURE;
    }
    return dmg_system_save(handle, data);
}

dmg_error_e dmg_set_error(dmg_handle_t const handle, const char *file, uint32_t line, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    vsnprintf(handle->error, sizeof(handle->error), format, arguments);
    va_end(arguments);
    snprintf(handle->error + strlen(handle->error), sizeof(handle->error) - strlen(handle->error), " (%s:%u)", file, line);
    return DMG_FAILURE;
}

void dmg_uninitialize(dmg_handle_t *handle)
{
    if (handle && *handle)
    {
        dmg_system_uninitialize(*handle);
        free(*handle);
        *handle = NULL;
    }
}
