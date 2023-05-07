/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
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
    { "version", no_argument, NULL, 'v', },
    { NULL, 0, NULL, 0, },
};

static const char *DESCRIPTION[] =
{
    "Show help information",
    "Enable serial link",
    "Show version information",
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
    while ((option = getopt_long(argc, argv, "hlv", COMMAND, &index)) != -1)
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
