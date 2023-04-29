/*
 * DMG
 * Copyright (C) 2023 David Jolly
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef DMG_H_
#define DMG_H_

#include <stdint.h>

typedef enum
{
    DMG_FAILURE = -1,
    DMG_SUCCESS,
    DMG_COMPLETE,
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

typedef struct dmg_s *dmg_handle_t;

typedef uint8_t (*dmg_output_f)(uint8_t value);

const char *dmg_get_error(dmg_handle_t const handle);
const dmg_version_t *dmg_get_version(void);
dmg_error_e dmg_initialize(dmg_handle_t *handle, const dmg_data_t *const data, const dmg_output_f output);
dmg_error_e dmg_input(dmg_handle_t const handle, uint8_t input, uint8_t *output);
dmg_error_e dmg_load(dmg_handle_t const handle, const dmg_data_t *const data);
dmg_error_e dmg_run(dmg_handle_t const handle);
dmg_error_e dmg_save(dmg_handle_t const handle, dmg_data_t *const data);
void dmg_uninitialize(dmg_handle_t *handle);

#endif /* DMG_H_ */
