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

#include "./usage_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
dmg_tool_usage(
	__in FILE *stream,
	__in bool verbose,
	__in const char *usage,
	__in const char **flags,
	__in const char **descriptions,
	__in int count
	)
{

	if(verbose) {
		dmg_tool_version(stream, verbose);
		TRACE_TOOL(stream, LEVEL_NONE, "%s", "\n");
	}

	TRACE_TOOL(stream, LEVEL_NONE, "%s\n", usage);

	if(verbose) {

		for(int flag = 0; flag < count; ++flag) {
			TRACE_TOOL(stream, LEVEL_NONE, "\n%s\t%s", flags[flag], descriptions[flag]);
		}

		TRACE_TOOL(stream, LEVEL_NONE, "%s", "\n");
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
