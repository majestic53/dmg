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

#include "../../include/system/video.h"
#include "../../src/system/video_type.h"
#include "../include/common.h"

// TODO

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// TODO

int
dmg_test_video_load(void)
{
	int result = EXIT_SUCCESS;

	// TODO

	TRACE_TEST(result);

	return result;
}

int
dmg_test_video_read(void)
{
	int result = EXIT_SUCCESS;

	// TODO

	TRACE_TEST(result);

	return result;
}

int
dmg_test_video_step(void)
{
	int result = EXIT_SUCCESS;

	// TODO

	TRACE_TEST(result);

	return result;
}

int
dmg_test_video_unload(void)
{
	int result = EXIT_SUCCESS;

	// TODO

	TRACE_TEST(result);

	return result;
}

int
dmg_test_video_write(void)
{
	int result = EXIT_SUCCESS;

	// TODO

	TRACE_TEST(result);

	return result;
}

static const dmg_test_cb TEST[] = {
	dmg_test_video_load,
	dmg_test_video_read,
	dmg_test_video_step,
	dmg_test_video_unload,
	dmg_test_video_write,
	};

int
main(
	__in int argc,
	__in char *argv[]
	)
{
	int result = EXIT_SUCCESS;

	TEST_SETUP();

	for(size_t test = 0; test < TEST_COUNT(TEST); ++test) {

		if(TEST[test]() != EXIT_SUCCESS) {
			result = EXIT_FAILURE;
		}
	}

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
