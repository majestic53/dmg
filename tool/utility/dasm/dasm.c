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

#include "./dasm_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static dmg_dasm_t g_dasm = {};

static int
dmg_utility_dasm_disassemble_comment(void)
{
	uint32_t address;
	uint16_t checksum = 0;
	int result = EXIT_SUCCESS;
	const dmg_version_t *version;
	const dmg_cartridge_header_t *header;

	if((version = dmg_version_get())) {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%s %u.%u.%u\n", COMMENT_PREFIX, DMG, version->major, version->minor, version->patch);
	} else {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "\n");
	}

	TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%s\n\n%c%s -- %.02f KB (%u bytes)\n\n", COMMENT_PREFIX, DMG_NOTICE, COMMENT_PREFIX, g_dasm.rom,
		g_dasm.buffer.length / (float)KBYTE, g_dasm.buffer.length);

	header = (const dmg_cartridge_header_t *)&((uint8_t *)g_dasm.buffer.data)[ADDRESS_HEADER_BEGIN];
	TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%cTitle     \"", COMMENT_PREFIX);

	for(address = 0; address < CARTRIDGE_HEADER_TITLE_LENGTH; ++address) {
		char value = header->title[address];

		if(!value) {
			break;
		}

		if(!isprint(value) && !isspace(value)) {
			TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\\%02x", value);
		} else {
			TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c", value);
		}
	}

	TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\"\n%cType      ", COMMENT_PREFIX);

	switch(header->cgb) {
		case CGB_SUPPORT:
			TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "GB/GBC (Gameboy/Gameboy Color)");
			break;
		case CGB_SUPPORT_ONLY:
			TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "GBC Only (Gameboy Color Only)");
			result = EXIT_FAILURE;
			break;
		default:
			TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "GB (Gameboy)");
			break;
	}

	if(header->sgb == SGB_SUPPORT) {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", ", SBC (Super Gameboy)");
	}

	TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\n%cRegion    %s", COMMENT_PREFIX, header->destination ? "U (International)" : "JP (Japan)");
	TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\n%cMapper    ", COMMENT_PREFIX);

	if((header->mapper >= MAPPER_MAX) || !strlen(MAPPER_STR[header->mapper])) {
		result = EXIT_FAILURE;
	} else {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s\n", MAPPER_STR[header->mapper]);
	}

	TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%cRom       ", COMMENT_PREFIX);

	if(header->rom >= ROM_MAX) {
		result = EXIT_FAILURE;
	} else {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s\n", ROM_STR[header->rom]);
	}

	TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%cRam       ", COMMENT_PREFIX);

	if(header->ram >= RAM_MAX) {
		result = EXIT_FAILURE;
	} else {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s\n\n", RAM_STR[header->ram]);
	}

	for(address = ADDRESS_HEADER_CHECKSUM_BEGIN; address <= ADDRESS_HEADER_CHECKSUM_END; ++address) {
		checksum = (checksum - ((uint8_t *)g_dasm.buffer.data)[address] - 1);
	}

	checksum &= UINT8_MAX;
	if(header->checksum != checksum) {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%cChecksum  MISMATCH (Expecting %02x)\n", COMMENT_PREFIX, checksum);
	}

	return result;
}

