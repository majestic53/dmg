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
	TRACE_FORMAT(level, "Audio control=%02x (1=%s:%u, 2=%s:%u)", audio->control.raw, audio->control.enable_1 ? "Enabled" : "Disabled",
			audio->control.level_1, audio->control.enable_2 ? "Enabled" : "Disabled", audio->control.level_2);
	TRACE_FORMAT(level, "Audio output=%02x (1=[%c%c%c%c], 2=[%c%c%c%c])", audio->output.raw, audio->output.output_1_1 ? '1' : '-',
			audio->output.output_2_1 ? '2' : '-', audio->output.output_3_1 ? '3' : '-', audio->output.output_4_1 ? '4' : '-',
			audio->output.output_1_2 ? '1' : '-', audio->output.output_2_2 ? '2' : '-', audio->output.output_3_2 ? '3' : '-',
			audio->output.output_4_2 ? '4' : '-');
	TRACE_FORMAT(level, "Audio state=%02x (%s [%c%c%c%c])", audio->state.raw, audio->state.enable, audio->state.enable_1, audio->state.enable_2,
			audio->state.enable_2, audio->state.enable_3, audio->state.enable_4);
	TRACE_FORMAT(level, "Audio mode 1 sweep=%02x (s=%u, i=%u, t=%u)", audio->mode_1.sweep.raw, audio->mode_1.sweep.shift, audio->mode_1.sweep.increase,
			audio->mode_1.sweep.time);
	TRACE_FORMAT(level, "Audio mode 1 length=%02x (l=%u, d=%u)", audio->mode_1.length.raw, audio->mode_1.length.length, audio->mode_1.length.duty);
	TRACE_FORMAT(level, "Audio mode 1 envelope=%02x (s=%u, u=%u, v=%u)", audio->mode_1.envelope.raw, audio->mode_1.envelope.sweep, audio->mode_1.envelope.up,
			audio->mode_1.envelope.volume);
	TRACE_FORMAT(level, "Audio mode 1 frequency=%04x (%u, %u:[s=%x, r=%x])", audio->mode_1.frequency.raw, audio->mode_1.frequency.low,
			audio->mode_1.frequency.high.high, audio->mode_1.frequency.high.select, audio->mode_1.frequency.high.restart);
	TRACE_FORMAT(level, "Audio mode 2 length=%02x (l=%u, d=%u)", audio->mode_2.length.raw, audio->mode_2.length.length, audio->mode_2.length.duty);
	TRACE_FORMAT(level, "Audio mode 2 envelope=%02x (s=%u, u=%u, v=%u)", audio->mode_2.envelope.raw, audio->mode_2.envelope.sweep, audio->mode_2.envelope.up,
			audio->mode_2.envelope.volume);
	TRACE_FORMAT(level, "Audio mode 2 frequency=%04x (%u, %u:[s=%x, r=%x])", audio->mode_2.frequency.raw, audio->mode_2.frequency.low,
			audio->mode_2.frequency.high.high, audio->mode_2.frequency.high.select, audio->mode_2.frequency.high.restart);
	TRACE_FORMAT(level, "Audio mode 3 enable=%02x (%s)", audio->mode_3.enable.raw, audio->mode_3.enable.enable ? "Enabled" : "Disabled");
	TRACE_FORMAT(level, "Audio mode 3 length=%02x", audio->mode_3.length);
	TRACE_FORMAT(level, "Audio mode 3 level=%02x (o=%u)", audio->mode_3.level.raw, audio->mode_3.level.output);
	TRACE_FORMAT(level, "Audio mode 3 frequency=%04x (%u, %u:[s=%x, r=%x])", audio->mode_3.frequency.raw, audio->mode_3.frequency.low,
			audio->mode_3.frequency.high.high, audio->mode_3.frequency.high.select, audio->mode_3.frequency.high.restart);
	TRACE_FORMAT(level, "Audio mode 4 length=%02x", audio->mode_4.length.length);
	TRACE_FORMAT(level, "Audio mode 4 envelope=%02x (s=%u, u=%u, v=%u)", audio->mode_4.envelope.raw, audio->mode_4.envelope.sweep, audio->mode_4.envelope.up,
			audio->mode_4.envelope.volume);
	TRACE_FORMAT(level, "Audio mode 4 polynomial-counter=%02x (r=%u, s=%x, sh=%u)", audio->mode_4.polynomial.raw, audio->mode_4.polynomial.ratio,
			audio->mode_4.polynomial.select, audio->mode_4.polynomial.shift);
	TRACE_FORMAT(level, "Audio mode 4 consecutive-counter=%02x (s=%x, r=%x)", audio->mode_4.consecutive.raw, audio->mode_4.consecutive.select,
			audio->mode_4.consecutive.restart);
}

#endif /* NDEBUG */

