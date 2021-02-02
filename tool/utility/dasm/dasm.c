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

#include "./dasm_type.h"

static dmg_dasm_t g_dasm = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_utility_dasm_disassemble_comment(void)
{
	uint32_t address;
	const char *mapper;
	uint16_t checksum = 0;
	const dmg_version_t *version;
	int result = DMG_STATUS_SUCCESS;
	const dmg_cartridge_header_t *header;

	if((version = dmg_version_get())) {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%s %u.%u.%u\n", DELIMITER_COMMENT, DMG, version->major, version->minor, version->patch);
	} else {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "\n");
	}

	TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%s\n\n%c%s -- %.02f KB (%u bytes)\n\n", DELIMITER_COMMENT, DMG_NOTICE, DELIMITER_COMMENT, g_dasm.rom,
		g_dasm.buffer.length / (float)KBYTE, g_dasm.buffer.length);

	header = (const dmg_cartridge_header_t *)&((uint8_t *)g_dasm.buffer.data)[ADDRESS_HEADER_BEGIN];
	TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%cTitle     \"", DELIMITER_COMMENT);

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

	TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\"\n%cType      ", DELIMITER_COMMENT);

	switch(header->cgb) {
		case CGB_SUPPORT:
			TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "GB/GBC (Gameboy/Gameboy Color)");
			break;
		case CGB_SUPPORT_ONLY:
			TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "GBC Only (Gameboy Color Only)");
			result = DMG_STATUS_FAILURE;
			break;
		default:
			TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "GB (Gameboy)");
			break;
	}

	if(header->sgb == SGB_SUPPORT) {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", ", SBC (Super Gameboy)");
	}

	TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\n%cRegion    %s", DELIMITER_COMMENT, header->destination ? "U (International)" : "JP (Japan)");
	TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\n%cMapper    ", DELIMITER_COMMENT);
	mapper = dmg_tool_syntax_mapper_string(header->mapper);

	if((header->mapper >= MAPPER_MAX) || !strlen(mapper)) {
		result = DMG_STATUS_FAILURE;
	} else {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s\n", mapper);
	}

	TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%cRom       ", DELIMITER_COMMENT);

	if(header->rom >= ROM_MAX) {
		result = DMG_STATUS_FAILURE;
	} else {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s\n", dmg_tool_syntax_rom_string(header->rom));
	}

	TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%cRam       ", DELIMITER_COMMENT);

	if(header->ram >= RAM_MAX) {
		result = DMG_STATUS_FAILURE;
	} else {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s\n\n", dmg_tool_syntax_ram_string(header->ram));
	}

	for(address = ADDRESS_HEADER_CHECKSUM_BEGIN; address <= ADDRESS_HEADER_CHECKSUM_END; ++address) {
		checksum = (checksum - ((uint8_t *)g_dasm.buffer.data)[address] - 1);
	}

	checksum &= UINT8_MAX;
	if(header->checksum != checksum) {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%cChecksum  MISMATCH (Expecting %02x)\n", DELIMITER_COMMENT, checksum);
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
	int result = DMG_STATUS_SUCCESS;
	uint8_t opcode, operand[2] = {};
	const dmg_tool_syntax_instruction_t *instruction;
	uint16_t base = ((bank ? ADDRESS_ROM_SWAP_BEGIN : ADDRESS_ROM_BEGIN) + *address);

	if(final) {
		uint32_t index = 0;

		for(; index < g_dasm.subroutine_count; ++index) {

			if(base == g_dasm.subroutine[index]) {
				break;
			}
		}

		if(index < g_dasm.subroutine_count) {

			if(bank > 1) {
				TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s%04x_%u%s\n", SUBROUTINE_PREFIX, base, bank, DELIMITER_LABEL);
			} else {
				TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s%04x%s\n", SUBROUTINE_PREFIX, base, DELIMITER_LABEL);
			}
		}
	}

	if((extended = ((opcode = ((uint8_t *)g_dasm.buffer.data)[origin + (*address)++]) == INSTRUCTION_EXTENDED_PREFIX))) {
		opcode = ((uint8_t *)g_dasm.buffer.data)[origin + (*address)++];
	}

	if(final) {
		format = dmg_tool_syntax_instruction_string(opcode, extended);
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "\t\t");
	}

	switch((instruction = dmg_tool_syntax_instruction(opcode, extended))->operand) {
		case OPERAND_BYTE:
			operand[0] = ((uint8_t *)g_dasm.buffer.data)[origin + (*address)++];

			if(final) {

				if(extended) {
					TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%04x {%02x %02x %02x}\n", DELIMITER_COMMENT, base, INSTRUCTION_EXTENDED_PREFIX,
						opcode, operand[0]);
				} else {
					TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%04x {%02x %02x}\n", DELIMITER_COMMENT, base, opcode, operand[0]);
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
					TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%04x {%02x %02x %02x %02x}\n", DELIMITER_COMMENT, base, INSTRUCTION_EXTENDED_PREFIX,
						opcode, operand[0], operand[1]);
				} else {
					TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%04x {%02x %02x %02x}\n", DELIMITER_COMMENT, base, opcode, operand[0], operand[1]);
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
					TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%04x {%02x %02x}\n", DELIMITER_COMMENT, base, INSTRUCTION_EXTENDED_PREFIX,
							opcode);
				} else {
					TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%c%04x {%02x}\n", DELIMITER_COMMENT, base, opcode);
				}

				TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\t%s\n", format);
			}
			break;
	}

	return result;
}

