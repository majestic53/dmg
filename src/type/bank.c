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

#include "./bank_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_bank_load(
	__inout dmg_bank_t *bank,
	__in uint32_t count
	)
{
	int result = ERROR_SUCCESS;
	uint32_t length = (count * sizeof(dmg_buffer_t));

	if(!(bank->buffer = (dmg_buffer_t *)malloc(length))) {
		result = ERROR_SET(ERROR_FAILURE, "Failed to allocate bank");
		goto exit;
	}

	memset(bank->buffer, 0, length);
	bank->count = count;

exit:
	return result;
}

void
dmg_bank_unload(
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