int
dmg_audio_export(
	__in const dmg_audio_t *audio,
	__in FILE *file
	)
{
	int result = DMG_STATUS_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Audio exporting");
	TRACE_AUDIO(LEVEL_VERBOSE, audio);

	if((result = dmg_service_export_data(file, &audio->cycle, sizeof(audio->cycle))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	for(uint32_t address = 0; address < audio->ram.length; ++address) {

		if((result = dmg_service_export_data(file, &((uint8_t *)audio->ram.data)[address], sizeof(uint8_t))) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	if((result = dmg_service_export_data(file, &audio->control, sizeof(audio->control))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &audio->mode_1, sizeof(audio->mode_1))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &audio->mode_2, sizeof(audio->mode_2))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &audio->mode_3, sizeof(audio->mode_3))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &audio->mode_4, sizeof(audio->mode_4))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &audio->output, sizeof(audio->output))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_export_data(file, &audio->state, sizeof(audio->state))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE(LEVEL_INFORMATION, "Audio exported");

exit:
	return result;
}

int
dmg_audio_import(
	__inout dmg_audio_t *audio,
	__in FILE *file
	)
{
	int result = DMG_STATUS_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Audio importing");

	if((result = dmg_service_import_data(file, &audio->cycle, sizeof(audio->cycle))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	for(uint32_t address = 0; address < audio->ram.length; ++address) {

		if((result = dmg_service_import_data(file, &((uint8_t *)audio->ram.data)[address], sizeof(uint8_t))) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	if((result = dmg_service_import_data(file, &audio->control, sizeof(audio->control))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &audio->mode_1, sizeof(audio->mode_1))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &audio->mode_2, sizeof(audio->mode_2))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &audio->mode_3, sizeof(audio->mode_3))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &audio->mode_4, sizeof(audio->mode_4))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &audio->output, sizeof(audio->output))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_service_import_data(file, &audio->state, sizeof(audio->state))) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE_AUDIO(LEVEL_VERBOSE, audio);
	TRACE(LEVEL_INFORMATION, "Audio imported");

exit:
	return result;
}

int
dmg_audio_load(
	__inout dmg_audio_t *audio,
	__in const dmg_t *configuration
	)
{
	int result = DMG_STATUS_SUCCESS;

	TRACE(LEVEL_INFORMATION, "Audio loading");

	if(!configuration->bootrom.data) {
		audio->control.raw = POST_CONTROL;
		audio->mode_1.sweep.raw = POST_MODE_1_SWEEP;
		audio->mode_1.length.raw = POST_MODE_1_LENGTH;
		audio->mode_1.envelope.raw = POST_MODE_1_ENVELOPE;
		audio->mode_1.frequency.high.raw = POST_MODE_1_FREQUENCY_HIGH;
		audio->mode_2.length.raw = POST_MODE_2_LENGTH;
		audio->mode_2.envelope.raw = POST_MODE_2_ENVELOPE;
		audio->mode_2.frequency.high.raw = POST_MODE_2_FREQUENCY_HIGH;
		audio->mode_3.enable.raw = POST_MODE_3_ENABLE;
		audio->mode_3.length = POST_MODE_3_LENGTH;
		audio->mode_3.level.raw = POST_MODE_3_LEVEL;
		audio->mode_3.frequency.low = POST_MODE_3_FREQUENCY_LOW;
		audio->mode_4.length.raw = POST_MODE_4_LENGTH;
		audio->mode_4.envelope.raw = POST_MODE_4_ENVELOPE;
		audio->mode_4.polynomial.raw = POST_MODE_4_COUNTER_POLYNOMIAL;
		audio->mode_4.consecutive.raw = POST_MODE_4_COUNTER_CONSECUTIVE;
		audio->output.raw = POST_OUTPUT;
		audio->state.raw = POST_STATE;
	}

	if((result = dmg_buffer_allocate(&audio->ram, RAM_WIDTH, 0)) != DMG_STATUS_SUCCESS) {
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
			result = audio->control.raw;
			break;
		case ADDRESS_AUDIO_MODE_1_SWEEP:
			result = audio->mode_1.sweep.raw;
			break;
		case ADDRESS_AUDIO_MODE_1_LENGTH:
			result = (audio->mode_1.length.raw & MODE_1_LENGTH_MASK);
			break;
		case ADDRESS_AUDIO_MODE_1_ENVELOPE:
			result = audio->mode_1.envelope.raw;
			break;
		case ADDRESS_AUDIO_MODE_1_FREQUENCY_HIGH:
			result = (audio->mode_1.frequency.high.raw & MODE_1_FREQUENCY_HIGH_MASK);
			break;
		case ADDRESS_AUDIO_MODE_2_LENGTH:
			result = (audio->mode_2.length.raw & MODE_2_LENGTH_MASK);
			break;
		case ADDRESS_AUDIO_MODE_2_ENVELOPE:
			result = audio->mode_2.envelope.raw;
			break;
		case ADDRESS_AUDIO_MODE_2_FREQUENCY_HIGH:
			result = (audio->mode_2.frequency.high.raw & MODE_2_FREQUENCY_HIGH_MASK);
			break;
		case ADDRESS_AUDIO_MODE_3_ENABLE:
			result = (audio->mode_3.enable.raw & MODE_3_ENABLE_MASK);
			break;
		case ADDRESS_AUDIO_MODE_3_LENGTH:
			result = audio->mode_3.length;
			break;
		case ADDRESS_AUDIO_MODE_3_LEVEL:
			result = (audio->mode_3.level.raw & MODE_3_LEVEL_MASK);
			break;
		case ADDRESS_AUDIO_MODE_3_FREQUENCY_HIGH:
			result = (audio->mode_3.frequency.high.raw & MODE_3_FREQUENCY_HIGH_MASK);
			break;
		case ADDRESS_AUDIO_MODE_4_LENGTH:
			result = audio->mode_4.length.raw;
			break;
		case ADDRESS_AUDIO_MODE_4_ENVELOPE:
			result = audio->mode_4.envelope.raw;
			break;
		case ADDRESS_AUDIO_MODE_4_COUNTER_POLYNOMIAL:
			result = audio->mode_4.polynomial.raw;
			break;
		case ADDRESS_AUDIO_MODE_4_COUNTER_CONSECUTIVE:
			result = (audio->mode_4.consecutive.raw & MODE_4_CONSECUTIVE_MASK);
			break;
		case ADDRESS_AUDIO_OUTPUT:
			result = audio->output.raw;
			break;
		case ADDRESS_AUDIO_RAM_BEGIN ... ADDRESS_AUDIO_RAM_END:
			result = ((uint8_t *)audio->ram.data)[address - ADDRESS_AUDIO_RAM_BEGIN];
			break;
		case ADDRESS_AUDIO_STATE:
			result = audio->state.raw;
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

	if(audio->state.enable) {

		for(uint32_t tick = 0; tick < cycle; tick += CYCLE) {

			// TODO: STEP SUBSYSTEM
		}
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
			audio->control.raw = value;
			break;
		case ADDRESS_AUDIO_MODE_1_SWEEP:
			audio->mode_1.sweep.raw = value;
			break;
		case ADDRESS_AUDIO_MODE_1_LENGTH:
			audio->mode_1.length.raw = value;
			break;
		case ADDRESS_AUDIO_MODE_1_ENVELOPE:
			audio->mode_1.envelope.raw = value;
			break;
		case ADDRESS_AUDIO_MODE_1_FREQUENCY_LOW:
			audio->mode_1.frequency.low = value;
			break;
		case ADDRESS_AUDIO_MODE_1_FREQUENCY_HIGH:
			audio->mode_1.frequency.high.raw = value;
			break;
		case ADDRESS_AUDIO_MODE_2_LENGTH:
			audio->mode_2.length.raw = value;
			break;
		case ADDRESS_AUDIO_MODE_2_ENVELOPE:
			audio->mode_2.envelope.raw = value;
			break;
		case ADDRESS_AUDIO_MODE_2_FREQUENCY_LOW:
			audio->mode_2.frequency.low = value;
			break;
		case ADDRESS_AUDIO_MODE_2_FREQUENCY_HIGH:
			audio->mode_2.frequency.high.raw = value;
			break;
		case ADDRESS_AUDIO_MODE_3_ENABLE:
			audio->mode_3.enable.raw = value;
			break;
		case ADDRESS_AUDIO_MODE_3_LENGTH:
			audio->mode_3.length = value;
			break;
		case ADDRESS_AUDIO_MODE_3_LEVEL:
			audio->mode_3.level.raw = value;
			break;
		case ADDRESS_AUDIO_MODE_3_FREQUENCY_LOW:
			audio->mode_3.frequency.low = value;
			break;
		case ADDRESS_AUDIO_MODE_3_FREQUENCY_HIGH:
			audio->mode_3.frequency.high.raw = value;
			break;
		case ADDRESS_AUDIO_MODE_4_LENGTH:
			audio->mode_4.length.raw = value;
			break;
		case ADDRESS_AUDIO_MODE_4_ENVELOPE:
			audio->mode_4.envelope.raw = value;
			break;
		case ADDRESS_AUDIO_MODE_4_COUNTER_POLYNOMIAL:
			audio->mode_4.polynomial.raw = value;
			break;
		case ADDRESS_AUDIO_MODE_4_COUNTER_CONSECUTIVE:
			audio->mode_4.consecutive.raw = value;
			break;
		case ADDRESS_AUDIO_OUTPUT:
			audio->output.raw = value;
			break;
		case ADDRESS_AUDIO_RAM_BEGIN ... ADDRESS_AUDIO_RAM_END:
			((uint8_t *)audio->ram.data)[address - ADDRESS_AUDIO_RAM_BEGIN] = value;
			break;
		case ADDRESS_AUDIO_STATE:
			audio->state.raw = value;
			break;
		default:
			TRACE_FORMAT(LEVEL_WARNING, "Unsupported joypad write [%04x]<-%02x", address, value);
			break;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
