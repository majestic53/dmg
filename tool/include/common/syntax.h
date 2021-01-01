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

#ifndef DMG_TOOL_COMMON_SYNTAX_H_
#define DMG_TOOL_COMMON_SYNTAX_H_

#include "../../../include/system/processor/instruction.h"

enum {
	CHARACTER_ESCAPE_BACKSLASH = 0,
	CHARACTER_ESCAPE_NEWLINE,
	CHARACTER_ESCAPE_QUOTATION,
	CHARACTER_ESCAPE_QUOTATION_DOUBLE,
	CHARACTER_ESCAPE_TAB,
	CHARACTER_ESCAPE_MAX,
};

enum {
	DIRECTIVE_BANK = 0,
	DIRECTIVE_DATA_BYTE,
	DIRECTIVE_DEFINED,
	DIRECTIVE_DATA_WORD,
	DIRECTIVE_ELSE_IF,
	DIRECTIVE_ELSE,
	DIRECTIVE_END,
	DIRECTIVE_IF,
	DIRECTIVE_IF_DEFINED,
	DIRECTIVE_INCLUDE,
	DIRECTIVE_INCLUDE_BINARY,
	DIRECTIVE_ORIGIN,
	DIRECTIVE_RESERVE,
	DIRECTIVE_UNDEFINED,
	DIRECTIVE_MAX,
};

enum {
	HEADER_ENTRY = 0,
	HEADER_LOGO,
	HEADER_TITLE,
	HEADER_MANUFACTURER,
	HEADER_COLOR_GAMEBOY,
	HEADER_LICENSEE,
	HEADER_SUPER_GAMEBOY,
	HEADER_MAPPER,
	HEADER_ROM,
	HEADER_RAM,
	HEADER_DESTINATION,
	HEADER_LICENSEE_OLD,
	HEADER_VERSION,
	HEADER_CHECKSUM,
	HEADER_CHECKSUM_GLOBAL,
	HEADER_MAX,
};

enum {
	MACRO_HIGH = 0,
	MACRO_LOW,
	MACRO_MAX,
};

enum {
	OPCODE_ADC = 0,
	OPCODE_ADD,
	OPCODE_AND,
	OPCODE_CALL,
	OPCODE_CCF,
	OPCODE_CP,
	OPCODE_CPL,
	OPCODE_DAA,
	OPCODE_DEC,
	OPCODE_DI,
	OPCODE_EI,
	OPCODE_HALT,
	OPCODE_INC,
	OPCODE_JP,
	OPCODE_JR,
	OPCODE_LD,
	OPCODE_NOP,
	OPCODE_OR,
	OPCODE_POP,
	OPCODE_PUSH,
	OPCODE_RET,
	OPCODE_RETI,
	OPCODE_RLA,
	OPCODE_RLCA,
	OPCODE_RRA,
	OPCODE_RRCA,
	OPCODE_RST,
	OPCODE_SCF,
	OPCODE_SBC,
	OPCODE_STOP,
	OPCODE_SUB,
	OPCODE_XOR,
	OPCODE_UNUSED_CB,
	OPCODE_UNUSED_D3,
	OPCODE_UNUSED_DB,
	OPCODE_UNUSED_DD,
	OPCODE_UNUSED_E3,
	OPCODE_UNUSED_E4,
	OPCODE_UNUSED_EB,
	OPCODE_UNUSED_EC,
	OPCODE_NUSED_ED,
	OPCODE_UNUSED_F4,
	OPCODE_UNUSED_FC,
	OPCODE_UNUSED_FD,
	OPCODE_BIT0,
	OPCODE_BIT1,
	OPCODE_BIT2,
	OPCODE_BIT3,
	OPCODE_BIT4,
	OPCODE_BIT5,
	OPCODE_BIT6,
	OPCODE_BIT7,
	OPCODE_RES0,
	OPCODE_RES1,
	OPCODE_RES2,
	OPCODE_RES3,
	OPCODE_RES4,
	OPCODE_RES5,
	OPCODE_RES6,
	OPCODE_RES7,
	OPCODE_RL,
	OPCODE_RLC,
	OPCODE_RR,
	OPCODE_RRC,
	OPCODE_SET0,
	OPCODE_SET1,
	OPCODE_SET2,
	OPCODE_SET3,
	OPCODE_SET4,
	OPCODE_SET5,
	OPCODE_SET6,
	OPCODE_SET7,
	OPCODE_SLA,
	OPCODE_SRA,
	OPCODE_SRL,
	OPCODE_SWAP,
	OPCODE_MAX,
};

