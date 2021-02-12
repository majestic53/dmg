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

#include "./action_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_action_cycle(
	__in const dmg_action_t *request,
	__in dmg_action_t *response,
	__in dmg_runtime_t *runtime
	)
{
	response->data.dword = runtime->cycle;
	response->length = sizeof(request->data.dword);

	return DMG_STATUS_SUCCESS;
}

static int
dmg_action_nop(
	__in const dmg_action_t *request,
	__in dmg_action_t *response,
	__in dmg_runtime_t *runtime
	)
{
	return DMG_STATUS_SUCCESS;
}

static int
dmg_action_read(
	__in const dmg_action_t *request,
	__in dmg_action_t *response,
	__in dmg_runtime_t *runtime
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(request->data.dword > UINT16_MAX) {

		switch(request->address) {
			case DMG_REGISTER_PROCESSOR_A ... DMG_REGISTER_PROCESSOR_STOP:
				dmg_processor_read_register(&runtime->processor, request, response);
				break;
			default:
				TRACE_FORMAT(LEVEL_WARNING, "Unsupported register read %04x", request->address);
				result = DMG_STATUS_INVALID;
				goto exit;
		}
	} else {
		response->data.byte = dmg_runtime_read(request->address);
		response->length = sizeof(request->data.byte);
	}

exit:
	return result;
}

static int
dmg_action_serial_in(
	__in const dmg_action_t *request,
	__in dmg_action_t *response,
	__in dmg_runtime_t *runtime
	)
{
	response->data.byte = dmg_runtime_serial_in(request->data.byte);
	response->length = sizeof(request->data.byte);

	return DMG_STATUS_SUCCESS;
}

static int
dmg_action_write(
	__in const dmg_action_t *request,
	__in dmg_action_t *response,
	__in dmg_runtime_t *runtime
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(request->data.dword > UINT16_MAX) {

		switch(request->address) {
			case DMG_REGISTER_PROCESSOR_A ... DMG_REGISTER_PROCESSOR_STOP:
				dmg_processor_write_register(&runtime->processor, request, response);
				break;
			default:
				TRACE_FORMAT(LEVEL_WARNING, "Unsupported register write %04x", request->address);
				result = DMG_STATUS_INVALID;
				goto exit;
		}
	} else {
		dmg_runtime_write(request->address, request->data.byte);
	}

exit:
	return result;
}

static const dmg_action_hdlr ACTION_HANDLER[] = {
	dmg_action_nop, /* DMG_ACTION_NOP */
	dmg_action_cycle, /* DMG_ACTION_CYCLE */
	dmg_action_read,/* DMG_ACTION_READ */
	dmg_action_serial_in, /* DMG_ACTION_SERIAL_IN */
	dmg_action_write, /* DMG_ACTION_WRITE */
	};

int
dmg_action_handler(
	__in const dmg_action_t *request,
	__in dmg_action_t *response,
	__in void *context
	)
{
	int result;

	if(!request) {
		result = ERROR_SET(DMG_STATUS_INVALID, "Request is NULL");
		goto exit;
	}

	if(request->type >= DMG_ACTION_MAX) {
		result = ERROR_SET_FORMAT(DMG_STATUS_INVALID, "Unsupported action [%u]->%u", request->id, request->type);
		goto exit;
	}

	if(!response) {
		result = ERROR_SET(DMG_STATUS_INVALID, "Response is NULL");
		goto exit;
	}

	memset(response, 0, sizeof(*response));
	response->id = request->id;
	response->type = request->type;
	result = ACTION_HANDLER[request->type](request, response, (dmg_runtime_t *)context);

exit:
	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
