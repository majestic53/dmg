/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <common.h>

void *dmg_allocate(uint32_t length)
{
    return calloc(1, length);
}

void dmg_free(void *buffer)
{
    free(buffer);
}
