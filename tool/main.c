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

#include <argument.h>
#include <file.h>

static dmg_error_e initialize(int argc, char *argv[], dmg_handle_t *handle, file_t *const file)
{
    dmg_error_e result;
    argument_t argument = {};
    if ((result = argument_parse(argc, argv, &argument)) != DMG_SUCCESS)
    {
        return result;
    }
    file->path = argument.path;
    if ((result = file_load(file)) != DMG_SUCCESS)
    {
        return result;
    }
    if ((result = dmg_initialize(handle, &file->data, NULL, argument.palette)) != DMG_SUCCESS)
    {
        fprintf(stderr, "%s\n", dmg_get_error(*handle));
        return result;
    }
    return result;
}

static dmg_error_e load(dmg_handle_t const handle, const char *const path)
{
    dmg_error_e result = DMG_SUCCESS;
    file_t file = {};
    if (!(file.path = calloc(strlen(path) + strlen(".sav") + 1, sizeof(uint8_t))))
    {
        fprintf(stderr, "Failed to allocate load buffer\n");
        return DMG_FAILURE;
    }
    strcpy(file.path, path);
    strcat(file.path, ".sav");
    if ((file_load(&file) == DMG_SUCCESS) && file.data.buffer && file.data.length)
    {
        if ((result = dmg_load(handle, &file.data)) != DMG_SUCCESS)
        {
            fprintf(stderr, "%s\n", dmg_get_error(handle));
        }
    }
    free(file.data.buffer);
    free(file.path);
    return result;
}

static dmg_error_e run(dmg_handle_t const handle)
{
    dmg_error_e result;
    if ((result = dmg_run(handle)) != DMG_SUCCESS)
    {
        fprintf(stderr, "%s\n", dmg_get_error(handle));
    }
    return result;
}

static dmg_error_e save(dmg_handle_t const handle, const char *const path)
{
    int result = DMG_SUCCESS;
    file_t file = {};
    if (!(file.path = calloc(strlen(path) + strlen(".sav") + 1, sizeof(uint8_t))))
    {
        fprintf(stderr, "Failed to allocate save buffer\n");
        return DMG_FAILURE;
    }
    strcpy(file.path, path);
    strcat(file.path, ".sav");
    if ((result = dmg_save(handle, &file.data)) != DMG_SUCCESS)
    {
        fprintf(stderr, "%s\n", dmg_get_error(handle));
    }
    else if (file.data.buffer && file.data.length)
    {
        result = file_save(&file);
    }
    free(file.path);
    return result;
}

static void uninitialize(dmg_handle_t *handle, file_t *const file)
{
    dmg_uninitialize(handle);
    free(file->data.buffer);
}

int main(int argc, char *argv[])
{
    dmg_error_e result;
    dmg_handle_t handle = NULL;
    file_t cartridge = {};
    if ((result = initialize(argc, argv, &handle, &cartridge)) == DMG_SUCCESS)
    {
        if ((result = load(handle, cartridge.path)) == DMG_SUCCESS)
        {
            if ((result = run(handle)) == DMG_SUCCESS)
            {
                result = save(handle, cartridge.path);
            }
        }
    }
    uninitialize(&handle, &cartridge);
    return (result == DMG_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE;
}
