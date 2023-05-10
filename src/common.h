/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_COMMON_H_
#define DMG_COMMON_H_

#include <dmg.h>

#define DMG_MAJOR 0
#define DMG_MINOR 1
#define DMG_PATCH 0x47c1200

#define DMG_ERROR(_HANDLE_, _FORMAT_, ...) \
    dmg_error_set(_HANDLE_, __FILE__, __LINE__, _FORMAT_, ##__VA_ARGS__)

dmg_error_e dmg_error_set(dmg_handle_t const handle, const char *file, uint32_t line, const char *format, ...);

#endif /* DMG_COMMON_H_ */
