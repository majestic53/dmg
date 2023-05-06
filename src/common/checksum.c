/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <common.h>

uint8_t dmg_get_checksum(const uint8_t *const data, uint16_t begin, uint16_t end)
{
    uint8_t result = 0;
    for (uint16_t index = begin; index <= end; ++index)
    {
        result = result - data[index] - 1;
    }
    return result;
}