static int
dmg_utility_dasm_disassemble_header(
	__inout uint32_t *banks,
	__in bool final
	)
{
	int result = EXIT_SUCCESS;
	const dmg_cartridge_header_t *header;

	if(final) {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%cHeader [%04x-%04x]", COMMENT_PREFIX, ADDRESS_HEADER_BEGIN, ADDRESS_HEADER_END);
	}

	header = (const dmg_cartridge_header_t *)&((uint8_t *)g_dasm.buffer.data)[ADDRESS_HEADER_BEGIN];
	*banks = ROM_BANK[header->rom];

	if(final) {
		int index = 0;

		for(uint16_t address = ADDRESS_HEADER_BEGIN; address <= ADDRESS_HEADER_END; address += HEADER_LEN[index++]) {
			uint16_t offset;
			int count = 0;
			char str[HEADER_WIDTH + 1] = {};

			TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\n%s\n\t\t%c%04x\n\t%s", dmg_tool_header_string(index), COMMENT_PREFIX, address,
					dmg_tool_directive_string(DIRECTIVE_DATA));

			for(offset = address; offset < (address + HEADER_LEN[index]); ++offset) {
				uint8_t value;

				if(!(count % HEADER_WIDTH)) {

					if(strlen(str)) {
						TRACE_TOOL(g_dasm.file, LEVEL_MAX, "   %c%s\n\t%s", COMMENT_PREFIX, str, dmg_tool_directive_string(DIRECTIVE_DATA));
						memset(str, 0, sizeof(str));
						count = 0;
					}
				}

				value = ((uint8_t *)g_dasm.buffer.data)[offset];
				TRACE_TOOL(g_dasm.file, LEVEL_MAX, " %02x", value);
				str[count++] = ((isprint((char)value) && !isspace((char)value)) ? value : CHARACTER_FILL);
			}

			if(strlen(str)) {

				for(offset = 0; offset < (HEADER_WIDTH - count); ++offset) {
					TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "   ");
				}

				TRACE_TOOL(g_dasm.file, LEVEL_MAX, "   %c%s", COMMENT_PREFIX, str);
				memset(str, 0, sizeof(str));
			}

			TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "\n");
		}

		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "\n");
	}

	return result;
}

static int
dmg_utility_dasm_disassemble_instruction(
	__inout uint16_t *address,
	__in uint32_t origin,
	__in uint32_t bank,
	__in bool final
	)
{
	bool extended = false;
	const char *format = NULL;
	int result = EXIT_SUCCESS;
	uint8_t opcode, operand[2] = {};
	const dmg_tool_instruction_t *instruction;
	uint16_t base = ((bank ? ADDRESS_ROM_SWAP_BEGIN : ADDRESS_ROM_BEGIN) + *address);

	if(final) {
		uint32_t index = 0;

		for(; index < g_dasm.subroutine_count; ++index) {

			if(base == g_dasm.subroutine[index]) {
				break;
			}
		}

		if(index < g_dasm.subroutine_count) {
			TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s%04x%s\n", SUBROUTINE_PREFIX, base, LABEL_PREFIX);
		}
	}

	if((extended = ((opcode = ((uint8_t *)g_dasm.buffer.data)[origin + (*address)++]) == INSTRUCTION_EXTENDED_PREFIX))) {
		opcode = ((uint8_t *)g_dasm.buffer.data)[origin + (*address)++];
	}

	if(final) {
		format = dmg_tool_instruction_string(opcode, extended);
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "\t\t");
	}

	switch((instruction = dmg_tool_instruction(opcode, extended))->operand) {
		case OPERAND_BYTE:
			operand[0] = ((uint8_t *)g_dasm.buffer.data)[origin + (*address)++];

			if(final) {

				if(extended) {
					TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%04x {%02x %02x %02x}\n", COMMENT_PREFIX, base, INSTRUCTION_EXTENDED_PREFIX,
						opcode, operand[0]);
				} else {
					TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%04x {%02x %02x}\n", COMMENT_PREFIX, base, opcode, operand[0]);
				}

				TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "\t");
				TRACE_TOOL(g_dasm.file, LEVEL_MAX, format, operand[0]);
				TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "\n");
			}
			break;
		case OPERAND_WORD:
			operand[0] = ((uint8_t *)g_dasm.buffer.data)[origin + (*address)++];
			operand[1] = ((uint8_t *)g_dasm.buffer.data)[origin + (*address)++];

			if(final) {

				if(extended) {
					TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%04x {%02x %02x %02x %02x}\n", COMMENT_PREFIX, base, INSTRUCTION_EXTENDED_PREFIX,
						opcode, operand[0], operand[1]);
				} else {
					TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%04x {%02x %02x %02x}\n", COMMENT_PREFIX, base, opcode, operand[0], operand[1]);
				}

				TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "\t");
				TRACE_TOOL(g_dasm.file, LEVEL_MAX, format, (operand[1] << CHAR_BIT) | operand[0]);
				TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "\n");
			} else if(!extended && (g_dasm.subroutine_count < SUBROUTINE_MAX)) {

				switch(opcode) {
					case INSTRUCTION_CALL_U16:
					case INSTRUCTION_CALL_C_U16:
					case INSTRUCTION_CALL_NC_U16:
					case INSTRUCTION_CALL_NZ_U16:
					case INSTRUCTION_CALL_Z_U16: {
							uint16_t value = ((operand[1] << CHAR_BIT) | operand[0]);

							if(value <= ADDRESS_ROM_SWAP_END) {
								uint32_t index = 0;

								for(; index < g_dasm.subroutine_count; ++index) {

									if(g_dasm.subroutine[index] == value) {
										break;
									}
								}

								if(index == g_dasm.subroutine_count) {
									g_dasm.subroutine[g_dasm.subroutine_count++] = value;
								}
							}
						} break;
					default:
						break;
				}
			}
			break;
		default:

			if(final) {

				if(extended) {
					TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%04x {%02x %02x}\n", COMMENT_PREFIX, base, INSTRUCTION_EXTENDED_PREFIX,
							opcode);
				} else {
					TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%04x {%02x}\n", COMMENT_PREFIX, base, opcode);
				}

				TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\t%s\n", format);
			}
			break;
	}

	return result;
}

