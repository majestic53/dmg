/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argument.h>
#include <file.h>
#include <socket.h>
#include <thread.h>

typedef struct
{
    dmg_handle_t handle;
    argument_t argument;
    file_t cartridge;
    socket_t sock;
} context_t;

static context_t g_context = {};
static thread_t g_thread = {};

static int input(void *context)
{
    /* TODO: HANDLE SERIAL INPUT */
    return EXIT_SUCCESS;
    /* ---- */
}

static uint8_t output(uint8_t value)
{
    /* TODO: HANDLE SERIAL OUTPUT */
    return 1;
    /* ---- */
}

static int initialize(int argc, char *argv[])
{
    int result;
    if ((result = argument_parse(argc, argv, &g_context.argument)) != EXIT_SUCCESS)
    {
        return result;
    }
    g_context.cartridge.path = g_context.argument.path;
    if ((result = file_load(&g_context.cartridge)) != EXIT_SUCCESS)
    {
        return result;
    }
    if (g_context.argument.link && (result = socket_open(&g_context.sock)) != EXIT_SUCCESS)
    {
        return result;
    }
    if (dmg_initialize(&g_context.handle, &g_context.cartridge.data, output) != DMG_SUCCESS)
    {
        fprintf(stderr, "%s\n", dmg_error(g_context.handle));
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
            fprintf(stderr, "%s\n", dmg_error(g_context.handle));
            result = EXIT_FAILURE;
        }
    }
    free(file.data.buffer);
    free(file.path);
    return result;
}

static int run(void)
{
    int result, thread_result = EXIT_SUCCESS;
    if (g_context.argument.link && ((result = thread_start(&g_thread, input, &g_context, false)) != EXIT_SUCCESS))
    {
        fprintf(stderr, "Failed to start thread\n");
        return result;
    }
    if (dmg_run(g_context.handle) != DMG_SUCCESS)
    {
        fprintf(stderr, "%s\n", dmg_error(g_context.handle));
        return EXIT_FAILURE;
    }
    if (g_context.argument.link && ((result = thread_wait(&g_thread, &thread_result)) != EXIT_SUCCESS))
    {
        fprintf(stderr, "Failed to wait for thread\n");
        return result;
    }
    return thread_result;
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
        fprintf(stderr, "%s\n", dmg_error(g_context.handle));
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
    if (g_context.argument.link)
    {
        socket_close(&g_context.sock);
    }
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
