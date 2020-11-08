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

#ifndef DMG_TYPE_CARTRIDGE_TYPE_H_
#define DMG_TYPE_CARTRIDGE_TYPE_H_

#include "../../include/type/cartridge.h"
#include "../../include/service.h"

#define RAM_WIDTH ADDRESS_WIDTH(ADDRESS_RAM_BEGIN, ADDRESS_RAM_END)
#define ROM_WIDTH ADDRESS_WIDTH(ADDRESS_ROM_BEGIN, ADDRESS_ROM_END)

enum {
	RAM_NONE = 0,
	RAM_2KB,
	RAM_8KB,
	RAM_32KB,
	RAM_128KB,
	RAM_MAX,
};

static const size_t RAM_BANK[] = {
	1, /* RAM_NONE */
	1, /* RAM_2KB */
	1, /* RAM_8KB */
	4, /* RAM_32KB */
	16, /* RAM_128KB */
	0, /* RAM_MAX */
	};

enum {
	ROM_32KB = 0,
	ROM_64KB,
	ROM_128KB,
	ROM_256KB,
	ROM_512KB,
	ROM_1MB,
	ROM_2MB,
	ROM_4MB,
	ROM_8MB,
	ROM_MAX,
};

static const size_t ROM_BANK[] = {
	2, /* ROM_32KB */
	4, /* ROM_64KB */
	8, /* ROM_128KB */
	16, /* ROM_256KB */
	32, /* ROM_512KB */
	64, /* ROM_1MB */
	128, /* ROM_2MB */
	256, /* ROM_4MB */
	512, /* ROM_8MB */
	0, /* ROM_MAX */
	};

#endif /* DMG_TYPE_CARTRIDGE_TYPE_H_ */
