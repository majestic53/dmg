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

#include "./parser_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int
dmg_assembler_parser_tree_parse(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(dmg_assembler_lexer_has_next(&parser->lexer)) {

		if((result = dmg_assembler_trees_resize(&parser->trees)) != DMG_STATUS_SUCCESS) {
			goto exit;
		}

		// TODO: PARSE TOKEN INTO TREES

		++parser->trees.count;
	}

exit:
	return result;
}

bool
dmg_assembler_parser_has_next(
	__in const dmg_assembler_parser_t *parser
	)
{
	return ((parser->position < parser->trees.count)
		|| (((parser->position + 1) == parser->trees.count) && dmg_assembler_lexer_has_next(&parser->lexer)));
}

bool
dmg_assembler_parser_has_previous(
	__in const dmg_assembler_parser_t *parser
	)
{
	return (parser->position > 0);
}

int
dmg_assembler_parser_load(
	__inout dmg_assembler_parser_t *parser,
	__in const dmg_buffer_t *buffer,
	__in const char *path
	)
{
	int result;

	if((result = dmg_assembler_lexer_load(&parser->lexer, buffer, path)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if((result = dmg_assembler_trees_allocate(&parser->trees)) != DMG_STATUS_SUCCESS) {
		goto exit;
	}

	if(dmg_assembler_lexer_has_next(&parser->lexer)) {
		result = dmg_assembler_parser_tree_parse(parser);
	}

exit:
	return result;
}

int
dmg_assembler_parser_next(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	if((++parser->position == parser->trees.count)
			&& ((result = dmg_assembler_parser_tree_parse(parser)) != DMG_STATUS_SUCCESS)) {
		goto exit;
	}

exit:
	return result;
}

int
dmg_assembler_parser_previous(
	__inout dmg_assembler_parser_t *parser
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!dmg_assembler_parser_has_previous(parser)) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "No previous tree %u", parser->position);
		goto exit;
	}

	--parser->position;

exit:
	return result;
}

const
dmg_assembler_tree_t *dmg_assembler_parser_tree(
	__in const dmg_assembler_parser_t *parser
	)
{
	return &parser->trees.tree[parser->position];
}

const
dmg_assembler_tree_t *dmg_assembler_parser_tree_child(
	__in const dmg_assembler_parser_t *parser,
	__in uint32_t position
	)
{
	return dmg_assembler_tree_child(dmg_assembler_parser_tree(parser), position);
}

void
dmg_assembler_parser_unload(
	__inout dmg_assembler_parser_t *parser
	)
{
	dmg_assembler_trees_free(&parser->trees);
	dmg_assembler_lexer_unload(&parser->lexer);
	memset(parser, 0, sizeof(*parser));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
