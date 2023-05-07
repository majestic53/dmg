/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>
#include <test.h>

typedef struct {
    dmg_system_t system;
    struct {
        bool fail;
        void *buffer;
        uint32_t length;
    } allocate;
    struct {
        dmg_error_e status;
        dmg_handle_t handle;
        const dmg_data_t *data;
        dmg_output_f output;
        dmg_palette_e palette;
    } initialize;
    struct {
        dmg_error_e status;
        dmg_handle_t handle;
        uint8_t input;
        uint8_t *output;
    } input;
    struct {
        dmg_error_e status;
        dmg_handle_t handle;
        const dmg_data_t *data;
    } load;
    struct {
        dmg_error_e status;
        dmg_handle_t handle;
    } run;
    struct {
        dmg_error_e status;
        dmg_handle_t handle;
        dmg_data_t *data;
    } save;
    struct {
        dmg_handle_t handle;
    } uninitialize;
} test_t;

static test_t g_test = {};

static void test_setup(void)
{
    memset(&g_test, 0, sizeof(g_test));
}

static uint8_t test_output(uint8_t value)
{
    return value;
}

void *dmg_allocate(uint32_t length)
{
    g_test.allocate.buffer = g_test.allocate.fail ? NULL : &g_test.system;
    g_test.allocate.length = length;
    return g_test.allocate.buffer;
}

void dmg_free(void *buffer)
{
    g_test.allocate.buffer = buffer;
}

dmg_error_e dmg_system_initialize(dmg_handle_t handle, const dmg_data_t *const data, const dmg_output_f output, dmg_palette_e palette)
{
    g_test.initialize.handle = handle;
    g_test.initialize.data = data;
    g_test.initialize.output = output;
    g_test.initialize.palette = palette;
    return g_test.initialize.status;
}

dmg_error_e dmg_system_input(dmg_handle_t const handle, uint8_t input, uint8_t *output)
{
    g_test.input.handle = handle;
    g_test.input.input = input;
    g_test.input.output = output;
    return g_test.input.status;
}

dmg_error_e dmg_system_load(dmg_handle_t const handle, const dmg_data_t *const data)
{
    g_test.load.handle = handle;
    g_test.load.data = data;
    return g_test.load.status;
}

dmg_error_e dmg_system_run(dmg_handle_t const handle)
{
    g_test.run.handle = handle;
    return g_test.run.status;
}

dmg_error_e dmg_system_save(dmg_handle_t const handle, dmg_data_t *const data)
{
    g_test.save.handle = handle;
    g_test.save.data = data;
    return g_test.save.status;
}

void dmg_system_uninitialize(dmg_handle_t const handle)
{
    g_test.uninitialize.handle = handle;
}

DMG_TEST(test_get_error)
{
    const char *error = "Test error";
    test_setup();
    DMG_ASSERT(!strcmp(dmg_get_error(NULL), "Invalid handle"));
    test_setup();
    DMG_ASSERT(!strcmp(dmg_get_error(&g_test.system), "No error"));
    test_setup();
    strcpy(g_test.system.error, error);
    DMG_ASSERT(!strcmp(dmg_get_error(&g_test.system), error));
    DMG_PASS();
}

DMG_TEST(test_get_version)
{
    const dmg_version_t *version = dmg_get_version();
    test_setup();
    DMG_ASSERT(version != NULL);
    DMG_ASSERT(version->major == DMG_MAJOR);
    DMG_ASSERT(version->minor == DMG_MINOR);
    DMG_ASSERT(version->patch == DMG_PATCH);
    DMG_PASS();
}

