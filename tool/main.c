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
#include <socket.h>

typedef struct
{
    dmg_handle_t handle;
    file_t cartridge;
    socket_t sock;
} context_t;

static context_t g_context = {};

static uint8_t serial_output(uint8_t value)
{
    return 1;
}

static int initialize(int argc, char *argv[])
{
    int result;
    argument_t argument = {};
    if ((result = argument_parse(argc, argv, &argument)) != EXIT_SUCCESS)
    {
        return result;
    }
    g_context.cartridge.path = argument.path;
    if ((result = file_load(&g_context.cartridge)) != EXIT_SUCCESS)
    {
        return result;
    }
    if ((result = socket_open(&g_context.sock)) != EXIT_SUCCESS)
    {
        return result;
    }
    if (dmg_initialize(&g_context.handle, &g_context.cartridge.data, serial_output, argument.palette) != DMG_SUCCESS)
    {
        fprintf(stderr, "%s\n", dmg_get_error(g_context.handle));
        return EXIT_FAILURE;
    }
    return result;
}

static int load(void)
{
    int result = EXIT_SUCCESS;
    file_t file = {};
    if (!(file.path = calloc(strlen(g_context.cartridge.path) + strlen(".sav") + 1, sizeof (uint8_t))))
    {
        fprintf(stderr, "Failed to allocate load buffer\n");
        return EXIT_FAILURE;
    }
    strcpy(file.path, g_context.cartridge.path);
    strcat(file.path, ".sav");
    if ((file_load(&file) == EXIT_SUCCESS) && file.data.buffer && file.data.length)
    {
        if (dmg_load(g_context.handle, &file.data) != DMG_SUCCESS)
        {
            fprintf(stderr, "%s\n", dmg_get_error(g_context.handle));
            result = EXIT_FAILURE;
        }
    }
    free(file.data.buffer);
    free(file.path);
    return result;
}

static int run(void)
{
    if (dmg_run(g_context.handle) != DMG_SUCCESS)
    {
        fprintf(stderr, "%s\n", dmg_get_error(g_context.handle));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static int save(void)
{
    int result = EXIT_SUCCESS;
    file_t file = {};
    if (!(file.path = calloc(strlen(g_context.cartridge.path) + strlen(".sav") + 1, sizeof (uint8_t))))
    {
        fprintf(stderr, "Failed to allocate save buffer\n");
        return EXIT_FAILURE;
    }
    strcpy(file.path, g_context.cartridge.path);
    strcat(file.path, ".sav");
    if (dmg_save(g_context.handle, &file.data) != DMG_SUCCESS)
    {
        fprintf(stderr, "%s\n", dmg_get_error(g_context.handle));
        result = EXIT_FAILURE;
    }
    else if (file.data.buffer && file.data.length)
    {
        result = file_save(&file);
    }
    free(file.path);
    return result;
}

static void uninitialize(void)
{
    dmg_uninitialize(&g_context.handle);
    socket_close(&g_context.sock);
    free(g_context.cartridge.data.buffer);
}

int main(int argc, char *argv[])
{
    int result;
    if ((result = initialize(argc, argv)) == EXIT_SUCCESS)
    {
        if ((result = load()) == EXIT_SUCCESS)
        {
            if ((result = run()) == EXIT_SUCCESS)
            {
                result = save();
            }
        }
    }
    uninitialize();
    return result;
}
