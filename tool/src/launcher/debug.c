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

#include "./debug_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_launcher_debug_register(
	__in const char *argument
	)
{
	int result;

	for(result = DMG_REGISTER_PROCESSOR_A; result < DMG_REGISTER_MAX; ++result) {

		if(!strcmp(REGISTER_STR[result], argument)) {
			break;
		}
	}

	return result;
}

static int
dmg_launcher_debug_disassemble_data(
	__in uint16_t address,
	__in uint16_t offset,
	__in bool verbose
	)
{
	int result;
	uint32_t index;
	dmg_action_t request = {}, response = {};

	request.type = DMG_ACTION_READ;
	request.address = address;

	for(index = 0; index < offset; ++index) {
		uint8_t opcode;
		bool extended = false;

		if((result = dmg_action(&request, &response)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		++request.address;

		if((extended = ((opcode = response.data.byte) == INSTRUCTION_EXTENDED_PREFIX))) {

			if((result = dmg_action(&request, &response)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			++request.address;
			opcode = response.data.byte;
		}

		request.address += dmg_processor_instruction(opcode, extended)->operand;
	}

	if(verbose) {
		TRACE_TOOL_MESSAGE("[%04x-%04x] -- %.02f KB (%u bytes), %u instructions\n\n", address, request.address - 1,
			(request.address - address) / (float)KBYTE, (uint32_t)(request.address - address), offset);
	}

	request.address = address;

	for(index = 0; index < offset; ++index) {
		const char *format;
		bool extended = false;
		uint8_t opcode, operand[2] = {};
		const dmg_processor_instruction_t *instruction;

		if((result = dmg_action(&request, &response)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		++request.address;

		if((extended = ((opcode = response.data.byte) == INSTRUCTION_EXTENDED_PREFIX))) {

			if((result = dmg_action(&request, &response)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			++request.address;
			opcode = response.data.byte;
		}

		format = dmg_processor_instruction_string(opcode, extended);

		switch((instruction = dmg_processor_instruction(opcode, extended))->operand) {
			case OPERAND_BYTE:

				if((result = dmg_action(&request, &response)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}

				++request.address;
				operand[0] = response.data.byte;

				if(extended) {
					TRACE_TOOL_MESSAGE("%04x | %02x %02x %02x     ", address, INSTRUCTION_EXTENDED_PREFIX, opcode,
						operand[0]);
				} else {
					TRACE_TOOL_MESSAGE("%04x | %02x %02x        ", address, opcode, operand[0]);
				}

				switch(instruction->opcode) {
					case INSTRUCTION_ADD_SP_I8:
					case INSTRUCTION_LD_HL_SP_I8:
						TRACE_TOOL_MESSAGE(format, operand[0], operand[0]);
						break;
					case INSTRUCTION_JR_C_I8:
					case INSTRUCTION_JR_NC_I8:
					case INSTRUCTION_JR_NZ_I8:
					case INSTRUCTION_JR_I8:
					case INSTRUCTION_JR_Z_I8:
						TRACE_TOOL_MESSAGE(format, operand[0], operand[0], request.address + (int8_t)operand[0]);
						break;
					default:
						TRACE_TOOL_MESSAGE(format, operand[0]);
						break;
				}
				break;
			case OPERAND_WORD:

				if((result = dmg_action(&request, &response)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}

				++request.address;
				operand[0] = response.data.byte;

				if((result = dmg_action(&request, &response)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}

				++request.address;
				operand[1] = response.data.byte;

				if(extended) {
					TRACE_TOOL_MESSAGE("%04x | %02x %02x %02x %02x   ", address, INSTRUCTION_EXTENDED_PREFIX, opcode,
						operand[0], operand[1]);
				} else {
					TRACE_TOOL_MESSAGE("%04x | %02x %02x %02x     ", address, opcode, operand[0], operand[1]);
				}

				TRACE_TOOL_MESSAGE(format, (operand[1] << CHAR_BIT) | operand[0]);
				break;
			default:

				if(extended) {
					TRACE_TOOL_MESSAGE("%04x | %02x %02x        ", address, INSTRUCTION_EXTENDED_PREFIX, opcode);
				} else {
					TRACE_TOOL_MESSAGE("%04x | %02x           ", address, opcode);
				}

				TRACE_TOOL_MESSAGE("%s", format);
				break;
		}

		TRACE_TOOL_MESSAGE("%s", "\n");
		address = request.address;
	}

exit:
	return result;
}

static int
dmg_launcher_debug_disassemble_register(
	__in uint16_t address,
	__in uint16_t offset,
	__in bool verbose
	)
{
	int result;
	dmg_action_t request = {}, response = {};

	request.type = DMG_ACTION_READ;
	request.address = address;
	request.data.dword = UINT32_MAX;

	if((result = dmg_action(&request, &response)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	switch(address) {
		case DMG_REGISTER_PROCESSOR_A:
		case DMG_REGISTER_PROCESSOR_B:
		case DMG_REGISTER_PROCESSOR_C:
		case DMG_REGISTER_PROCESSOR_D:
		case DMG_REGISTER_PROCESSOR_E:
		case DMG_REGISTER_PROCESSOR_F:
		case DMG_REGISTER_PROCESSOR_H:
		case DMG_REGISTER_PROCESSOR_HALT:
		case DMG_REGISTER_PROCESSOR_IE:
		case DMG_REGISTER_PROCESSOR_IF:
		case DMG_REGISTER_PROCESSOR_IME:
		case DMG_REGISTER_PROCESSOR_L:
		case DMG_REGISTER_PROCESSOR_STOP:
			address = response.data.byte;
			break;
		case DMG_REGISTER_PROCESSOR_AF:
		case DMG_REGISTER_PROCESSOR_BC:
		case DMG_REGISTER_PROCESSOR_DE:
		case DMG_REGISTER_PROCESSOR_HL:
		case DMG_REGISTER_PROCESSOR_PC:
		case DMG_REGISTER_PROCESSOR_SP:
			address = response.data.word;
			break;
		default:
			result = DMG_STATUS_INVALID;
			goto exit;
	}

	result = dmg_launcher_debug_disassemble_data(address, offset, verbose);

exit:
	return result;
}

static int
dmg_launcher_debug_disassemble(
	__in const char *argument[],
	__in uint32_t count
	)
{
	int result;
	bool register_read = false;
	uint16_t address = 0, offset = 1;

	for(uint32_t index = 0; index < count; ++index) {

		switch(index) {
			case DEBUG_READ_ADDRESS:

				if((address = dmg_launcher_debug_register(argument[0])) < DMG_REGISTER_MAX) {
					register_read = true;
				} else {
					address = strtoul(argument[0], NULL, 16);
				}
				break;
			case DEBUG_READ_OFFSET:
				offset = strtoul(argument[1], NULL, 16);
				break;
			default:
				result = DMG_STATUS_INVALID;
				goto exit;
		}
	}

	if(register_read) {
		result = dmg_launcher_debug_disassemble_register(address, offset, true);
	} else {
		result = dmg_launcher_debug_disassemble_data(address, offset, true);
	}

exit:
	return result;
}

static int
dmg_launcher_debug_help(
	__in const char *argument[],
	__in uint32_t count
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(count) {
		result = DMG_STATUS_INVALID;
		goto exit;
	}

	for(int debug = 0; debug < DEBUG_MAX; ++debug) {
		TRACE_TOOL_MESSAGE("%c\t%s\n", DEBUG_CHAR[debug], DEBUG_DESCRIPTION_STR[debug]);
	}

exit:
	return result;
}

static int
dmg_launcher_debug_read_data(
	__in uint16_t address,
	__in uint16_t offset
	)
{
	uint32_t index;
	int result = DMG_STATUS_SUCCESS;
	char str[ARGUMENT_READ_WIDTH + 1] = {};
	dmg_action_t request = {}, response = {};

	request.type = DMG_ACTION_READ;

	if(offset > 1) {
		TRACE_TOOL_MESSAGE("[%04x-%04x] -- %.02f KB (%u bytes)\n\n      ", address, (address + offset - 1) % UINT16_MAX,
			offset / (float)KBYTE, offset);

		for(index = address; index < (address + ARGUMENT_READ_WIDTH); ++index) {
			TRACE_TOOL_MESSAGE(" %02x", index % ARGUMENT_READ_WIDTH);
		}

		TRACE_TOOL_MESSAGE("%s", "\n      ");

		for(index = 0; index < ARGUMENT_READ_WIDTH; ++index) {
			TRACE_TOOL_MESSAGE("%s", " --");
		}
	}

	for(uint32_t count = 0, index = address; index < (address + offset); count %= ARGUMENT_READ_WIDTH, ++index) {
		request.address = (index % (UINT16_MAX + 1));

		if((offset > 1) && !count) {

			if(strlen(str)) {
				TRACE_TOOL_MESSAGE("   %s", str);
				memset(str, 0, sizeof(str));
			}

			TRACE_TOOL_MESSAGE("\n%04x |", request.address);
		}

		if((result = dmg_action(&request, &response)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(offset > 1) {
			TRACE_TOOL_MESSAGE("%s", " ");
		}

		TRACE_TOOL_MESSAGE("%02x", response.data.byte);
		str[count++] = ((isprint((char)response.data.byte) && !isspace((char)response.data.byte))
				? response.data.byte : CHARACTER_FILL);
	}

	if(offset > 1 && strlen(str)) {

		for(index = 0; index < (ARGUMENT_READ_WIDTH - ((request.address - address) % ARGUMENT_READ_WIDTH)); ++index) {
			TRACE_TOOL_MESSAGE("%s", "   ");
		}

		TRACE_TOOL_MESSAGE("%s", str);
		memset(str, 0, sizeof(str));
	}

	TRACE_TOOL_MESSAGE("%s", "\n");

exit:
	return result;
}

static int
dmg_launcher_debug_read_register(
	__in uint16_t address
	)
{
	int result;
	dmg_action_t request = {}, response = {};

	request.type = DMG_ACTION_READ;
	request.address = address;
	request.data.dword = UINT32_MAX;

	if((result = dmg_action(&request, &response)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	switch(address) {
		case DMG_REGISTER_PROCESSOR_A:
		case DMG_REGISTER_PROCESSOR_B:
		case DMG_REGISTER_PROCESSOR_C:
		case DMG_REGISTER_PROCESSOR_D:
		case DMG_REGISTER_PROCESSOR_E:
		case DMG_REGISTER_PROCESSOR_F:
		case DMG_REGISTER_PROCESSOR_H:
		case DMG_REGISTER_PROCESSOR_IE:
		case DMG_REGISTER_PROCESSOR_IF:
		case DMG_REGISTER_PROCESSOR_L:
			TRACE_TOOL_MESSAGE("%02x", response.data.byte);

			switch(address) {
				case DMG_REGISTER_PROCESSOR_IE:
				case DMG_REGISTER_PROCESSOR_IF:
					TRACE_TOOL_MESSAGE("     [%c%c%c%c%c]",
						response.data.interrupt.vblank ? PROCESSOR_INTERRUPT_CHAR[PROCESSOR_INTERRUPT_VBLANK] : PROCESSOR_DELIMITER,
						response.data.interrupt.lcdc ? PROCESSOR_INTERRUPT_CHAR[PROCESSOR_INTERRUPT_LCDC] : PROCESSOR_DELIMITER,
						response.data.interrupt.timer ? PROCESSOR_INTERRUPT_CHAR[PROCESSOR_INTERRUPT_TIMER] : PROCESSOR_DELIMITER,
						response.data.interrupt.serial ? PROCESSOR_INTERRUPT_CHAR[PROCESSOR_INTERRUPT_SERIAL] : PROCESSOR_DELIMITER,
						response.data.interrupt.joypad ? PROCESSOR_INTERRUPT_CHAR[PROCESSOR_INTERRUPT_JOYPAD] : PROCESSOR_DELIMITER);
					break;
				default:
					break;
			}
			break;
		case DMG_REGISTER_PROCESSOR_AF:
		case DMG_REGISTER_PROCESSOR_BC:
		case DMG_REGISTER_PROCESSOR_DE:
		case DMG_REGISTER_PROCESSOR_HL:
		case DMG_REGISTER_PROCESSOR_PC:
		case DMG_REGISTER_PROCESSOR_SP:
			TRACE_TOOL_MESSAGE("%04x", response.data.word);

			switch(address) {
				case DMG_REGISTER_PROCESSOR_AF:
					TRACE_TOOL_MESSAGE("   %s=%02x, %s=%02x [%c%c%c%c]",
						REGISTER_STR[DMG_REGISTER_PROCESSOR_A], response.data.high,
						REGISTER_STR[DMG_REGISTER_PROCESSOR_F], response.data.low,
						response.data.flag.carry ? PROCESSOR_FLAG_CHAR[PROCESSOR_FLAG_CARRY] : PROCESSOR_DELIMITER,
						response.data.flag.carry_half ? PROCESSOR_FLAG_CHAR[PROCESSOR_FLAG_CARRY_HALF] : PROCESSOR_DELIMITER,
						response.data.flag.subtract ? PROCESSOR_FLAG_CHAR[PROCESSOR_FLAG_SUBTRACT] : PROCESSOR_DELIMITER,
						response.data.flag.zero ? PROCESSOR_FLAG_CHAR[PROCESSOR_FLAG_ZERO] : PROCESSOR_DELIMITER);
					break;
				case DMG_REGISTER_PROCESSOR_BC:
					TRACE_TOOL_MESSAGE("   %s=%02x, %s=%02x", REGISTER_STR[DMG_REGISTER_PROCESSOR_B], response.data.high,
						REGISTER_STR[DMG_REGISTER_PROCESSOR_C], response.data.low);
					break;
				case DMG_REGISTER_PROCESSOR_DE:
					TRACE_TOOL_MESSAGE("   %s=%02x, %s=%02x", REGISTER_STR[DMG_REGISTER_PROCESSOR_D], response.data.high,
						REGISTER_STR[DMG_REGISTER_PROCESSOR_E], response.data.low);
					break;
				case DMG_REGISTER_PROCESSOR_HL:
					TRACE_TOOL_MESSAGE("   %s=%02x, %s=%02x", REGISTER_STR[DMG_REGISTER_PROCESSOR_H], response.data.high,
						REGISTER_STR[DMG_REGISTER_PROCESSOR_L], response.data.low);
					break;
				default:
					break;
			}
			break;
		case DMG_REGISTER_PROCESSOR_HALT:
		case DMG_REGISTER_PROCESSOR_IME:
		case DMG_REGISTER_PROCESSOR_STOP:
			TRACE_TOOL_MESSAGE("%x", response.data.byte);
			break;
		default:
			result = DMG_STATUS_INVALID;
			break;
	}

	TRACE_TOOL_MESSAGE("%s", "\n");

exit:
	return result;
}

static int
dmg_launcher_debug_processor(
	__in const char *argument[],
	__in uint32_t count
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(count) {
		result = DMG_STATUS_INVALID;
		goto exit;
	}

	TRACE_TOOL_MESSAGE("%s   | ", REGISTER_STR[DMG_REGISTER_PROCESSOR_AF]);

	if((result = dmg_launcher_debug_read_register(DMG_REGISTER_PROCESSOR_AF)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("%s   | ", REGISTER_STR[DMG_REGISTER_PROCESSOR_BC]);

	if((result = dmg_launcher_debug_read_register(DMG_REGISTER_PROCESSOR_BC)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("%s   | ", REGISTER_STR[DMG_REGISTER_PROCESSOR_DE]);

	if((result = dmg_launcher_debug_read_register(DMG_REGISTER_PROCESSOR_DE)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("%s   | ", REGISTER_STR[DMG_REGISTER_PROCESSOR_HL]);

	if((result = dmg_launcher_debug_read_register(DMG_REGISTER_PROCESSOR_HL)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("%s   | ", REGISTER_STR[DMG_REGISTER_PROCESSOR_PC]);

	if((result = dmg_launcher_debug_read_register(DMG_REGISTER_PROCESSOR_PC)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("%s   | ", REGISTER_STR[DMG_REGISTER_PROCESSOR_SP]);

	if((result = dmg_launcher_debug_read_register(DMG_REGISTER_PROCESSOR_SP)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("%s  | ", REGISTER_STR[DMG_REGISTER_PROCESSOR_IME]);

	if((result = dmg_launcher_debug_read_register(DMG_REGISTER_PROCESSOR_IME)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("%s   | ", REGISTER_STR[DMG_REGISTER_PROCESSOR_IE]);

	if((result = dmg_launcher_debug_read_register(DMG_REGISTER_PROCESSOR_IE)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("%s   | ", REGISTER_STR[DMG_REGISTER_PROCESSOR_IF]);

	if((result = dmg_launcher_debug_read_register(DMG_REGISTER_PROCESSOR_IF)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("%s | ", REGISTER_STR[DMG_REGISTER_PROCESSOR_HALT]);

	if((result = dmg_launcher_debug_read_register(DMG_REGISTER_PROCESSOR_HALT)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("%s | ", REGISTER_STR[DMG_REGISTER_PROCESSOR_STOP]);

	if((result = dmg_launcher_debug_read_register(DMG_REGISTER_PROCESSOR_STOP)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_launcher_debug_read(
	__in const char *argument[],
	__in uint32_t count
	)
{
	int result;
	bool register_read = false;
	uint16_t address = 0, offset = 1;

	for(uint32_t index = 0; index < count; ++index) {

		switch(index) {
			case DEBUG_READ_ADDRESS:

				if((address = dmg_launcher_debug_register(argument[0])) < DMG_REGISTER_MAX) {
					register_read = true;
				} else {
					address = strtoul(argument[0], NULL, 16);
				}
				break;
			case DEBUG_READ_OFFSET:

				if(register_read) {
					result = DMG_STATUS_INVALID;
					goto exit;
				}

				offset = strtoul(argument[1], NULL, 16);
				break;
			default:
				result = DMG_STATUS_INVALID;
				goto exit;
		}
	}

	if(register_read) {
		result = dmg_launcher_debug_read_register(address);
	} else {
		result = dmg_launcher_debug_read_data(address, offset);
	}

exit:
	return result;
}

static int
dmg_launcher_debug_run(
	__in const char *argument[],
	__in uint32_t count
	)
{
	uint16_t breakpoint[ARGUMENT_MAX] = {};

	for(uint32_t index = 0; index < count; ++index) {
		breakpoint[index] = strtoul(argument[index], NULL, 16);
	}

	return dmg_run(breakpoint, count);
}

static int
dmg_launcher_debug_step(
	__in const char *argument[],
	__in uint32_t count
	)
{
	int result;
	uint16_t breakpoint[ARGUMENT_MAX] = {}, instruction = 1;

	if(count >= 1) {
		instruction = strtoul(argument[0], NULL, 10);

		for(uint32_t index = 1; index < count; ++index) {
			breakpoint[index - 1] = strtoul(argument[index], NULL, 16);
		}

		--count;
	}

	if((result = dmg_step(instruction, breakpoint, count)) == DMG_STATUS_SUCCESS) {
		dmg_launcher_debug_disassemble_register(DMG_REGISTER_PROCESSOR_PC, 1, false);
	}

	return result;
}

static int
dmg_launcher_debug_version(
	__in const char *argument[],
	__in uint32_t count
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(count) {
		result = DMG_STATUS_INVALID;
		goto exit;
	}

	dmg_tool_version(stdout, false);

exit:
	return result;
}

static int
dmg_launcher_debug_write_data(
	__in uint16_t address,
	__in uint8_t value,
	__in uint16_t offset
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_action_t request = {}, response = {};

	request.type = DMG_ACTION_WRITE;
	request.address = address;
	request.data.byte = value;

	for(uint32_t index = 0; index < offset; ++index) {

		if((result = dmg_action(&request, &response)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		++request.address;
	}

exit:
	return result;
}

static int
dmg_launcher_debug_write_register(
	__in uint16_t address,
	__in uint16_t value
	)
{
	dmg_action_t request = {}, response = {};

	request.type = DMG_ACTION_WRITE;
	request.address = address;
	request.data.dword = UINT32_MAX;
	request.data.word = value;

	return dmg_action(&request, &response);
}

static int
dmg_launcher_debug_write(
	__in const char *argument[],
	__in uint32_t count
	)
{
	int result;
	bool register_read = false;
	uint16_t address = 0, offset = 1, value = 0;

	for(uint32_t index = 0; index < count; ++index) {

		switch(index) {
			case DEBUG_WRITE_ADDRESS:

				if((address = dmg_launcher_debug_register(argument[0])) < DMG_REGISTER_MAX) {
					register_read = true;
				} else {
					address = strtoul(argument[0], NULL, 16);
				}
				break;
			case DEBUG_WRITE_VALUE:
				value = strtoul(argument[1], NULL, 16);
				break;
			case DEBUG_WRITE_OFFSET:

				if(register_read) {
					result = DMG_STATUS_INVALID;
					goto exit;
				}

				offset = strtoul(argument[2], NULL, 16);
				break;
			default:
				result = DMG_STATUS_INVALID;
				goto exit;
		}
	}

	if(register_read) {
		result = dmg_launcher_debug_write_register(address, value);
	} else {
		result = dmg_launcher_debug_write_data(address, value, offset);
	}

exit:
	return result;
}

static const dmg_launcher_debug_hdlr DEBUG_HANDLER[] = {
	NULL, /* DEBUG_EXIT */
	dmg_launcher_debug_disassemble, /* DEBUG_DISASSEMBLE */
	dmg_launcher_debug_help, /* DEBUG_HELP */
	dmg_launcher_debug_processor, /* DEBUG_PROCESSOR */
	dmg_launcher_debug_read, /* DEBUG_READ */
	dmg_launcher_debug_run,/* DEBUG_RUN */
	dmg_launcher_debug_step, /* DEBUG_STEP */
	dmg_launcher_debug_version, /* DEBUG_VERSION */
	dmg_launcher_debug_write, /* DEBUG_WRITE */
	};

static int
dmg_launcher_debug_header(
	__in const char *root,
	__in const char *rom,
	__in const char *save
	)
{
	int result;
	char *path_end, path_full[PATH_MAX] = {};

	if((path_end = strrchr(root, PATH_DELIMITER))) {
		memcpy(path_full, root, (path_end - root) + 1);
	}

	dmg_tool_version(stdout, true);
	TRACE_TOOL_MESSAGE("%s", "\n");
	strcat(path_full, PATH_ROM_INFO);
	strcat(path_full, rom);
	result = system(path_full);

	if(save) {
		TRACE_TOOL_MESSAGE("%s", "\n");
		memset(path_full, 0, sizeof(path_full));

		if((path_end = strrchr(root, '/'))) {
			memcpy(path_full, root, (path_end - root) + 1);
		}

		strcat(path_full, PATH_SAVE_INFO);
		strcat(path_full, save);
		result = system(path_full);
	}

	return result;
}

static int
dmg_launcher_debug_prompt(
	__in char *prompt,
	__in uint32_t length
	)
{
	int result;
	dmg_action_t request = {}, response = {};

	request.type = DMG_ACTION_CYCLE;

	if((result = dmg_action(&request, &response)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	snprintf(prompt, length, "%s%s%u%s%s", dmg_tool_trace_level_string(LEVEL_INFORMATION), PROMPT_PREFIX, response.data.dword,
			PROMPT_POSTFIX, dmg_tool_trace_level_string(LEVEL_NONE));

exit:
	return result;
}

int
dmg_launcher_debug(
	__in const char *root,
	__in const char *rom,
	__in const char *save
	)
{
	int result;
	bool complete = false;

	using_history();
	dmg_launcher_debug_header(root, rom, save);

	while(!complete) {
		char *input, prompt[PROMPT_MAX] = {};

		if((result = dmg_launcher_debug_prompt(prompt, PROMPT_MAX)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((input = readline(prompt))) {
			int debug = 0;
			uint32_t count = 0;
			bool first = true, from_history = false;
			dmg_action_t request = {}, response = {};
			const char *argument[ARGUMENT_MAX] = {}, *next;

			if((from_history = !strlen(input))) {
				HIST_ENTRY *previous;

				if(!(previous = history_get(where_history()))) {
					goto cleanup;
				}

				input = previous->line;
			}

			for(; debug < DEBUG_MAX; ++debug) {

				if(input[0] == DEBUG_CHAR[debug]) {
					break;
				}
			}

			if(debug >= DEBUG_MAX) {
				TRACE_TOOL_ERROR("Unsupported command: %s\n", input);
				goto cleanup;
			}

			add_history(input);
			next = strtok(input, " ");

			while(next) {

				if(count >= ARGUMENT_MAX) {
					TRACE_TOOL_ERROR("Too many arguments: %u\n", count + 1);
					goto cleanup;
				}

				if(!first) {
					argument[count++] = next;
				}

				next = strtok(NULL, " ");
				first = false;
			}

			switch(debug) {
				case DEBUG_EXIT:
					complete = true;
					break;
				case (DEBUG_EXIT + 1) ... (DEBUG_MAX - 1):

					switch(DEBUG_HANDLER[debug](argument, count)) {
						case DMG_STATUS_SUCCESS:
							break;
						case DMG_STATUS_BREAKPOINT:
							request.type = DMG_ACTION_READ;
							request.address = DMG_REGISTER_PROCESSOR_PC;
							request.data.dword = UINT32_MAX;

							if(dmg_action(&request, &response) == DMG_STATUS_SUCCESS) {
								TRACE_TOOL_WARNING("Breakpoint: %04x\n", response.data.word);
							} else {
								TRACE_TOOL_WARNING("%s", "Breakpoint\n");
							}
							break;
						default:
							TRACE_TOOL_ERROR("Command failed: %s\n", input);
							break;
					}
					break;
				default:
					TRACE_TOOL_ERROR("Unsupported command type: %i\n", debug);
					break;
			}

cleanup:

			if(!from_history && input) {
				free(input);
			}
		}
	}

exit:
	clear_history();

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
