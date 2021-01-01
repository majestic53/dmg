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

#include "./version_type.h"

static dmg_version_t g_version = { VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH };

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

const dmg_version_t *
dmg_version_get(void)
{
	return &g_version;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