static int
dmg_utility_dasm_disassemble_header(
	__inout uint32_t *banks,
	__in bool final
	)
{
	int result = DMG_STATUS_SUCCESS;
	const dmg_cartridge_header_t *header;

	if(final) {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%cHeader [%04x-%04x]", DELIMITER_COMMENT, ADDRESS_HEADER_BEGIN, ADDRESS_HEADER_END);
	}

	header = (const dmg_cartridge_header_t *)&((uint8_t *)g_dasm.buffer.data)[ADDRESS_HEADER_BEGIN];
	*banks = ROM_BANK[header->rom];

	if(final) {
		int index = 0;

		for(uint16_t address = ADDRESS_HEADER_BEGIN; address <= ADDRESS_HEADER_END; address += HEADER_LEN[index++]) {
			uint16_t offset;
			int count = 0;
			char str[HEADER_WIDTH + 1] = {};

			TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\n%s\n\t%s %s%04x\n", dmg_tool_syntax_header_string(index),
					dmg_tool_syntax_directive_string(DIRECTIVE_ORIGIN), DELIMITER_HEXIDECIMAL, address);

			if(address >= 0x0104) {
				TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\t\t%c%04x\n\t%s", DELIMITER_COMMENT, address,
						dmg_tool_syntax_directive_string(DIRECTIVE_DATA_BYTE));

				for(offset = address; offset < (address + HEADER_LEN[index]); ++offset) {
					uint8_t value;

					if(!(count % HEADER_WIDTH)) {

						if(strlen(str)) {
							TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\n\t\t%c%s\n\t%s", DELIMITER_COMMENT, str,
									dmg_tool_syntax_directive_string(DIRECTIVE_DATA_BYTE));
							memset(str, 0, sizeof(str));
							count = 0;
						}
					}

					value = ((uint8_t *)g_dasm.buffer.data)[offset];
					TRACE_TOOL(g_dasm.file, LEVEL_MAX, " %s%02x", DELIMITER_HEXIDECIMAL, value);
					str[count++] = ((isprint((char)value) && !isspace((char)value)) ? value : CHARACTER_FILL);
				}

				if(strlen(str)) {
					TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\n\t\t%c%s", DELIMITER_COMMENT, str);
					memset(str, 0, sizeof(str));
				}

				TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "\n");
			} else {
				uint16_t entry_address = address;

				while(entry_address < 0x0104) {

					if((result = dmg_utility_dasm_disassemble_instruction(&entry_address, 0, 0, final)) != DMG_STATUS_SUCCESS) {
						goto exit;
					}
				}
			}
		}

		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%s", "\n");
	}

