/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <thread.h>

int thread_start(thread_t *const thread, thrd_start_t start, void *context, bool detached)
{
    if (thrd_create(&thread->id, start, context) != thrd_success)
    {
        return EXIT_FAILURE;
    }
    if (detached && (thrd_detach(thread->id) != thrd_success))
    {
        return EXIT_FAILURE;
    }
    thread->detached = detached;
    return EXIT_SUCCESS;
}

int thread_wait(thread_t *const thread, int *result)
{
    if (!thread->detached && (thrd_join(thread->id, result) != thrd_success))
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
