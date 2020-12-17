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

#ifndef DMG_COMMON_BOOTROM_TYPE_H_
#define DMG_COMMON_BOOTROM_TYPE_H_

#include "../../include/common/bootrom.h"
#include "../../include/service.h"

#define BOOTROM_CHECKSUM 0x626e
#define BOOTROM_WIDTH ADDRESS_WIDTH(ADDRESS_BOOTROM_BEGIN, ADDRESS_BOOTROM_END)

#endif /* DMG_COMMON_BOOTROM_TYPE_H_ */
