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

#ifndef DMG_TYPE_MAPPER_TYPE_H_
#define DMG_TYPE_MAPPER_TYPE_H_

#include "../../include/type/mapper/mbc1.h"
#include "../../include/type/mapper/mbc2.h"
#include "../../include/type/mapper/mbc3.h"
#include "../../include/type/mapper/mbc5.h"
#include "../../include/type/mapper.h"
#include "../../include/service.h"

enum {
	MAPPER_ROM_ONLY = 0,
	MAPPER_MBC1,
	MAPPER_MBC1_RAM,
	MAPPER_MBC1_RAM_BATTERY,
	MAPPER_MBC2 = 0x05,
	MAPPER_MBC2_BATTERY,
	MAPPER_ROM_RAM = 0x08,
	MAPPER_ROM_RAM_BATTERY,
	MAPPER_MBC3_TIMER_BATTERY = 0x0f,
	MAPPER_MBC3_TIMER_RAM_BATTERY,
	MAPPER_MBC3,
	MAPPER_MBC3_RAM,
	MAPPER_MBC3_RAM_BATTERY,
	MAPPER_MBC5 = 0x19,
	MAPPER_MBC5_RAM,
	MAPPER_MBC5_RAM_BATTERY,
	MAPPER_MBC5_RUMBLE,
	MAPPER_MBC5_RUMBLE_RAM,
	MAPPER_MBC5_RUMBLE_RAM_BATTERY,
	MAPPER_MAX,
};

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
