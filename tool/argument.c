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
#include <argument.h>

static const struct option COMMAND[] =
{
    { "help", no_argument, NULL, 'h', },
    { "link", no_argument, NULL, 'l', },
    { "palette", required_argument, NULL, 'p', },
    { "version", no_argument, NULL, 'v', },
    { NULL, 0, NULL, 0, },
};

static const char *DESCRIPTION[] =
{
    "Show help information",
    "Enable serial link",
    "Set color palette",
    "Show version information",
};

static const char *PALETTE[] =
{
    "dmg",
    "gbp",
};

static int argument_link(argument_t *const argument)
{
    if (argument->link)
    {
        fprintf(stderr, "Redefined serial link\n");
        return EXIT_FAILURE;
    }
    argument->link = true;
    return EXIT_SUCCESS;
}

static int argument_palette(argument_t *const argument)
{
    if (argument->palette < DMG_PALETTE_MAX)
    {
        fprintf(stderr, "Redefined color palette -- %s\n", optarg);
        return EXIT_FAILURE;
    }
    for (argument->palette = 0; argument->palette < DMG_PALETTE_MAX; ++argument->palette)
    {
        if (!strcmp(optarg, PALETTE[argument->palette]))
        {
            break;
        }
    }
    if (argument->palette == DMG_PALETTE_MAX)
    {
        fprintf(stderr, "Unsupported color palette -- %s\n", optarg);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static void argument_usage(void)
{
    uint32_t index = 0;
    fprintf(stdout, "Usage: dmg [options] file\n\n");
    fprintf(stdout, "Options:\n");
    while (COMMAND[index].name)
    {
        char buffer[22] = {};
        snprintf(buffer, sizeof (buffer), "   -%c, --%s", COMMAND[index].val, COMMAND[index].name);
        for (uint32_t offset = strlen(buffer); offset < sizeof (buffer); ++offset)
        {
            buffer[offset] = (offset == (sizeof (buffer) - 1)) ? '\0' : ' ';
        }
        fprintf(stdout, "%s%s\n", buffer, DESCRIPTION[index]);
        ++index;
    }
}

static void argument_version(void)
{
    const dmg_version_t *version = dmg_get_version();
    fprintf(stdout, "%u.%u-%x\n", version->major, version->minor, version->patch);
}

int argument_parse(int argc, char *argv[], argument_t *const argument)
{
    int option, index, result;
    opterr = 1;
    argument->palette = DMG_PALETTE_MAX;
    while ((option = getopt_long(argc, argv, "hlp:v", COMMAND, &index)) != -1)
    {
        switch (option)
        {
            case 'h': /* HELP */
                argument_usage();
                return EXIT_FAILURE;
            case 'l': /* LINK */
                if ((result = argument_link(argument)) != EXIT_SUCCESS)
                {
                    return result;
                }
                break;
            case 'p': /* PALETTE */
                if ((result = argument_palette(argument)) != EXIT_SUCCESS)
                {
                    return result;
                }
                break;
            case 'v': /* VERSION */
                argument_version();
                return EXIT_FAILURE;
            case '?':
            default:
                return EXIT_FAILURE;
        }
    }
    for (option = optind; option < argc; ++option)
    {
        if (argument->path)
        {
            fprintf(stderr, "Redefined file path -- %s\n", argv[option]);
            return EXIT_FAILURE;
        }
        argument->path = argv[option];
    }
    if (!argument->path)
    {
        fprintf(stderr, "Undefined file path\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
