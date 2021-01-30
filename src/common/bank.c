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

#include "./bank_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_bank_allocate(
	__inout dmg_bank_t *bank,
	__in uint32_t count
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!(bank->buffer = (dmg_buffer_t *)calloc(count, sizeof(dmg_buffer_t)))) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to allocate bank");
		goto exit;
	}

	bank->count = count;

exit:
	return result;
}

void
dmg_bank_free(
	__inout dmg_bank_t *bank
	)
{

	if(bank->buffer) {
		free(bank->buffer);
	}

	memset(bank, 0, sizeof(*bank));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