DMG_TEST(test_initialize)
{
    dmg_data_t data = {};
    dmg_handle_t handle = NULL;
    dmg_palette_e palette = DMG_PALETTE_MAX - 1;
    test_setup();
    DMG_ASSERT(dmg_initialize(NULL, &data, test_output, palette) == DMG_FAILURE);
    test_setup();
    g_test.allocate.fail = true;
    DMG_ASSERT(dmg_initialize(&handle, &data, test_output, palette) == DMG_FAILURE);
    test_setup();
    handle = NULL;
    g_test.initialize.status = DMG_FAILURE;
    DMG_ASSERT(dmg_initialize(&handle, &data, test_output, palette) == g_test.initialize.status);
    test_setup();
    handle = NULL;
    DMG_ASSERT(dmg_initialize(&handle, &data, test_output, palette) == g_test.initialize.status);
    DMG_ASSERT(g_test.allocate.length == sizeof(*handle));
    DMG_ASSERT(g_test.initialize.handle == handle);
    DMG_ASSERT(g_test.initialize.data == &data);
    DMG_ASSERT(g_test.initialize.output == test_output);
    DMG_ASSERT(g_test.initialize.palette == palette);
    DMG_ASSERT(handle == &g_test.system);
    DMG_PASS();
}

DMG_TEST(test_input)
{
    uint8_t input = 0, output = 0;
    test_setup();
    DMG_ASSERT(dmg_input(NULL, input, &output) == DMG_FAILURE);
    test_setup();
    g_test.input.status = DMG_FAILURE;
    DMG_ASSERT(dmg_input(&g_test.system, input, &output) == g_test.input.status);
    test_setup();
    DMG_ASSERT(dmg_input(&g_test.system, input, &output) == g_test.input.status);
    DMG_ASSERT(g_test.input.handle == &g_test.system);
    DMG_ASSERT(g_test.input.input == input);
    DMG_ASSERT(g_test.input.output == &output);
    DMG_PASS();
}

DMG_TEST(test_load)
{
    dmg_data_t data = {};
    test_setup();
    DMG_ASSERT(dmg_load(NULL, &data) == DMG_FAILURE);
    test_setup();
    g_test.load.status = DMG_FAILURE;
    DMG_ASSERT(dmg_load(&g_test.system, &data) == g_test.load.status);
    test_setup();
    DMG_ASSERT(dmg_load(&g_test.system, &data) == g_test.load.status);
    DMG_ASSERT(g_test.load.handle == &g_test.system);
    DMG_ASSERT(g_test.load.data == &data);
    DMG_PASS();
}

DMG_TEST(test_run)
{
    test_setup();
    DMG_ASSERT(dmg_run(NULL) == DMG_FAILURE);
    test_setup();
    g_test.run.status = DMG_FAILURE;
    DMG_ASSERT(dmg_run(&g_test.system) == g_test.run.status);
    test_setup();
    DMG_ASSERT(dmg_run(&g_test.system) == g_test.run.status);
    DMG_ASSERT(g_test.run.handle == &g_test.system);
    DMG_PASS();
}

DMG_TEST(test_save)
{
    dmg_data_t data = {};
    test_setup();
    DMG_ASSERT(dmg_save(NULL, &data) == DMG_FAILURE);
    test_setup();
    g_test.save.status = DMG_FAILURE;
    DMG_ASSERT(dmg_save(&g_test.system, &data) == g_test.save.status);
    test_setup();
    DMG_ASSERT(dmg_save(&g_test.system, &data) == g_test.save.status);
    DMG_ASSERT(g_test.save.handle == &g_test.system);
    DMG_ASSERT(g_test.save.data == &data);
    DMG_PASS();
}

DMG_TEST(test_uninitialize)
{
    dmg_handle_t handle = NULL;
    test_setup();
    dmg_uninitialize(NULL);
    test_setup();
    dmg_uninitialize(&handle);
    DMG_ASSERT(g_test.uninitialize.handle == handle);
    DMG_ASSERT(g_test.allocate.buffer == handle);
    test_setup();
    handle = &g_test.system;
    dmg_uninitialize(&handle);
    DMG_ASSERT(g_test.uninitialize.handle == &g_test.system);
    DMG_ASSERT(g_test.allocate.buffer == &g_test.system);
    DMG_ASSERT(handle == NULL);
    DMG_PASS();
}

DMG_RUN(test_get_error, test_get_version, test_initialize, test_input,
    test_load, test_run, test_save, test_uninitialize);