exit:
	return result;
}

static int
dmg_utility_dasm_disassemble_vectors(
	__in bool final
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(final) {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "%cVector table [%04x-%04x]", DELIMITER_COMMENT, ADDRESS_VECTOR_BEGIN, ADDRESS_HEADER_BEGIN - 1);
	}

	for(uint16_t address = ADDRESS_VECTOR_BEGIN; address < ADDRESS_HEADER_BEGIN;) {

		if(final && !(address % VECTOR_WIDTH) && (address <= ADDRESS_VECTOR_END)) {
			TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\n%s\n\t%s %s%04x\n", dmg_tool_syntax_vector_string(address / VECTOR_WIDTH),
					dmg_tool_syntax_directive_string(DIRECTIVE_ORIGIN), DELIMITER_HEXIDECIMAL, address);
		}

		if((result = dmg_utility_dasm_disassemble_instruction(&address, 0, 0, final)) != DMG_STATUS_SUCCESS) {
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
	int result = DMG_STATUS_SUCCESS;
	uint32_t origin = (bank ? ADDRESS_ROM_SWAP_BEGIN : ADDRESS_ROM_BEGIN);

	if(final) {
		TRACE_TOOL(g_dasm.file, LEVEL_MAX, "\n%cBank #%u\n\t%s %u\n\t%s %s%04x\n\n", DELIMITER_COMMENT, bank,
			dmg_tool_syntax_directive_string(DIRECTIVE_BANK), bank, dmg_tool_syntax_directive_string(DIRECTIVE_ORIGIN),
			DELIMITER_HEXIDECIMAL, origin);
	}

	if(!bank) {

		if((result = dmg_utility_dasm_disassemble_vectors(final)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((result = dmg_utility_dasm_disassemble_header(banks, final)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		origin = 0;
		address = (ADDRESS_HEADER_END + 1);
	} else {
		origin = (bank * BANK_WIDTH);
		address = 0;
	}

	for(; address < BANK_WIDTH;) {

		if((result = dmg_utility_dasm_disassemble_instruction(&address, origin, bank, final)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

exit:
	return result;
}

static int
dmg_utility_dasm_disassemble(void)
{
	int result = DMG_STATUS_SUCCESS;
	uint32_t bank = 0, banks, length = 0;

	TRACE_TOOL_MESSAGE("%s -- %.02f KB (%u bytes)\n\n", g_dasm.rom, g_dasm.buffer.length / (float)KBYTE, g_dasm.buffer.length);

	if(g_dasm.buffer.length <= ADDRESS_HEADER_END) {
		TRACE_TOOL_ERROR("File is too small -- %.02f KB (%u bytes) (expecting > %.02f KB (%i bytes))\n", g_dasm.buffer.length / (float)KBYTE,
			g_dasm.buffer.length, ADDRESS_HEADER_END / (float)KBYTE, ADDRESS_HEADER_END);
		result = DMG_STATUS_FAILURE;
		goto exit;
	}

	if((result = dmg_utility_dasm_disassemble_comment()) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("Parsing bank[%u]...      ", bank);

	if((result = dmg_utility_dasm_disassemble_bank(&banks, bank, false)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("%s", "[Done]\n");

	for(bank = 1; bank < banks; ++bank) {
		TRACE_TOOL_MESSAGE("Parsing bank[%u]...      ", bank);

		if((result = dmg_utility_dasm_disassemble_bank(&banks, bank, false)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		TRACE_TOOL_MESSAGE("%s", "[Done]\n");
	}

	TRACE_TOOL_MESSAGE("\nFound %u subroutines\n\n", g_dasm.subroutine_count);
	bank = 0;

	TRACE_TOOL_MESSAGE("Writing bank[%u]...      ", bank);

	if((result = dmg_utility_dasm_disassemble_bank(&banks, bank, true)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	TRACE_TOOL_MESSAGE("%s", "[Done]\n");

	for(bank = 1; bank < banks; ++bank) {
		TRACE_TOOL_MESSAGE("Writing bank[%u]...      ", bank);

		if((result = dmg_utility_dasm_disassemble_bank(&banks, bank, true)) != DMG_STATUS_SUCCESS) {
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
dmg_utility_dasm_output_load(
	__in const char *path
	)
{
	char path_full[PATH_MAX] = {};
	int length = 0, result = DMG_STATUS_SUCCESS;

	if(!g_dasm.output || !strlen(g_dasm.output)) {
		char *path_end;

		if((path_end = strrchr(path, PATH_DELIMITER))) {
			memcpy(path_full, path, (path_end - path) + 1);
		}

		strcat(path_full, PATH_OUTPUT);
	} else {
		strcpy(path_full, g_dasm.output);
	}

	if((result = dmg_tool_file_open(path_full, false, true, &g_dasm.file, &length)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static void
dmg_utility_dasm_output_unload(void)
{
	dmg_tool_file_close(&g_dasm.file);
}

static int
dmg_utility_dasm_parse(
	__in int argc,
	__in char *argv[]
	)
{
	int option, result = DMG_STATUS_SUCCESS;

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
				result = DMG_STATUS_FAILURE;
				goto exit;
			default:
				result = DMG_STATUS_FAILURE;
				goto exit;
		}
	}

	if(!g_dasm.output) {
		TRACE_TOOL_ERROR("%s: Missing output path -- %s\n", argv[0], g_dasm.output);
		result = DMG_STATUS_INVALID;
		goto exit;
	} else if(!g_dasm.rom) {
		TRACE_TOOL_ERROR("%s: Missing rom path -- %s\n", argv[0], g_dasm.rom);
		result = DMG_STATUS_INVALID;
		goto exit;
	}

exit:
	return result;
}

static int
dmg_utility_dasm_rom_load(void)
{
	FILE *file = NULL;
	int length = 0, result = DMG_STATUS_SUCCESS;

	if((result = dmg_tool_file_open(g_dasm.rom, true, false, &file, &length)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(!(g_dasm.buffer.data = (void *)calloc(length, sizeof(uint8_t)))) {
		result = DMG_STATUS_FAILURE;
		goto exit;
	}

	if(fread(g_dasm.buffer.data, sizeof(uint8_t), length, file) != length) {
		result = DMG_STATUS_FAILURE;
		goto exit;
	}

	g_dasm.buffer.length = length;

exit:
	dmg_tool_file_close(&file);

	return result;
}

static void
dmg_utility_dasm_rom_unload(void)
{

	if(g_dasm.buffer.data) {
		free(g_dasm.buffer.data);
	}

	memset(&g_dasm.buffer, 0, sizeof(g_dasm.buffer));
}

int
main(
	__in int argc,
	__in char *argv[]
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((result = dmg_utility_dasm_parse(argc, argv)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(g_dasm.help) {
		dmg_tool_usage(stdout, true, DMG_USAGE, FLAG_STR, FLAG_DESCRIPTION_STR, FLAG_MAX);
	} else if(g_dasm.version) {
		dmg_tool_version(stdout, false);
	} else {

		if((result = dmg_utility_dasm_rom_load()) != DMG_STATUS_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to load rom file -- %s\n", argv[0], g_dasm.rom);
			goto exit;
		}

		if((result = dmg_utility_dasm_output_load(argv[0])) != DMG_STATUS_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to create output file -- %s\n", argv[0], g_dasm.output);
			goto exit;
		}

		if((result = dmg_utility_dasm_disassemble()) != DMG_STATUS_SUCCESS) {
			TRACE_TOOL_ERROR("%s: Failed to assemble rom file -- %s\n", argv[0], g_dasm.rom);
			goto exit;
		}
	}

exit:
	dmg_utility_dasm_output_unload();
	dmg_utility_dasm_rom_unload();
	memset(&g_dasm, 0, sizeof(g_dasm));

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
