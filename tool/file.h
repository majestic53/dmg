/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef FILE_H_
#define FILE_H_

#include <dmg.h>

typedef struct
{
    char *path;
    dmg_data_t data;
} file_t;

int file_load(file_t *const file);
int file_save(const file_t *const file);

#endif /* FILE_H_ */
