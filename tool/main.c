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

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dmg.h>

typedef struct
{
    char *path;
} argument_t;

typedef struct
{
    char *path;
    dmg_data_t data;
} file_t;

static const struct option COMMAND[] =
{
    { "help", no_argument, NULL, 'h', },
    { "version", no_argument, NULL, 'v', },
    { NULL, 0, NULL, 0, },
};

static const char *DESCRIPTION[] =
{
    "Show help information",
    "Show version information",
};

static dmg_error_e file_load(file_t *const file)
{
    FILE *fp;
    long length;
    if (!(fp = fopen(file->path, "rb")))
    {
        fprintf(stderr, "File does not exist -- %s\n", file->path);
        return DMG_FAILURE;
    }
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (length <= 0)
    {
        fprintf(stderr, "Invalid file length -- %s\n", file->path);
        fclose(fp);
        return DMG_FAILURE;
    }
    if (!(file->data.buffer = calloc(length, sizeof(uint8_t))))
    {
        fprintf(stderr, "Failed to allocate read buffer\n");
        fclose(fp);
        return DMG_FAILURE;
    }
    file->data.length = length;
    if (fread(file->data.buffer, sizeof(uint8_t), file->data.length, fp) != file->data.length)
    {
        fprintf(stderr, "Failed to read file -- %s\n", file->path);
        fclose(fp);
        return DMG_FAILURE;
    }
    fclose(fp);
    return DMG_SUCCESS;
}

static dmg_error_e file_save(const file_t *const file)
{
    FILE *fp;
    if (!(fp = fopen(file->path, "wb")))
    {
        fprintf(stderr, "File does not exist -- %s\n", file->path);
        return DMG_FAILURE;
    }
    if (fwrite(file->data.buffer, sizeof(uint8_t), file->data.length, fp) != file->data.length)
    {
        fprintf(stderr, "Failed to write file -- %s\n", file->path);
        fclose(fp);
        return DMG_FAILURE;
    }
    fclose(fp);
    return DMG_SUCCESS;
}

static void usage(void)
{
    uint32_t index = 0;
    fprintf(stdout, "Usage: dmg [options] file\n\n");
    fprintf(stdout, "Options:\n");
    while (COMMAND[index].name)
    {
        char buffer[22] = {};
        snprintf(buffer, sizeof(buffer), "   -%c, --%s", COMMAND[index].val, COMMAND[index].name);
        for (uint32_t offset = strlen(buffer); offset < sizeof(buffer); ++offset)
        {
            buffer[offset] = (offset == (sizeof(buffer) - 1)) ? '\0' : ' ';
        }
        fprintf(stdout, "%s%s\n", buffer, DESCRIPTION[index]);
        ++index;
    }
}

static void version(void)
{
    const dmg_version_t *version = dmg_get_version();
    fprintf(stdout, "%u.%u-%x\n", version->major, version->minor, version->patch);
}

static dmg_error_e initialize(int argc, char *argv[], dmg_handle_t *handle, file_t *const file)
{
    int option, index;
    dmg_error_e result;
    argument_t argument = {};
    opterr = 1;
    while ((option = getopt_long(argc, argv, "hv", COMMAND, &index)) != -1)
    {
        switch (option)
        {
            case 'h': /* HELP */
                usage();
                return DMG_FAILURE;
            case 'v': /* VERSION */
                version();
                return DMG_FAILURE;
            case '?':
            default:
                return DMG_FAILURE;
        }
    }
    for (option = optind; option < argc; ++option)
    {
        if (argument.path)
        {
            fprintf(stderr, "Redefined file path -- %s\n", argv[option]);
            return DMG_FAILURE;
        }
        argument.path = argv[option];
    }
    if (!argument.path)
    {
        fprintf(stderr, "Undefined file path\n");
        return DMG_FAILURE;
    }
    file->path = argument.path;
    if ((result = file_load(file)) != DMG_SUCCESS)
    {
        return result;
    }
    if ((result = dmg_initialize(handle, &file->data)) != DMG_SUCCESS)
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