static int
dmg_utility_dasm_disassemble_vectors(
	__in bool final
	)
{
	int result = EXIT_SUCCESS;

	if(final) {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%cVector table [%04x-%04x]", COMMENT_PREFIX, ADDRESS_VECTOR_BEGIN, ADDRESS_HEADER_BEGIN - 1);
	}

	for(uint16_t address = ADDRESS_VECTOR_BEGIN; address < ADDRESS_HEADER_BEGIN;) {

		if(final && !(address % VECTOR_WIDTH) && (address <= ADDRESS_VECTOR_END)) {
			TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\n%s\n", dmg_tool_vector_string(address / VECTOR_WIDTH));
		}

		if((result = dmg_utility_dasm_disassemble_instruction(&address, 0, 0, final)) != EXIT_SUCCESS) {
			goto exit;
		}
	}

	if(final) {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "\n");
	}

exit:
	return result;
}

static int
dmg_utility_dasm_disassemble_bank(
	__inout uint32_t *banks,
	__in uint32_t bank,
	__in bool final
	)
{
	uint16_t address;
	int result = EXIT_SUCCESS;
	uint32_t origin = (bank ? ADDRESS_ROM_SWAP_BEGIN : ADDRESS_ROM_BEGIN);

	if(final) {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\n%cBank #%u\n\t%s %x\n\t%s %04x\n\n", COMMENT_PREFIX, bank,
			dmg_tool_directive_string(DIRECTIVE_BANK), bank, dmg_tool_directive_string(DIRECTIVE_ORIGIN), origin);
	}

	if(!bank) {

		if((result = dmg_utility_dasm_disassemble_vectors(final)) != EXIT_SUCCESS) {
			goto exit;
		}

		if((result = dmg_utility_dasm_disassemble_header(banks, final)) != EXIT_SUCCESS) {
			goto exit;
		}

		origin = 0;
		address = (ADDRESS_HEADER_END + 1);
	} else {
		origin = (bank * BANK_WIDTH);
		address = 0;
	}

	for(; address < BANK_WIDTH;) {

		if((result = dmg_utility_dasm_disassemble_instruction(&address, origin, bank, final)) != EXIT_SUCCESS) {
			goto exit;
		}
	}

exit:
	return result;
}

