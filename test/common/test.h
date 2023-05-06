/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_TEST_H_
#define DMG_TEST_H_

#include <stdio.h>
#include <common.h>

#define DMG_ASSERT(_CONDITION_) \
    if(!(_CONDITION_)) { \
        fprintf(stderr, "[\x1b[91mFAIL\x1b[0m] %s\n", __FUNCTION__); \
        fprintf(stderr, "--> \x1b[91m%s (%s@%u)\x1b[0m\n", #_CONDITION_, __FILE__, __LINE__); \
        return DMG_FAILURE; \
    }

#define DMG_PASS() \
    fprintf(stderr, "[\x1b[92mPASS\x1b[0m] %s\n", __FUNCTION__); \
    return DMG_SUCCESS

#define DMG_RUN(...) \
    int main(void) \
    { \
        dmg_error_e result = DMG_SUCCESS; \
        const dmg_test_f TEST[] = { __VA_ARGS__ }; \
        for(size_t test = 0; test < sizeof(TEST) / sizeof(*TEST); ++test) { \
            if(TEST[test]() == DMG_FAILURE) { \
                result = DMG_FAILURE; \
            } \
        } \
        return result; \
    }

#define DMG_TEST(_NAME_) \
    static dmg_error_e _NAME_(void)

typedef dmg_error_e (*dmg_test_f)(void);

#endif /* DMG_TEST_H_ */
