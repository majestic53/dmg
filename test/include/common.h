/**
 * DMG
 * Copyright (C) 2020 David Jolly
 *
 * DMG is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DMG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DMG_TEST_COMMON_H_
#define DMG_TEST_COMMON_H_

#include "../../include/common.h"

#define ASSERT(_CONDITION_) \
	((_CONDITION_) ? EXIT_SUCCESS : EXIT_FAILURE)

#define ASSERT_FAILURE(_CONDITION_) \
	((_CONDITION_ == ERROR_FAILURE) ? EXIT_SUCCESS : EXIT_FAILURE)

#define ASSERT_SUCCESS(_CONDITION_) \
	((_CONDITION_ == ERROR_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE)

#define TEST_COUNT(_TESTS_) \
	(sizeof(_TESTS_) / sizeof(dmg_test_cb))

typedef int (*dmg_test_cb)(void);

#ifndef NDEBUG
static uint32_t g_cycle = 0;
#define TRACE_TEST(_RESULT_) { \
		TRACE_ENABLE(&g_cycle); \
		if(_RESULT_ != EXIT_SUCCESS) { \
			TRACE_FORMAT(LEVEL_ERROR, "Test %s -- FAIL", __FUNCTION__); \
		} else { \
			TRACE_FORMAT(LEVEL_INFORMATION, "Test %s -- PASS", __FUNCTION__); \
		} \
		TRACE_DISABLE(); \
	}
#else
#define TRACE_TEST(_RESULT_)
#endif /* NDEBUG */

#endif /* DMG_TEST_COMMON_H_ */
