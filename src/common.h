/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_COMMON_H_
#define DMG_COMMON_H_

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <dmg.h>

#define DMG_MAJOR 0
#define DMG_MINOR 1
#define DMG_PATCH 0xd963d00

#define DMG_ERROR(_DMG_, _FORMAT_, ...) \
    dmg_error_set(_DMG_, __FILE__, __LINE__, _FORMAT_, ##__VA_ARGS__)

dmg_error_e dmg_error_set(dmg_t const dmg, const char *file, uint32_t line, const char *format, ...);

#endif /* DMG_COMMON_H_ */
