/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef ARGUMENT_H_
#define ARGUMENT_H_

#include <stdbool.h>
#include <dmg.h>

typedef struct
{
    bool link;
    char *path;
    dmg_palette_e palette;
} argument_t;

int argument_parse(int argc, char *argv[], argument_t *const argument);

#endif /* ARGUMENT_H_ */
