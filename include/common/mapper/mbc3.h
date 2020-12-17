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

#ifndef DMG_COMMON_MAPPER_MBC3_H_
#define DMG_COMMON_MAPPER_MBC3_H_

#include "../mapper.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uint8_t dmg_mapper_mbc3_read_ram(
	__in const dmg_mapper_t *mapper,
	__in uint16_t address
	);

void dmg_mapper_mbc3_write_ram(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	);

void dmg_mapper_mbc3_write_rom(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_COMMON_MAPPER_MBC3_H_ */
