/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_H_
#define DMG_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef enum
{
    DMG_FAILURE = -1,
    DMG_SUCCESS,
    DMG_COMPLETE,
} dmg_error_e;

typedef enum
{
    DMG_PALETTE_GB = 0,
    DMG_PALETTE_GBP,
    DMG_PALETTE_MAX,
} dmg_palette_e;

typedef struct
{
    uint8_t *buffer;
    uint32_t length;
} dmg_data_t;

typedef struct
{
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
} dmg_version_t;

typedef struct dmg_s *dmg_handle_t;

typedef uint8_t (*dmg_output_f)(uint8_t value);

const char *dmg_get_error(dmg_handle_t const handle);
const dmg_version_t *dmg_get_version(void);
dmg_error_e dmg_initialize(dmg_handle_t *handle, const dmg_data_t *const data, const dmg_output_f output, dmg_palette_e palette);
dmg_error_e dmg_input(dmg_handle_t const handle, uint8_t input, uint8_t *output);
dmg_error_e dmg_load(dmg_handle_t const handle, const dmg_data_t *const data);
dmg_error_e dmg_run(dmg_handle_t const handle);
dmg_error_e dmg_save(dmg_handle_t const handle, dmg_data_t *const data);
void dmg_uninitialize(dmg_handle_t *handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_H_ */
