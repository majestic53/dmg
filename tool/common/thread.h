/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <common.h>

typedef struct
{
    thrd_t id;
    bool detached;
    void *context;
} thread_t;

int thread_create(thread_t *const thread, thrd_start_t start, void *context, bool detached);
int thread_join(thread_t *const thread, int *result);

#endif /* THREAD_H_ */
