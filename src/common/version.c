/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <common.h>

static const dmg_version_t VERSION =
{
    DMG_MAJOR, DMG_MINOR, DMG_PATCH
};

const dmg_version_t *dmg_version(void)
{
    return &VERSION;
}