enum {
	OPERATOR_ARITHMETIC_ADD = 0,
	OPERATOR_ARITHMETIC_DIVIDE,
	OPERATOR_ARITHMETIC_MODULUS,
	OPERATOR_ARITHMETIC_MULTIPLY,
	OPERATOR_ARITHMETIC_SUBTRACT,
	OPERATOR_BINARY_AND,
	OPERATOR_BINARY_OR,
	OPERATOR_BINARY_XOR,
	OPERATOR_CONDITIONAL_EQUALS,
	OPERATOR_CONDITIONAL_GREATER_THAN,
	OPERATOR_CONDITIONAL_GREATER_THAN_EQUALS,
	OPERATOR_CONDITIONAL_LESS_THAN,
	OPERATOR_CONDITIONAL_LESS_THAN_EQUALS,
	OPERATOR_CONDITIONAL_NOT_EQUALS,
	OPERATOR_LOGICAL_AND,
	OPERATOR_LOGICAL_OR,
	OPERATOR_LOGICAL_SHIFT_LEFT,
	OPERATOR_LOGICAL_SHIFT_RIGHT,
	OPERATOR_UNARY_NEGATE,
	OPERATOR_UNARY_NOT,
	OPERATOR_MAX,
};

enum {
	REGISTER_A = 0,
	REGISTER_AF,
	REIGSTER_B,
	REGISTER_BC,
	REGISTER_C,
	REGISTER_D,
	REGISTER_DE,
	REGISTER_E,
	REGISTER_H,
	REGISTER_HL,
	REGISTER_L,
	REGISTER_PC,
	REGISTER_SP,
	REGISTER_MAX,
};

enum {
	SYMBOL_BRACE_CLOSE = 0,
	SYMBOL_BRACE_OPEN,
	SYMBOL_SEPERATOR,
	SYMBOL_MAX,
};

enum {
	VECTOR_RST_00 = 0,
	VECTOR_RST_08,
	VECTOR_RST_10,
	VECTOR_RST_18,
	VECTOR_RST_20,
	VECTOR_RST_28,
	VECTOR_RST_30,
	VECTOR_RST_38,
	VECTOR_VBLANK,
	VECTOR_LCDC,
	VECTOR_TIMER,
	VECTOR_SERIAL,
	VECTOR_JOYPAD,
	VECTOR_MAX,
};

typedef struct {
	uint8_t opcode;
	uint8_t operand;
} dmg_tool_syntax_instruction_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

const char *dmg_tool_syntax_directive_string(
	__in int type
	);

const char *dmg_tool_syntax_header_string(
	__in int type
	);

const dmg_tool_syntax_instruction_t *dmg_tool_syntax_instruction(
	__in uint8_t opcode,
	__in bool extended
	);

const char *dmg_tool_syntax_instruction_string(
	__in uint8_t opcode,
	__in bool extended
	);

bool dmg_tool_syntax_is_escape_character(
	__in const char ch,
	__inout int *type
	);

bool dmg_tool_syntax_is_directive_string(
	__in const char *str,
	__inout int *type
	);

bool dmg_tool_syntax_is_macro_string(
	__in const char *str,
	__inout int *type
	);

bool dmg_tool_syntax_is_opcode_string(
	__in const char *str,
	__inout int *type
	);

bool dmg_tool_syntax_is_operator_string(
	__in const char *str,
	__inout int *type
	);

bool dmg_tool_syntax_is_register_string(
	__in const char *str,
	__inout int *type
	);

bool dmg_tool_syntax_is_symbol_string(
	__in const char *str,
	__inout int *type
	);

const char *dmg_tool_syntax_mapper_string(
	__in int type
	);

const char *dmg_tool_syntax_ram_string(
	__in int type
	);

const char *dmg_tool_syntax_rom_string(
	__in int type
	);

const char *dmg_tool_syntax_vector_string(
	__in int type
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TOOL_COMMON_SYNTAX_H_ */
