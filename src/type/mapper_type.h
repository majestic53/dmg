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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DMG_TYPE_MAPPER_TYPE_H_
#define DMG_TYPE_MAPPER_TYPE_H_

#include "../../include/type/mapper.h"
#include "../../include/service.h"

enum {
	MAPPER_ROM_ONLY = 0,
	MAPPER_MBC1,
	MAPPER_MBC1_RAM,
	MAPPER_MBC1_RAM_BATTERY,
	MAPPER_MBC3_RAM = 0x12,
	MAPPER_MBC3_RAM_BATTERY,
	MAPPER_MBC5 = 0x19,
	MAPPER_MBC5_RAM,
	MAPPER_MBC5_RAM_BATTERY,
	MAPPER_MBC5_RUMBLE,
	MAPPER_MBC5_RUMBLE_RAM,
	MAPPER_MBC5_RUMBLE_RAM_BATTERY,
	MAPPER_MAX,
};

#define MBC1_BANK_00 0x00
#define MBC1_BANK_20 0x20
#define MBC1_BANK_40 0x40
#define MBC1_BANK_60 0x60

enum {
	MBC1_MODE_ROM = 0,
	MBC1_MODE_RAM,
	MBC1_MODE_MAX,
};

#define MBC3_RAM_MASK 0x03

#define MBC5_RAM_MASK 0x0f
#define MBC5_ROM_MASK 0x01ff

#define RAM_ENABLE 0x0a

#ifndef NDEBUG
#define TRACE_MAPPER(_LEVEL_, _MAPPER_) \
	if((_LEVEL_) <= (LEVEL)) { \
		dmg_mapper_trace(_LEVEL_, _MAPPER_); \
	}
#else
#define TRACE_MAPPER(_LEVEL_, _MAPPER_)
#endif /* NDEBUG */

#endif /* DMG_TYPE_MAPPER_TYPE_H_ */
