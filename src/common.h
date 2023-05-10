/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_COMMON_H_
#define DMG_COMMON_H_

#include <dmg.h>

#define DMG_MAJOR 0
#define DMG_MINOR 1
#define DMG_PATCH 0x8e410c7

#define DMG_ERROR(_HANDLE_, _FORMAT_, ...) \
    dmg_error_set(_HANDLE_, __FILE__, __LINE__, _FORMAT_, ##__VA_ARGS__)

dmg_error_e dmg_error_set(dmg_handle_t const handle, const char *file, uint32_t line, const char *format, ...);
uint8_t dmg_read(dmg_handle_t const handle, uint16_t address);
void dmg_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_COMMON_H_ */
