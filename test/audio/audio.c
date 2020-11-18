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

#include "../../include/system/audio.h"
#include "../../src/system/audio_type.h"
#include "../include/common.h"

typedef struct {
	dmg_t configuration;
	dmg_audio_t audio;
} dmg_audio_test_t;

static dmg_audio_test_t g_audio = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_buffer_allocate(
	__inout dmg_buffer_t *buffer,
	__in uint32_t length,
	__in uint8_t value
	)
{
	return ERROR_SUCCESS;
}

void
dmg_buffer_free(
	__inout dmg_buffer_t *buffer
	)
{
	return;
}

int
dmg_test_audio_load(void)
{
	int result = EXIT_SUCCESS;

	// TODO
	(void)g_audio;
	// ---

	TRACE_TEST(result);

	return result;
}

int
dmg_test_audio_read(void)
{
	int result = EXIT_SUCCESS;

	// TODO
	(void)g_audio;
	// ---

	TRACE_TEST(result);

	return result;
}

int
dmg_test_audio_step(void)
{
	int result = EXIT_SUCCESS;

	// TODO
	(void)g_audio;
	// ---

	TRACE_TEST(result);

	return result;
}

int
dmg_test_audio_unload(void)
{
	int result = EXIT_SUCCESS;

	// TODO
	(void)g_audio;
	// ---

	TRACE_TEST(result);

	return result;
}

int
dmg_test_audio_write(void)
{
	int result = EXIT_SUCCESS;

	// TODO
	(void)g_audio;
	// ---

	TRACE_TEST(result);

	return result;
}

static const dmg_test TEST[] = {
	dmg_test_audio_load,
	dmg_test_audio_read,
	dmg_test_audio_step,
	dmg_test_audio_unload,
	dmg_test_audio_write,
	};

int
main(
	__in int argc,
	__in char *argv[]
	)
{
	int result = EXIT_SUCCESS;

	if(argc > 1) {
		TEST_SEED(strtol(argv[1], NULL, 16));
	} else {
		TEST_SEED(time(NULL));
	}

	for(size_t trial = 0; trial < TEST_TRIALS; ++trial) {
		TRACE_TEST_TRIAL(trial);

		for(size_t test = 0; test < TEST_COUNT(TEST); ++test) {

			if(TEST[test]() != EXIT_SUCCESS) {
				result = EXIT_FAILURE;
			}
		}
	}

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
