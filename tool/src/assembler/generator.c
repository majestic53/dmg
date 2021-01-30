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
