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

#ifndef DMG_COMMON_MAPPER_MBC1_TYPE_H_
#define DMG_COMMON_MAPPER_MBC1_TYPE_H_

#include "../../../include/common/mapper/mbc1.h"
#include "../mapper_type.h"

#define MBC1_BANK_00 0x00
#define MBC1_BANK_20 0x20
#define MBC1_BANK_40 0x40
#define MBC1_BANK_60 0x60

enum {
	MBC1_MODE_ROM = 0,
	MBC1_MODE_RAM,
	MBC1_MODE_MAX,
};

#endif /* DMG_COMMON_MAPPER_MBC1_TYPE_H_ */
