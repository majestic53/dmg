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

#include "../../include/system/memory.h"
#include "../../src/system/memory_type.h"
#include "../include/common.h"

typedef struct {
	dmg_t configuration;
	dmg_memory_t memory;
	bool bootrom;
	uint16_t bootrom_address;
	uint8_t bootrom_value;
	bool mapper;
	uint16_t mapper_ram_address;
	uint8_t mapper_ram_value;
	uint16_t mapper_rom_address;
	uint8_t mapper_rom_value;
} dmg_memory_test_t;

static dmg_memory_test_t g_memory = {};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
dmg_bootrom_load(
	__inout dmg_bootrom_t *bootrom,
	__in const dmg_buffer_t *buffer
	)
{
	g_memory.bootrom = true;

	return ERROR_SUCCESS;
}

uint8_t
dmg_bootrom_read(
	__in const dmg_bootrom_t *bootrom,
	__in uint16_t address
	)
{
	g_memory.bootrom_address = address;

	return g_memory.bootrom_value;
}

void
dmg_bootrom_unload(
	__inout dmg_bootrom_t *bootrom
	)
{
	g_memory.bootrom = false;
}

void
dmg_bootrom_write(
	__inout dmg_bootrom_t *bootrom,
	__in uint16_t address,
	__in uint8_t value
	)
{
	g_memory.bootrom_address = address;
	g_memory.bootrom_value = value;
}

int
dmg_buffer_allocate(
	__inout dmg_buffer_t *buffer,
	__in uint32_t length,
	__in uint8_t value
	)
{
	return ERROR_SUCCESS;
}

void
dmg_buffer_free(
	__inout dmg_buffer_t *buffer
	)
{
	return;
}

int
dmg_mapper_load(
	__inout dmg_mapper_t *mapper,
	__in const dmg_buffer_t *buffer
	)
{
	g_memory.mapper = true;

	return ERROR_SUCCESS;
}

uint8_t
dmg_mapper_read_ram(
	__in const dmg_mapper_t *mapper,
	__in uint16_t address
	)
{
	g_memory.mapper_ram_address = address;

	return g_memory.mapper_ram_value;
}

uint8_t
dmg_mapper_read_rom(
	__in const dmg_mapper_t *mapper,
	__in uint16_t address
	)
{
	g_memory.mapper_rom_address = address;

	return g_memory.mapper_rom_value;
}

void
dmg_mapper_unload(
	__inout dmg_mapper_t *mapper
	)
{
	g_memory.mapper = false;
}

void
dmg_mapper_write_ram(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	)
{
	g_memory.mapper_ram_address = address;
	g_memory.mapper_ram_value = value;
}

void
dmg_mapper_write_rom(
	__inout dmg_mapper_t *mapper,
	__in uint16_t address,
	__in uint8_t value
	)
{
	g_memory.mapper_rom_address = address;
	g_memory.mapper_rom_value = value;
}

static void
dmg_test_memory_initialize(void)
{
	memset(&g_memory, 0, sizeof(g_memory));
}

