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

#ifndef DMG_TOOL_ASSEMBLER_COMMON_GLOBAL_TYPE_H_
#define DMG_TOOL_ASSEMBLER_COMMON_GLOBAL_TYPE_H_

#include "../../../include/assembler/common/global.h"
#include "../../../include/assembler/common/string.h"

#define GLOBAL_CAPACITY_INIT 128
#define GLOBAL_CAPACITY_SCALE 2

#define GLOBAL_ERROR(_TOKEN_, _MESSAGE_) \
	dmg_assembler_global_error(_TOKEN_, _MESSAGE_)

#endif /* DMG_TOOL_ASSEMBLER_COMMON_GLOBAL_TYPE_H_ */
