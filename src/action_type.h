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

#ifndef DMG_ACTION_TYPE_H_
#define DMG_ACTION_TYPE_H_

#include "./runtime_type.h"

typedef int (*dmg_action_hdlr)(
	__in const dmg_action_t *request,
	__in dmg_action_t *response,
	__in dmg_runtime_t *runtime
	);

#endif /* DMG_ACTION_TYPE_H_ */
