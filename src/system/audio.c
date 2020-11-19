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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "./audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NDEBUG

static void
dmg_audio_trace(
	__in int level,
	__in const dmg_audio_t *audio
	)
{
	// TODO: TRACE REGISTERS
}

#endif /* NDEBUG */

int
dmg_audio_export(
	__in const dmg_audio_t *audio,
	__in FILE *file
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Audio exporting");
	TRACE_AUDIO(LEVEL_VERBOSE, audio);

	// TODO: EXPORT REGISTERS/RAM TO FILE

	TRACE(LEVEL_INFORMATION, "Audio exported");

//exit:
	return result;
}

int
dmg_audio_import(
	__inout dmg_audio_t *audio,
	__in FILE *file
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Audio importing");

	// TODO: IMPORT REGISTERS/RAM FROM FILE

	TRACE_AUDIO(LEVEL_VERBOSE, audio);
	TRACE(LEVEL_INFORMATION, "Audio imported");

//exit:
	return result;
}

int
dmg_audio_load(
	__inout dmg_audio_t *audio,
	__in const dmg_t *configuration
	)
{
	int result = ERROR_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Audio loading");

	if(!configuration->bootrom.data) {

		// TODO: SET REGISTER POST VALUES
	}

	if((result = dmg_buffer_allocate(&audio->ram, RAM_WIDTH, 0)) != ERROR_SUCCESS) {
		goto exit;
	}

	TRACE_AUDIO(LEVEL_VERBOSE, audio);
	TRACE(LEVEL_INFORMATION, "Audio loaded");

exit:
	return result;
}

uint8_t
dmg_audio_read(
	__in const dmg_audio_t *audio,
	__in uint16_t address
	)
{
	uint8_t result = 0;

	switch(address) {
		case ADDRESS_AUDIO_CONTROL:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_1_SWEEP:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_1_LENGTH:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_1_ENVELOPE:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_1_FREQUENCY_HIGH:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_2_LENGTH:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_2_ENVELOPE:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_2_FREQUENCY_HIGH:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_3_ENABLE:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_3_LENGTH:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_3_LEVEL:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_3_FREQUENCY_HIGH:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_4_LENGTH:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_4_ENVELOPE:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_4_COUNTER_POLYNOMIAL:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_4_COUNTER_CONSECUTIVE:

			// TODO
			break;
		case ADDRESS_AUDIO_OUTPUT:

			// TODO
			break;
		case ADDRESS_AUDIO_RAM_BEGIN ... ADDRESS_AUDIO_RAM_END:
			result = ((uint8_t *)audio->ram.data)[address - ADDRESS_AUDIO_RAM_BEGIN];
			break;
		case ADDRESS_AUDIO_STATE:

			// TODO
			break;
		default:
			result = UINT8_MAX;
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported audio read [%04x]->%02x", address, result);
			break;
	}

	return result;
}

void
dmg_audio_step(
	__inout dmg_audio_t *audio,
	__in uint32_t cycle
	)
{

	for(uint32_t tick = 0; tick < cycle; tick += CYCLE) {

		// TODO: STEP SUBSYSTEM
	}
}

void
dmg_audio_unload(
	__inout dmg_audio_t *audio
	)
{
	TRACE(LEVEL_INFORMATION, "Audio unloading");
	dmg_buffer_free(&audio->ram);
	memset(audio, 0, sizeof(*audio));
	TRACE(LEVEL_INFORMATION, "Audio unloaded");
}

void
dmg_audio_write(
	__inout dmg_audio_t *audio,
	__in uint16_t address,
	__in uint8_t value
	)
{

	switch(address) {
		case ADDRESS_AUDIO_CONTROL:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_1_SWEEP:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_1_LENGTH:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_1_ENVELOPE:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_1_FREQUENCY_LOW:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_1_FREQUENCY_HIGH:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_2_LENGTH:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_2_ENVELOPE:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_2_FREQUENCY_LOW:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_2_FREQUENCY_HIGH:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_3_ENABLE:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_3_LENGTH:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_3_LEVEL:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_3_FREQUENCY_LOW:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_3_FREQUENCY_HIGH:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_4_LENGTH:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_4_ENVELOPE:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_4_COUNTER_POLYNOMIAL:

			// TODO
			break;
		case ADDRESS_AUDIO_MODE_4_COUNTER_CONSECUTIVE:

			// TODO
			break;
		case ADDRESS_AUDIO_OUTPUT:

			// TODO
			break;
		case ADDRESS_AUDIO_RAM_BEGIN ... ADDRESS_AUDIO_RAM_END:
			((uint8_t *)audio->ram.data)[address - ADDRESS_AUDIO_RAM_BEGIN] = value;
			break;
		case ADDRESS_AUDIO_STATE:

			// TODO
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported joypad write [%04x]<-%02x", address, value);
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
