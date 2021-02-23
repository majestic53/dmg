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
dmg_assembler_bank_add(
	__inout dmg_assembler_banks_t *banks,
	__in const dmg_assembler_scalar_t *origin
	)
{
	int result = DMG_STATUS_SUCCESS;

	if(!(banks->bank = (dmg_assembler_bank_t *)realloc(banks->bank, sizeof(dmg_assembler_bank_t) * (banks->count + 1)))) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to reallocate bank buffer");
		goto exit;
	}

	memset(&banks->bank[banks->count], 0, sizeof(dmg_assembler_bank_t));
	banks->bank[banks->count].origin.word = origin->word;
	++banks->count;

exit:
	return result;
}

int
dmg_assembler_bank_set_byte(
	__inout dmg_assembler_banks_t *banks,
	__in uint32_t bank,
	__in const dmg_assembler_scalar_t *address,
	__in const dmg_assembler_scalar_t *value
	)
{
	uint16_t offset;
	int result = DMG_STATUS_SUCCESS;

	if(bank >= banks->count) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Invalid bank: %u (must not exceed %u)", bank, banks->count - 1);
		goto exit;
	} else if(address->word > ADDRESS_MAX) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Invalid bank address: %04x (must not exceed %04x)", address->word, ADDRESS_MAX);
		goto exit;
	}

	offset = (address->word - banks->bank[bank].origin.word);
	banks->bank[bank].data[offset] = value->low;

	if(banks->bank[bank].size.word < BANK_WIDTH) {
		banks->bank[bank].size.word += sizeof(uint8_t);
	}

exit:
	return result;
}

int
dmg_assembler_bank_set_word(
	__inout dmg_assembler_banks_t *banks,
	__in uint32_t bank,
	__in const dmg_assembler_scalar_t *address,
	__in const dmg_assembler_scalar_t *value
	)
{
	uint16_t offset;
	int result = DMG_STATUS_SUCCESS;

	if(bank >= banks->count) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Invalid bank: %u (must not exceed %u)", bank, banks->count - 1);
		goto exit;
	} else if(address->word > ADDRESS_MAX) {
		result = ERROR_SET_FORMAT(DMG_STATUS_FAILURE, "Invalid bank address: %04x (must not exceed %04x)", address->word, ADDRESS_MAX);
		goto exit;
	}

	offset = (address->word - banks->bank[bank].origin.word);
	banks->bank[bank].data[offset] = value->low;
	banks->bank[bank].data[(offset + 1) % BANK_WIDTH] = value->high;

	if(banks->bank[bank].size.word < BANK_WIDTH) {
		banks->bank[bank].size.word += sizeof(uint16_t);
	}

exit:
	return result;
}

int
dmg_assembler_banks_allocate(
	__inout dmg_assembler_banks_t *banks
	)
{
	int result = DMG_STATUS_SUCCESS;
	dmg_assembler_scalar_t origin = {};

	dmg_assembler_banks_free(banks);

	if(!(banks->bank = (dmg_assembler_bank_t *)calloc(BANK_COUNT_INIT, sizeof(dmg_assembler_bank_t)))) {
		result = ERROR_SET(DMG_STATUS_FAILURE, "Failed to allocate bank buffer");
		goto exit;
	}

	banks->count = BANK_COUNT_INIT;

	for(uint32_t index = 0; index < banks->count; ++index) {
		banks->bank[index].origin.word = origin.word;

		if(!index) {
			origin.word += BANK_WIDTH;
		}
	}

exit:
	return result;
}

void
dmg_assembler_banks_free(
	__inout dmg_assembler_banks_t *banks
	)
{

	if(banks->bank) {
		free(banks->bank);
	}

	memset(banks, 0, sizeof(*banks));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