int
dmg_test_memory_load(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_memory_initialize();

	if(ASSERT_SUCCESS(dmg_memory_load(&g_memory.memory, &g_memory.configuration))
			|| ASSERT(g_memory.bootrom == false)
			|| ASSERT(g_memory.mapper == true)) {
		result = EXIT_FAILURE;
	}

	dmg_test_memory_initialize();
	g_memory.configuration.bootrom.data = (void *)1;

	if(ASSERT_SUCCESS(dmg_memory_load(&g_memory.memory, &g_memory.configuration))
			|| ASSERT(g_memory.bootrom == true)
			|| ASSERT(g_memory.mapper == true)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_memory_read(void)
{
	uint8_t value = rand();
	int result = EXIT_SUCCESS;

	dmg_test_memory_initialize();
	g_memory.memory.ram.data = &value;

	if(ASSERT(dmg_memory_read(&g_memory.memory, ADDRESS_RAM_BEGIN) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_memory_initialize();
	g_memory.memory.ram.data = &value;

	if(ASSERT(dmg_memory_read(&g_memory.memory, ADDRESS_RAM_ECHO_BEGIN) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_memory_initialize();
	g_memory.memory.ram_high.data = &value;

	if(ASSERT(dmg_memory_read(&g_memory.memory, ADDRESS_RAM_HIGH_BEGIN) == value)) {
		result = EXIT_FAILURE;
	}

	dmg_test_memory_initialize();
	g_memory.mapper_ram_value = value;

	if(ASSERT(dmg_memory_read(&g_memory.memory, ADDRESS_RAM_SWAP_BEGIN) == value)
			|| ASSERT(g_memory.mapper_ram_address == ADDRESS_RAM_SWAP_BEGIN)) {
		result = EXIT_FAILURE;
	}

	dmg_test_memory_initialize();

	if(ASSERT(dmg_memory_read(&g_memory.memory, ADDRESS_RAM_UNUSED_BEGIN) == 0)) {
		result = EXIT_FAILURE;
	}

	dmg_test_memory_initialize();
	g_memory.memory.bootrom.enable = true;
	g_memory.bootrom_value = value;

	if(ASSERT(dmg_memory_read(&g_memory.memory, ADDRESS_BOOTROM_BEGIN) == value)
			|| ASSERT(g_memory.bootrom_address == ADDRESS_BOOTROM_BEGIN)) {
		result = EXIT_FAILURE;
	}

	dmg_test_memory_initialize();
	g_memory.mapper_rom_value = value;

	if(ASSERT(dmg_memory_read(&g_memory.memory, ADDRESS_BOOTROM_END + 1) == value)
			|| ASSERT(g_memory.mapper_rom_address == (ADDRESS_BOOTROM_END + 1))) {
		result = EXIT_FAILURE;
	}

	dmg_test_memory_initialize();
	g_memory.mapper_rom_value = value;

	if(ASSERT(dmg_memory_read(&g_memory.memory, ADDRESS_ROM_SWAP_BEGIN) == value)
			|| ASSERT(g_memory.mapper_rom_address == ADDRESS_ROM_SWAP_BEGIN)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_memory_unload(void)
{
	int result = EXIT_SUCCESS;

	dmg_test_memory_initialize();
	dmg_memory_load(&g_memory.memory, &g_memory.configuration);
	dmg_memory_unload(&g_memory.memory);

	if(ASSERT(g_memory.bootrom == false)
			|| ASSERT(g_memory.mapper == false)
			|| ASSERT(g_memory.memory.ram.data == NULL)
			|| ASSERT(g_memory.memory.ram_high.data == NULL)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

int
dmg_test_memory_write(void)
{
	int result = EXIT_SUCCESS;
	uint8_t value_0 = rand(), value_1;

	dmg_test_memory_initialize();
	dmg_memory_write(&g_memory.memory, ADDRESS_BOOTROM_DISABLE, value_0);

	if(ASSERT(g_memory.bootrom_address == ADDRESS_BOOTROM_DISABLE)
			|| ASSERT(g_memory.bootrom_value == value_0)) {
		return EXIT_FAILURE;
	}

	dmg_test_memory_initialize();
	g_memory.memory.ram.data = &value_1;
	dmg_memory_write(&g_memory.memory, ADDRESS_RAM_BEGIN, value_0);

	if(ASSERT(value_1 == value_0)) {
		result = EXIT_FAILURE;
	}

	dmg_test_memory_initialize();
	g_memory.memory.ram.data = &value_1;
	dmg_memory_write(&g_memory.memory, ADDRESS_RAM_ECHO_BEGIN, value_0);

	if(ASSERT(value_1 == value_0)) {
		result = EXIT_FAILURE;
	}

	dmg_test_memory_initialize();
	g_memory.memory.ram_high.data = &value_1;
	dmg_memory_write(&g_memory.memory, ADDRESS_RAM_HIGH_BEGIN, value_0);

	if(ASSERT(value_1 == value_0)) {
		result = EXIT_FAILURE;
	}

	dmg_test_memory_initialize();
	dmg_memory_write(&g_memory.memory, ADDRESS_RAM_SWAP_BEGIN, value_0);

	if(ASSERT(g_memory.mapper_ram_address == ADDRESS_RAM_SWAP_BEGIN)
			|| ASSERT(g_memory.mapper_ram_value == value_0)) {
		result = EXIT_FAILURE;
	}

	dmg_test_memory_initialize();
	dmg_memory_write(&g_memory.memory, ADDRESS_RAM_UNUSED_BEGIN, value_0);

	if(ASSERT(g_memory.mapper_ram_address == 0)
			|| ASSERT(g_memory.mapper_ram_value == 0)) {
		result = EXIT_FAILURE;
	}

	dmg_test_memory_initialize();
	dmg_memory_write(&g_memory.memory, ADDRESS_ROM_BEGIN, value_0);

	if(ASSERT(g_memory.mapper_rom_address == ADDRESS_ROM_BEGIN)
			|| ASSERT(g_memory.mapper_rom_value == value_0)) {
		result = EXIT_FAILURE;
	}

	TRACE_TEST(result);

	return result;
}

static const dmg_test TEST[] = {
	dmg_test_memory_load,
	dmg_test_memory_read,
	dmg_test_memory_unload,
	dmg_test_memory_write,
	};

int
main(
	__in int argc,
	__in char *argv[]
	)
{
	int result = EXIT_SUCCESS;

	if(argc > 1) {
		TEST_SEED(strtol(argv[1], NULL, 16));
	} else {
		TEST_SEED(time(NULL));
	}

	for(size_t trial = 0; trial < TEST_TRIALS; ++trial) {
		TRACE_TEST_TRIAL(trial);

		for(size_t test = 0; test < TEST_COUNT(TEST); ++test) {

			if(TEST[test]() != EXIT_SUCCESS) {
				result = EXIT_FAILURE;
			}
		}
	}

	return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
