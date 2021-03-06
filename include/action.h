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

#ifndef DMG_ACTION_H_
#define DMG_ACTION_H_

#include "./common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmg_action_handler(
	__in const dmg_action_t *request,
	__in dmg_action_t *response,
	__in void *context
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_ACTION_H_ */
