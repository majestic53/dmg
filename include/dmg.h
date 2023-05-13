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

#define DMG_API_VERSION DMG_API_VERSION_1
#define DMG_API_VERSION_1 1

typedef enum
{
    DMG_FAILURE = -1,
    DMG_SUCCESS,
} dmg_error_e;

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

typedef struct dmg_s *dmg_t;

typedef uint8_t (*dmg_output_f)(uint8_t value);

const char *dmg_error(dmg_t const dmg);
dmg_error_e dmg_initialize(dmg_t *dmg, const dmg_data_t *const data, const dmg_output_f output);
dmg_error_e dmg_input(dmg_t const dmg, uint8_t input, uint8_t *output);
dmg_error_e dmg_load(dmg_t const dmg, const dmg_data_t *const data);
dmg_error_e dmg_run(dmg_t const dmg);
dmg_error_e dmg_save(dmg_t const dmg, dmg_data_t *const data);
void dmg_uninitialize(dmg_t *dmg);
const dmg_version_t *dmg_version(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_H_ */
