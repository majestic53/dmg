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

#include "./generator_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NDEBUG

static int
dmg_assembler_generator_error_tree(
	__in const dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree,
	__inout dmg_assembler_string_t *string,
	__in uint32_t depth
	)
{
	int result = DMG_STATUS_SUCCESS;
	char str[GENERATOR_ERROR_STR_MAX] = {};

	if((result = dmg_assembler_string_append_character(string, '\n')) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	for(uint32_t index = 0; index < depth; ++index) {

		if((result = dmg_assembler_string_append_character(string, '\t')) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	if(tree && tree->parent) {
		const dmg_assembler_token_t *token = tree->parent;

		snprintf(str, GENERATOR_ERROR_STR_MAX, "{%u} [%i", tree->count, token->type);

		if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(token->subtype != TOKEN_SUBTYPE_UNDEFINED) {
			snprintf(str, GENERATOR_ERROR_STR_MAX, ":%i", token->subtype);

			if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}

		if((result = dmg_assembler_string_append_string(string, "] ")) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if((token->type > TOKEN_END) && (token->type < TOKEN_MAX)) {

			if((result = dmg_assembler_string_append_character(string, '\"')) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			for(uint32_t index = 0; index < token->literal.length; ++index) {

				if((result = dmg_assembler_string_append_character(string, token->literal.str[index]))
						!= DMG_STATUS_SUCCESS) {
					goto exit;
				}
			}

			if((result = dmg_assembler_string_append_character(string, '\"')) != DMG_STATUS_SUCCESS) {
				goto exit;
			}

			if(token->type == TOKEN_SCALAR) {
				snprintf(str, GENERATOR_ERROR_STR_MAX, " %04x (%u)", token->scalar.word, token->scalar.word);

				if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
			}
		} else {
			snprintf(str, GENERATOR_ERROR_STR_MAX, " \'%c\' (%02x)", (isprint(token->scalar.low) && !isspace(token->scalar.low))
				? token->scalar.low : CHARACTER_FILL, token->scalar.low);

			if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}

		snprintf(str, GENERATOR_ERROR_STR_MAX, " (%s@%u)", generator->parser.lexer.stream.path, token->line);

		if((result = dmg_assembler_string_append_string(string, str)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		for(uint32_t index = 0; index < tree->count; ++index) {

			if((result = dmg_assembler_generator_error_tree(generator, (const dmg_assembler_tree_t *)tree->child[index], string, depth + 1))
					!= DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}
	}

exit:
	return result;
}

#endif /* NDEBUG */

static int
dmg_assembler_generator_error(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree,
	__in const char *message,
	...
	)
{
	dmg_assembler_string_t string = {};

	if(dmg_assembler_string_allocate(&string) == DMG_STATUS_SUCCESS) {

		if(tree->parent->literal.length) {

			for(uint32_t index = 0; index < tree->parent->literal.length; ++index) {

				if(dmg_assembler_string_append_character(&string, tree->parent->literal.str[index]) != DMG_STATUS_SUCCESS) {
					break;
				}
			}
		} else {
			strcpy(string.str, "EOF");
		}
	}

#ifndef NDEBUG
	dmg_assembler_string_t debug_string = {};

	if(dmg_assembler_string_allocate(&debug_string) == DMG_STATUS_SUCCESS) {
		dmg_assembler_generator_error_tree(generator, tree, &debug_string, 1);
	}

	ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s \"%s\" (%s@%u)%s", message, string.str, generator->parser.lexer.stream.path, tree->parent->line, debug_string);
	dmg_assembler_string_free(&debug_string);
#else
	ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "%s \"%s\" (%s@%u)", message, string.str, generator->parser.lexer.stream.path, tree->parent->line);
#endif /* NDEBUG */
	dmg_assembler_string_free(&string);

	return DMG_STATUS_FAILURE;
}

/*static int
dmg_assembler_generator_bank_set(
	__inout dmg_assembler_generator_t *generator,
	__in uint32_t bank,
	__in const dmg_assembler_scalar_t *origin
	)
{
	int result = DMG_STATUS_SUCCESS;

	for(; (generator->banks.count - 1) < bank;) {

		if((result = dmg_assembler_bank_add(&generator->banks, origin)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}
	}

	generator->bank = bank;
	generator->offset.word = (origin % BANK_WIDTH);

exit:
	return result;
}*/

static int
dmg_assembler_generator_bank_set_byte(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_scalar_t *value
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((result = dmg_assembler_bank_set_byte(&generator->banks, generator->bank, &generator->offset, value))
			!= DMG_STATUS_SUCCESS) {
		goto exit;
	}

	generator->offset.word += sizeof(value->low);

exit:
	return result;
}

static int
dmg_assembler_generator_bank_set_word(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_scalar_t *value
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((result = dmg_assembler_bank_set_word(&generator->banks, generator->bank, &generator->offset, value))
			!= DMG_STATUS_SUCCESS) {
		goto exit;
	}

	generator->offset.word += sizeof(value->word);

exit:
	return result;
}

static dmg_assembler_generator_hdlr DIRECTIVE_HANDLER[] = {
	NULL, /* DIRECTIVE_BANK */
	NULL, /* DIRECTIVE_DATA_BYTE */
	NULL, /* DIRECTIVE_DATA_WORD */
	NULL, /* DIRECTIVE_DEFINE */
	NULL, /* DIRECTIVE_ELSE_IF */
	NULL, /* DIRECTIVE_ELSE */
	NULL, /* DIRECTIVE_END */
	NULL, /* DIRECTIVE_IF */
	NULL, /* DIRECTIVE_IF_DEFINE */
	NULL, /* DIRECTIVE_INCLUDE */
	NULL, /* DIRECTIVE_INCLUDE_BINARY */
	NULL, /* DIRECTIVE_ORIGIN */
	NULL, /* DIRECTIVE_RESERVE */
	NULL, /* DIRECTIVE_UNDEFINE */
	};

static int
dmg_assembler_generator_generate_directive(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_generator_hdlr handler;

	if((tree->parent->subtype >= DIRECTIVE_MAX)
			|| !(handler = DIRECTIVE_HANDLER[tree->parent->subtype])) {
		result = GENERATOR_ERROR(generator, tree, "Unsupported directive");
		goto exit;
	}

	if((result = handler(generator, tree)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generator_generate_label(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->parent->type != TOKEN_LABEL) {
		result = GENERATOR_ERROR(generator, tree, "Expecting label");
		goto exit;
	}

	if(dmg_assembler_global_get(&generator->globals, tree->parent, &value) == DMG_STATUS_SUCCESS) {
		result = GENERATOR_ERROR(generator, tree, "Duplicate label");
		goto exit;
	}

	value.word = (generator->banks.bank[generator->bank].origin.word + generator->offset.word);

	if((result = dmg_assembler_global_add(&generator->globals, tree->parent, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_instruction(
	__inout dmg_assembler_generator_t *generator,
	__in int instruction,
	__in bool extended,
	__in const dmg_assembler_scalar_t *operand
	)
{
	dmg_assembler_scalar_t value = {};
	int length, result = DMG_STATUS_SUCCESS;

	length = dmg_processor_instruction(instruction, extended)->operand;
	value.low = instruction;

	if(length > OPERAND_NONE) {
		value.high = operand->low;

		if((result = dmg_assembler_generator_bank_set_word(generator, &value)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		if(length > OPERAND_BYTE) {
			value.low = operand->high;

			if((result = dmg_assembler_generator_bank_set_byte(generator, &value)) != DMG_STATUS_SUCCESS) {
				goto exit;
			}
		}
	} else if((result = dmg_assembler_generator_bank_set_byte(generator, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_ccf(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->parent->subtype != OPCODE_CCF) {
		result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count) {
		result = GENERATOR_ERROR(generator, tree, "Unexpected parameters");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_CCF, false, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_cpl(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->parent->subtype != OPCODE_CPL) {
		result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count) {
		result = GENERATOR_ERROR(generator, tree, "Unexpected parameters");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_CPL, false, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_daa(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->parent->subtype != OPCODE_DAA) {
		result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count) {
		result = GENERATOR_ERROR(generator, tree, "Unexpected parameters");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_DAA, false, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_di(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->parent->subtype != OPCODE_DI) {
		result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count) {
		result = GENERATOR_ERROR(generator, tree, "Unexpected parameters");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_DI, false, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_ei(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->parent->subtype != OPCODE_EI) {
		result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count) {
		result = GENERATOR_ERROR(generator, tree, "Unexpected parameters");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_EI, false, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_halt(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->parent->subtype != OPCODE_HALT) {
		result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count) {
		result = GENERATOR_ERROR(generator, tree, "Unexpected parameters");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_HALT, false, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_nop(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->parent->subtype != OPCODE_NOP) {
		result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count) {
		result = GENERATOR_ERROR(generator, tree, "Unexpected parameters");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_NOP, false, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_reti(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->parent->subtype != OPCODE_RETI) {
		result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count) {
		result = GENERATOR_ERROR(generator, tree, "Unexpected parameters");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_RETI, false, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_rla(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->parent->subtype != OPCODE_RLA) {
		result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count) {
		result = GENERATOR_ERROR(generator, tree, "Unexpected parameters");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_RLA, false, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_rlca(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->parent->subtype != OPCODE_RLCA) {
		result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count) {
		result = GENERATOR_ERROR(generator, tree, "Unexpected parameters");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_RLCA, false, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_rra(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->parent->subtype != OPCODE_RRA) {
		result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count) {
		result = GENERATOR_ERROR(generator, tree, "Unexpected parameters");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_RRA, false, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_rrca(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->parent->subtype != OPCODE_RRCA) {
		result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count) {
		result = GENERATOR_ERROR(generator, tree, "Unexpected parameters");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_RRCA, false, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_scf(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->parent->subtype != OPCODE_SCF) {
		result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count) {
		result = GENERATOR_ERROR(generator, tree, "Unexpected parameters");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_SCF, false, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_stop(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t value = {};

	if(tree->parent->subtype != OPCODE_STOP) {
		result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
		goto exit;
	}

	if(tree->count) {
		result = GENERATOR_ERROR(generator, tree, "Unexpected parameters");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, INSTRUCTION_STOP, false, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static int
dmg_assembler_generate_opcode_unused(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	dmg_assembler_scalar_t value = {};
	int instruction, result = DMG_STATUS_SUCCESS;

	switch(tree->parent->subtype) {
		case OPCODE_UNUSED_D3:
			instruction = INSTRUCTION_UNUSED_D3;
			break;
		case OPCODE_UNUSED_DB:
			instruction = INSTRUCTION_UNUSED_DB;
			break;
		case OPCODE_UNUSED_DD:
			instruction = INSTRUCTION_UNUSED_DD;
			break;
		case OPCODE_UNUSED_E3:
			instruction = INSTRUCTION_UNUSED_E3;
			break;
		case OPCODE_UNUSED_E4:
			instruction = INSTRUCTION_UNUSED_E4;
			break;
		case OPCODE_UNUSED_EB:
			instruction = INSTRUCTION_UNUSED_EB;
			break;
		case OPCODE_UNUSED_EC:
			instruction = INSTRUCTION_UNUSED_EC;
			break;
		case OPCODE_UNUSED_ED:
			instruction = INSTRUCTION_UNUSED_ED;
			break;
		case OPCODE_UNUSED_F4:
			instruction = INSTRUCTION_UNUSED_F4;
			break;
		case OPCODE_UNUSED_FC:
			instruction = INSTRUCTION_UNUSED_FC;
			break;
		case OPCODE_UNUSED_FD:
			instruction = INSTRUCTION_UNUSED_FD;
			break;
		default:
			result = GENERATOR_ERROR(generator, tree, "Expecting opcode");
			goto exit;
	}

	if(tree->count) {
		result = GENERATOR_ERROR(generator, tree, "Unexpected parameters");
		goto exit;
	}

	if((result = dmg_assembler_generate_instruction(generator, instruction, false, &value)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

static dmg_assembler_generator_hdlr OPCODE_HANDLER[] = {
	NULL, /* OPCODE_ADC */
	NULL, /* OPCODE_ADD */
	NULL, /* OPCODE_AND */
	NULL, /* OPCODE_CALL */
	dmg_assembler_generate_opcode_ccf, /* OPCODE_CCF */
	NULL, /* OPCODE_CP */
	dmg_assembler_generate_opcode_cpl, /* OPCODE_CPL */
	dmg_assembler_generate_opcode_daa, /* OPCODE_DAA */
	NULL, /* OPCODE_DEC */
	dmg_assembler_generate_opcode_di, /* OPCODE_DI */
	dmg_assembler_generate_opcode_ei, /* OPCODE_EI */
	dmg_assembler_generate_opcode_halt, /* OPCODE_HALT */
	NULL, /* OPCODE_INC */
	NULL, /* OPCODE_JP */
	NULL, /* OPCODE_JR */
	NULL, /* OPCODE_LD */
	dmg_assembler_generate_opcode_nop, /* OPCODE_NOP */
	NULL, /* OPCODE_OR */
	NULL, /* OPCODE_POP */
	NULL, /* OPCODE_PUSH */
	NULL, /* OPCODE_RET */
	dmg_assembler_generate_opcode_reti, /* OPCODE_RETI */
	dmg_assembler_generate_opcode_rla, /* OPCODE_RLA */
	dmg_assembler_generate_opcode_rlca, /* OPCODE_RLCA */
	dmg_assembler_generate_opcode_rra, /* OPCODE_RRA */
	dmg_assembler_generate_opcode_rrca, /* OPCODE_RRCA */
	NULL, /* OPCODE_RST */
	dmg_assembler_generate_opcode_scf, /* OPCODE_SCF */
	NULL, /* OPCODE_SBC */
	dmg_assembler_generate_opcode_stop, /* OPCODE_STOP */
	NULL, /* OPCODE_SUB */
	NULL, /* OPCODE_XOR */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_CB */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_D3 */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_DB */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_DD */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_E3 */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_E4 */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_EB */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_EC */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_ED */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_F4 */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_FC */
	dmg_assembler_generate_opcode_unused, /* OPCODE_UNUSED_FD */
	NULL, /* OPCODE_BIT0 */
	NULL, /* OPCODE_BIT1 */
	NULL, /* OPCODE_BIT2 */
	NULL, /* OPCODE_BIT3 */
	NULL, /* OPCODE_BIT4 */
	NULL, /* OPCODE_BIT5 */
	NULL, /* OPCODE_BIT6 */
	NULL, /* OPCODE_BIT7 */
	NULL, /* OPCODE_RES0 */
	NULL, /* OPCODE_RES1 */
	NULL, /* OPCODE_RES2 */
	NULL, /* OPCODE_RES3 */
	NULL, /* OPCODE_RES4 */
	NULL, /* OPCODE_RES5 */
	NULL, /* OPCODE_RES6 */
	NULL, /* OPCODE_RES7 */
	NULL, /* OPCODE_RL */
	NULL, /* OPCODE_RLC */
	NULL, /* OPCODE_RR */
	NULL, /* OPCODE_RRC */
	NULL, /* OPCODE_SET0 */
	NULL, /* OPCODE_SET1 */
	NULL, /* OPCODE_SET2 */
	NULL, /* OPCODE_SET3 */
	NULL, /* OPCODE_SET4 */
	NULL, /* OPCODE_SET5 */
	NULL, /* OPCODE_SET6 */
	NULL, /* OPCODE_SET7 */
	NULL, /* OPCODE_SLA */
	NULL, /* OPCODE_SRA */
	NULL, /* OPCODE_SRL */
	NULL, /* OPCODE_SWAP */
	};

static int
dmg_assembler_generator_generate_opcode(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_tree_t *tree
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_generator_hdlr handler;

	if((tree->parent->subtype >= OPCODE_MAX)
			|| !(handler = OPCODE_HANDLER[tree->parent->subtype])) {
		result = GENERATOR_ERROR(generator, tree, "Unsupported opcode");
		goto exit;
	}

	if((result = handler(generator, tree)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

exit:
	return result;
}

int
dmg_assembler_generator_load(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_buffer_t *buffer,
	__in const char *path,
	__in FILE *file
	)
{
	int result;

	if((result = dmg_assembler_parser_load(&generator->parser, buffer, path)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_banks_allocate(&generator->banks)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_globals_allocate(&generator->globals)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	generator->file = file;

exit:
	return result;
}

int
dmg_assembler_generator_run(
	__inout dmg_assembler_generator_t *generator
	)
{
	int result = DMG_STATUS_SUCCESS;

	for(;;) {
		const dmg_assembler_tree_t *tree = dmg_assembler_parser_tree(&generator->parser);

		switch(tree->parent->type) {
			case TOKEN_DIRECTIVE:

				if((result = dmg_assembler_generator_generate_directive(generator, tree)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
				break;
			case TOKEN_LABEL:

				if((result = dmg_assembler_generator_generate_label(generator, tree)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
				break;
			case TOKEN_OPCODE:

				if((result = dmg_assembler_generator_generate_opcode(generator, tree)) != DMG_STATUS_SUCCESS) {
					goto exit;
				}
				break;
			default:
				result = GENERATOR_ERROR(generator, tree, "Expecting statement");
				goto exit;
		}

		if(!dmg_assembler_parser_has_next(&generator->parser)
				|| ((result = dmg_assembler_parser_next(&generator->parser)) != DMG_STATUS_SUCCESS)) {
			break;
		}
	}

	for(uint32_t index = 0; index < generator->banks.count; ++index) {
		dmg_assembler_bank_t *bank = &generator->banks.bank[index];

		if(fwrite(bank->data, sizeof(uint8_t), sizeof(bank->data), generator->file) != sizeof(bank->data)) {
			result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Failed to write bank %u to file", index);
			goto exit;
		}
	}

exit:
	return result;
}

void
dmg_assembler_generator_unload(
	__inout dmg_assembler_generator_t *generator
	)
{
	dmg_assembler_globals_free(&generator->globals);
	dmg_assembler_banks_free(&generator->banks);
	dmg_assembler_parser_unload(&generator->parser);
	memset(generator, 0, sizeof(*generator));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
