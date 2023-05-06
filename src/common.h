/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_COMMON_H_
#define DMG_COMMON_H_

#include <dmg.h>

#define DMG_MAJOR 0
#define DMG_MINOR 1
#define DMG_PATCH 0x4ebe098

#define DMG_ERROR(_HANDLE_, _FORMAT_, ...) \
    dmg_set_error(_HANDLE_, __FILE__, __LINE__, _FORMAT_, ##__VA_ARGS__)

void *dmg_allocate(uint32_t length);
void dmg_free(void *buffer);
uint8_t dmg_get_checksum(const uint8_t *const data, uint16_t begin, uint16_t end);
dmg_error_e dmg_set_error(dmg_handle_t const handle, const char *file, uint32_t line, const char *format, ...);

#endif /* DMG_COMMON_H_ */
