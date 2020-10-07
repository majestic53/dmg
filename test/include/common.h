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

#define ARRAY_LENGTH(_TYPE_, _ARRAY_) \
	(sizeof(_ARRAY_) / sizeof(_TYPE_))

#define ASSERT(_CONDITION_) \
	((_CONDITION_) ? EXIT_SUCCESS : EXIT_FAILURE)

#define ASSERT_FAILURE(_CONDITION_) \
	((_CONDITION_ == ERROR_FAILURE) ? EXIT_SUCCESS : EXIT_FAILURE)

#define ASSERT_SUCCESS(_CONDITION_) \
	((_CONDITION_ == ERROR_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE)

typedef int (*dmg_test_cb)(void);

#endif /* DMG_TEST_COMMON_H_ */
