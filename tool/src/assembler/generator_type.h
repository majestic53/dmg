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

#ifndef DMG_TOOL_ASSEMBLER_GENERATOR_TYPE_H_
#define DMG_TOOL_ASSEMBLER_GENERATOR_TYPE_H_

#include "../../include/assembler/generator.h"

#define GENERATOR_ERROR_STR_MAX 256

#define ORIGIN_MAX ((BANK_WIDTH * 2) - 1)

enum {
	ADDRESS_RST_00 = 0x0000,
	ADDRESS_RST_08 = 0x0008,
	ADDRESS_RST_10 = 0x0010,
	ADDRESS_RST_18 = 0x0018,
	ADDRESS_RST_20 = 0x0020,
	ADDRESS_RST_28 = 0x0028,
	ADDRESS_RST_30 = 0x0030,
	ADDRESS_RST_38 = 0x0038,
};

#define GENERATOR_ERROR(_PARSER_, _TREE_, _MESSAGE_) \
	dmg_assembler_generator_error(_PARSER_, _TREE_, _MESSAGE_)

typedef int (*dmg_assembler_evaluator_hdlr)(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree,
	__inout dmg_assembler_scalar_t *value
	);

typedef int (*dmg_assembler_generator_hdlr)(
	__inout dmg_assembler_generator_t *generator,
	__in const dmg_assembler_parser_t *parser,
	__in const dmg_assembler_tree_t *tree
	);

#endif /* DMG_TOOL_ASSEMBLER_GENERATOR_TYPE_H_ */
