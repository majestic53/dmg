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

#ifndef DMG_TYPE_MAPPER_MBC3_TYPE_H_
#define DMG_TYPE_MAPPER_MBC3_TYPE_H_

#include "../../../include/type/mapper/mbc3.h"
#include "../mapper_type.h"

#define MBC3_RAM_MASK 0x03
#define MBC3_RTC_LATCH 0x01

enum {
	MBC3_MODE_RAM_0 = 0,
	MBC3_MODE_RAM_7 = 7,
	MBC3_MODE_RTC_SEC,
	MBC3_MODE_RTC_MIN,
	MBC3_MODE_RTC_HOUR,
	MBC3_MODE_RTC_DAY_LOW,
	MBC3_MODE_RTC_DAY_HIGH,
};

#endif /* DMG_TYPE_MAPPER_MBC3_TYPE_H_ */
