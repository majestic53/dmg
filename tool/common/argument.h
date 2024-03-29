/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef ARGUMENT_H_
#define ARGUMENT_H_

#include <common.h>

typedef struct
{
    bool link;
    char *path;
} argument_t;

int argument_parse(int argc, char *argv[], argument_t *const argument);

#endif /* ARGUMENT_H_ */
