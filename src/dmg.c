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

#include "./dmg_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg(
	__in const dmg_t *configuration
	)
{
	return (dmg_runtime(configuration) == ERROR_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE;
}

const char *
dmg_error(void)
{
	return dmg_error_get();
}

const dmg_version_t *
dmg_version(void)
{
	return dmg_version_get();
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