static void
dmg_utility_dasm_file_close(void)
{

	if(g_dasm.file) {
		fclose(g_dasm.file);
		g_dasm.file = NULL;
	}
}

static int
dmg_utility_dasm_file_disassemble(void)
{
	int result = EXIT_SUCCESS;
	uint32_t bank = 0, banks, length = 0;

	TRACE_TOOL_MESSAGE("%s -- %.02f KB (%u bytes)\n\n", g_dasm.rom, g_dasm.buffer.length / (float)KBYTE, g_dasm.buffer.length);

	if(g_dasm.buffer.length <= ADDRESS_HEADER_END) {
		TRACE_TOOL_ERROR("File is too small -- %.02f KB (%u bytes) (expecting > %.02f KB (%i bytes))\n", g_dasm.buffer.length / (float)KBYTE,
			g_dasm.buffer.length, ADDRESS_HEADER_END / (float)KBYTE, ADDRESS_HEADER_END);
		result = EXIT_FAILURE;
		goto exit;
	}

	if((result = dmg_utility_dasm_disassemble_comment()) != EXIT_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("Parsing bank[%u]...      ", bank);

	if((result = dmg_utility_dasm_disassemble_bank(&banks, bank, false)) != EXIT_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("%s", "[Done]\n");

	for(bank = 1; bank < banks; ++bank) {
		TRACE_TOOL_MESSAGE("Parsing bank[%u]...      ", bank);

		if((result = dmg_utility_dasm_disassemble_bank(&banks, bank, false)) != EXIT_SUCCESS) {
			goto exit;
		}

		TRACE_TOOL_MESSAGE("%s", "[Done]\n");
	}

	TRACE_TOOL_MESSAGE("\nFound %u subroutines\n\n", g_dasm.subroutine_count);
	bank = 0;

	TRACE_TOOL_MESSAGE("Writing bank[%u]...      ", bank);

	if((result = dmg_utility_dasm_disassemble_bank(&banks, bank, true)) != EXIT_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("%s", "[Done]\n");

	for(bank = 1; bank < banks; ++bank) {
		TRACE_TOOL_MESSAGE("Writing bank[%u]...      ", bank);

		if((result = dmg_utility_dasm_disassemble_bank(&banks, bank, true)) != EXIT_SUCCESS) {
			goto exit;
		}

		TRACE_TOOL_MESSAGE("%s", "[Done]\n");
	}

	fseek(g_dasm.file, 0, SEEK_END);
	length = ftell(g_dasm.file);
	fseek(g_dasm.file, 0, SEEK_SET);
	TRACE_TOOL_MESSAGE("\n%s -- %.02f KB (%u bytes)\n", g_dasm.output, length / (float)KBYTE, length);

exit:
	return result;
}

static int
dmg_utility_dasm_file_open(
	__in const char *path
	)
{
	int result = EXIT_SUCCESS;
	char path_full[PATH_MAX] = {};

	if(!g_dasm.output || !strlen(g_dasm.output)) {
		char *path_end;

		if((path_end = strrchr(path, PATH_DELIMITER))) {
			memcpy(path_full, path, (path_end - path) + 1);
		}

		strcat(path_full, PATH_OUTPUT);
	} else {
		strcpy(path_full, g_dasm.output);
	}

	if(!(g_dasm.file = fopen(path_full, "wb"))) {
		result = EXIT_FAILURE;
		goto exit;
	}

exit:
	return result;
}

static int
dmg_utility_dasm_file_load(void)
{
	FILE *file = NULL;
	int length, result = EXIT_SUCCESS;

	if(!(file = fopen(g_dasm.rom, "rb"))) {
		result = EXIT_FAILURE;
		goto exit;
	}

	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	if(length <= 0) {
		result = EXIT_FAILURE;
		goto exit;
	}

	if(!(g_dasm.buffer.data = (void *)malloc(length))) {
		result = EXIT_FAILURE;
		goto exit;
	}

	if(fread(g_dasm.buffer.data, sizeof(uint8_t), length, file) != length) {
		result = EXIT_FAILURE;
		goto exit;
	}

	g_dasm.buffer.length = length;

exit:

	if(file) {
		fclose(file);
		file = NULL;
	}

	return result;
}

static void
dmg_utility_dasm_file_unload(void)
{

	if(g_dasm.buffer.data) {
		free(g_dasm.buffer.data);
	}

	memset(&g_dasm.buffer, 0, sizeof(g_dasm.buffer));
}

static int
dmg_utility_dasm_parse(
	__in int argc,
	__in char *argv[]
	)
{
	int option, result = EXIT_SUCCESS;

	opterr = 1;

	while((option = getopt(argc, argv, OPTIONS)) != -1) {

		switch(option) {
			case OPTION_HELP:
				g_dasm.help = true;
				break;
			case OPTION_OUTPUT:
				g_dasm.output = optarg;
				break;
			case OPTION_ROM:
				g_dasm.rom = optarg;
				break;
			case OPTION_VERSION:
				g_dasm.version = true;
				break;
			case '?':
				result = EXIT_FAILURE;
				goto exit;
			default:
				result = EXIT_FAILURE;
				goto exit;
		}
	}

exit:
	return result;
}

static void
dmg_utility_dasm_version(
	__in FILE *stream,
	__in bool verbose
	)
{
	const dmg_version_t *version;

	if(verbose) {
		TRACE_TOOL(stream, LEVEL_NONE, "%s", DMG);
	}

	if((version = dmg_version_get())) {

		if(verbose) {
			TRACE_TOOL(stream, LEVEL_NONE, "%s", " ");
		}

		TRACE_TOOL(stream, LEVEL_NONE, "%u.%u.%u\n", version->major, version->minor, version->patch);
	} else {
		TRACE_TOOL(stream, LEVEL_NONE, "%s", "\n");
	}

	if(verbose) {
		TRACE_TOOL(stream, LEVEL_NONE, "%s\n", DMG_NOTICE);
	}
}

static void
dmg_utility_dasm_usage(
	__in FILE *stream,
	__in bool verbose
	)
{

	if(verbose) {
		dmg_utility_dasm_version(stream, verbose);
		TRACE_TOOL(stream, LEVEL_NONE, "%s", "\n");
	}

	TRACE_TOOL(stream, LEVEL_NONE, "%s\n", DMG_USAGE);

	if(verbose) {

		for(int flag = 0; flag < FLAG_MAX; ++flag) {
			TRACE_TOOL(stream, LEVEL_NONE, "\n%s\t%s", FLAG_STR[flag], FLAG_DESCRIPTION_STR[flag]);
		}

		TRACE_TOOL(stream, LEVEL_NONE, "%s", "\n");
	}
}

int
main(
	__in int argc,
	__in char *argv[]
	)
{
	int result = EXIT_SUCCESS;

	if((result = dmg_utility_dasm_parse(argc, argv)) != EXIT_SUCCESS) {
		goto exit;
	}

	if(g_dasm.help) {
		dmg_utility_dasm_usage(stdout, true);
	} else if(g_dasm.version) {
		dmg_utility_dasm_version(stdout, false);
	} else {

		if((result = dmg_utility_dasm_file_load()) != EXIT_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to load file -- %s\n", argv[0], g_dasm.rom);
			goto exit;
		}

		if((result = dmg_utility_dasm_file_open(argv[0])) != EXIT_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to open file -- %s\n", argv[0], g_dasm.output);
			goto exit;
		}

		if((result = dmg_utility_dasm_file_disassemble()) != EXIT_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to disassemble file -- %s\n", argv[0], g_dasm.rom);
			goto exit;
		}
	}

exit:
	dmg_utility_dasm_file_close();
	dmg_utility_dasm_file_unload();
	memset(&g_dasm, 0, sizeof(g_dasm));

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
