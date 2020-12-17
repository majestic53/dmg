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

#ifndef DMG_COMMON_MAPPER_MBC2_TYPE_H_
#define DMG_COMMON_MAPPER_MBC2_TYPE_H_

#include "../../../include/common/mapper/mbc2.h"
#include "../../../include/system/processor.h"
#include "../mapper_type.h"

#define MBC2_RAM_BEGIN 0x0000
#define MBC2_RAM_END 0x01ff
#define MBC2_RAM_MASK 0xf0

#define MBC2_RAM_WIDTH ADDRESS_WIDTH(MBC2_RAM_BEGIN, MBC2_RAM_END)

#endif /* DMG_COMMON_MAPPER_MBC2_TYPE_H_ */
