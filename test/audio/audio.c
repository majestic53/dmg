/**
 * DMG
 * Copyright (C) 2020-2021 David Jolly
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../../include/system/audio.h"
#include "../../src/system/audio_type.h"
#include "../include/common.h"

typedef struct {
	dmg_t configuration;
	dmg_audio_t audio;
	int export;
	FILE *export_file;
	const void *export_data;
	uint32_t export_length;
	int import;
	FILE *import_file;
	void *import_data;
	uint32_t import_length;
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
	return DMG_STATUS_SUCCESS;
}

void
dmg_buffer_free(
	__inout dmg_buffer_t *buffer
	)
{
	return;
}

int
dmg_service_export_data(
	__in FILE *file,
	__in const void *data,
	__in uint32_t length
	)
{
	g_audio.export_file = file;
	g_audio.export_data = data;
	g_audio.export_length = length;

	return g_audio.export;
}

int
dmg_service_import_data(
	__in FILE *file,
	__in void *data,
	__in uint32_t length
	)
{
	g_audio.import_file = file;
	g_audio.import_data = data;
	g_audio.import_length = length;

	return g_audio.import;
}

static void
dmg_test_audio_initialize(void)
{
	memset(&g_audio, 0, sizeof(g_audio));
}

int
dmg_test_audio_load(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_audio_initialize();

	if(ASSERT_SUCCESS(dmg_audio_load(&g_audio.audio, &g_audio.configuration)) != EXIT_SUCCESS) {
		result = EXIT_FAILURE;
	}

	if(ASSERT(g_audio.audio.control.raw == POST_CONTROL)
			|| ASSERT(g_audio.audio.mode_1.sweep.raw == POST_MODE_1_SWEEP)
			|| ASSERT(g_audio.audio.mode_1.length.raw == POST_MODE_1_LENGTH)
			|| ASSERT(g_audio.audio.mode_1.envelope.raw == POST_MODE_1_ENVELOPE)
			|| ASSERT(g_audio.audio.mode_1.frequency.low == 0)
			|| ASSERT(g_audio.audio.mode_1.frequency.high.raw == POST_MODE_1_FREQUENCY_HIGH)
			|| ASSERT(g_audio.audio.mode_2.length.raw == POST_MODE_2_LENGTH)
			|| ASSERT(g_audio.audio.mode_2.envelope.raw == POST_MODE_2_ENVELOPE)
			|| ASSERT(g_audio.audio.mode_2.frequency.low == 0)
			|| ASSERT(g_audio.audio.mode_2.frequency.high.raw == POST_MODE_2_FREQUENCY_HIGH)
			|| ASSERT(g_audio.audio.mode_3.enable.raw == POST_MODE_3_ENABLE)
			|| ASSERT(g_audio.audio.mode_3.length == POST_MODE_3_LENGTH)
			|| ASSERT(g_audio.audio.mode_3.level.raw == POST_MODE_3_LEVEL)
			|| ASSERT(g_audio.audio.mode_3.frequency.low == POST_MODE_3_FREQUENCY_LOW)
			|| ASSERT(g_audio.audio.mode_3.frequency.high.raw == 0)
			|| ASSERT(g_audio.audio.mode_4.length.raw == POST_MODE_4_LENGTH)
			|| ASSERT(g_audio.audio.mode_4.envelope.raw == POST_MODE_4_ENVELOPE)
			|| ASSERT(g_audio.audio.mode_4.polynomial.raw == POST_MODE_4_COUNTER_POLYNOMIAL)
			|| ASSERT(g_audio.audio.mode_4.consecutive.raw == POST_MODE_4_COUNTER_CONSECUTIVE)
			|| ASSERT(g_audio.audio.output.raw == POST_OUTPUT)
			|| ASSERT(g_audio.audio.state.raw == POST_STATE)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.configuration.bootrom.data = (void *)1;

	if(ASSERT_SUCCESS(dmg_audio_load(&g_audio.audio, &g_audio.configuration)) != EXIT_SUCCESS) {
		result = EXIT_FAILURE;
	}

	if(ASSERT(g_audio.audio.control.raw == 0)
			|| ASSERT(g_audio.audio.mode_1.sweep.raw == 0)
			|| ASSERT(g_audio.audio.mode_1.length.raw == 0)
			|| ASSERT(g_audio.audio.mode_1.envelope.raw == 0)
			|| ASSERT(g_audio.audio.mode_1.frequency.low == 0)
			|| ASSERT(g_audio.audio.mode_1.frequency.high.raw == 0)
			|| ASSERT(g_audio.audio.mode_2.length.raw == 0)
			|| ASSERT(g_audio.audio.mode_2.envelope.raw == 0)
			|| ASSERT(g_audio.audio.mode_2.frequency.low == 0)
			|| ASSERT(g_audio.audio.mode_2.frequency.high.raw == 0)
			|| ASSERT(g_audio.audio.mode_3.enable.raw == 0)
			|| ASSERT(g_audio.audio.mode_3.length == 0)
			|| ASSERT(g_audio.audio.mode_3.level.raw == 0)
			|| ASSERT(g_audio.audio.mode_3.frequency.low == 0)
			|| ASSERT(g_audio.audio.mode_3.frequency.high.raw == 0)
			|| ASSERT(g_audio.audio.mode_4.length.raw == 0)
			|| ASSERT(g_audio.audio.mode_4.envelope.raw == 0)
			|| ASSERT(g_audio.audio.mode_4.polynomial.raw == 0)
			|| ASSERT(g_audio.audio.mode_4.consecutive.raw == 0)
			|| ASSERT(g_audio.audio.output.raw == 0)
			|| ASSERT(g_audio.audio.state.raw == 0)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_audio_read(void)
{
	uint8_t value = rand();
	int result = EXIT_SUCCESS;

	dmg_test_audio_initialize();
	g_audio.audio.control.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_CONTROL) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_1.sweep.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_1_SWEEP) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_1.length.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_1_LENGTH) == (value & MODE_1_LENGTH_MASK))) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_1.envelope.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_1_ENVELOPE) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_1.frequency.low = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_1_FREQUENCY_LOW) == UINT8_MAX)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_1.frequency.high.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_1_FREQUENCY_HIGH) == (value & MODE_1_FREQUENCY_HIGH_MASK))) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_2.length.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_2_LENGTH) == (value & MODE_2_LENGTH_MASK))) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_2.envelope.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_2_ENVELOPE) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_2.frequency.low = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_2_FREQUENCY_LOW) == UINT8_MAX)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_2.frequency.high.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_2_FREQUENCY_HIGH) == (value & MODE_2_FREQUENCY_HIGH_MASK))) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_3.enable.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_3_ENABLE) == (value & MODE_3_ENABLE_MASK))) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_3.length = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_3_LENGTH) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_3.level.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_3_LEVEL) == (value & MODE_3_LEVEL_MASK))) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_3.frequency.low = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_3_FREQUENCY_LOW) == UINT8_MAX)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_3.frequency.high.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_3_FREQUENCY_HIGH) == (value & MODE_3_FREQUENCY_HIGH_MASK))) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_4.length.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_4_LENGTH) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_4.envelope.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_4_ENVELOPE) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_4.polynomial.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_4_COUNTER_POLYNOMIAL) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.mode_4.consecutive.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_MODE_4_COUNTER_CONSECUTIVE) == (value & MODE_4_CONSECUTIVE_MASK))) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.output.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_OUTPUT) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	g_audio.audio.state.raw = value;

	if(ASSERT(dmg_audio_read(&g_audio.audio, ADDRESS_AUDIO_STATE) == value)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_audio_step(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_audio_initialize();

	// TODO

	TRACE_TEST(result);

	return result;
}

int
dmg_test_audio_unload(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_audio_initialize();
	dmg_audio_load(&g_audio.audio, &g_audio.configuration);
	dmg_audio_unload(&g_audio.audio);

	if(ASSERT(g_audio.audio.ram.data == NULL)
			|| ASSERT(g_audio.audio.ram.length == 0)
			|| ASSERT(g_audio.audio.cycle == 0)
			|| ASSERT(g_audio.audio.control.raw == 0)
			|| ASSERT(g_audio.audio.mode_1.sweep.raw == 0)
			|| ASSERT(g_audio.audio.mode_1.length.raw == 0)
			|| ASSERT(g_audio.audio.mode_1.envelope.raw == 0)
			|| ASSERT(g_audio.audio.mode_1.frequency.low == 0)
			|| ASSERT(g_audio.audio.mode_1.frequency.high.raw == 0)
			|| ASSERT(g_audio.audio.mode_2.length.raw == 0)
			|| ASSERT(g_audio.audio.mode_2.envelope.raw == 0)
			|| ASSERT(g_audio.audio.mode_2.frequency.low == 0)
			|| ASSERT(g_audio.audio.mode_2.frequency.high.raw == 0)
			|| ASSERT(g_audio.audio.mode_3.enable.raw == 0)
			|| ASSERT(g_audio.audio.mode_3.length == 0)
			|| ASSERT(g_audio.audio.mode_3.level.raw == 0)
			|| ASSERT(g_audio.audio.mode_3.frequency.low == 0)
			|| ASSERT(g_audio.audio.mode_3.frequency.high.raw == 0)
			|| ASSERT(g_audio.audio.mode_4.length.raw == 0)
			|| ASSERT(g_audio.audio.mode_4.envelope.raw == 0)
			|| ASSERT(g_audio.audio.mode_4.polynomial.raw == 0)
			|| ASSERT(g_audio.audio.mode_4.consecutive.raw == 0)
			|| ASSERT(g_audio.audio.output.raw == 0)
			|| ASSERT(g_audio.audio.state.raw == 0)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_audio_write(void)
{
	uint8_t value = rand();
	int result = EXIT_SUCCESS;

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_CONTROL, value);

	if(ASSERT(g_audio.audio.control.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_1_SWEEP, value);

	if(ASSERT(g_audio.audio.mode_1.sweep.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_1_LENGTH, value);

	if(ASSERT(g_audio.audio.mode_1.length.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_1_ENVELOPE, value);

	if(ASSERT(g_audio.audio.mode_1.envelope.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_1_FREQUENCY_LOW, value);

	if(ASSERT(g_audio.audio.mode_1.frequency.low == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_1_FREQUENCY_HIGH, value);

	if(ASSERT(g_audio.audio.mode_1.frequency.high.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_2_LENGTH, value);

	if(ASSERT(g_audio.audio.mode_2.length.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_2_ENVELOPE, value);

	if(ASSERT(g_audio.audio.mode_2.envelope.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_2_FREQUENCY_LOW, value);

	if(ASSERT(g_audio.audio.mode_2.frequency.low == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_2_FREQUENCY_HIGH, value);

	if(ASSERT(g_audio.audio.mode_2.frequency.high.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_3_ENABLE, value);

	if(ASSERT(g_audio.audio.mode_3.enable.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_3_LENGTH, value);

	if(ASSERT(g_audio.audio.mode_3.length == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_3_LEVEL, value);

	if(ASSERT(g_audio.audio.mode_3.level.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_3_FREQUENCY_LOW, value);

	if(ASSERT(g_audio.audio.mode_3.frequency.low == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_3_FREQUENCY_HIGH, value);

	if(ASSERT(g_audio.audio.mode_3.frequency.high.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_4_LENGTH, value);

	if(ASSERT(g_audio.audio.mode_4.length.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_4_ENVELOPE, value);

	if(ASSERT(g_audio.audio.mode_4.envelope.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_4_COUNTER_POLYNOMIAL, value);

	if(ASSERT(g_audio.audio.mode_4.polynomial.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_MODE_4_COUNTER_CONSECUTIVE, value);

	if(ASSERT(g_audio.audio.mode_4.consecutive.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_OUTPUT, value);

	if(ASSERT(g_audio.audio.output.raw == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_audio_initialize();
	dmg_audio_write(&g_audio.audio, ADDRESS_AUDIO_STATE, value);

	if(ASSERT(g_audio.audio.state.raw == value)) {
		result = EXIT_FAILURE;
	}

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
