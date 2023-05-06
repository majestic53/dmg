/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <test.h>

void *dmg_allocate(uint32_t length)
{
    /* TODO */
    return NULL;
    /* ---- */
}

void dmg_free(void *buffer)
{
    /* TODO */
}

dmg_error_e dmg_system_input(dmg_handle_t const handle, uint8_t input, uint8_t *output)
{
    /* TODO */
    return DMG_FAILURE;
    /* ---- */
}

dmg_error_e dmg_system_load(dmg_handle_t const handle, const dmg_data_t *const data)
{
    /* TODO */
    return DMG_FAILURE;
    /* ---- */
}

dmg_error_e dmg_system_run(dmg_handle_t const handle)
{
    /* TODO */
    return DMG_FAILURE;
    /* ---- */
}

dmg_error_e dmg_system_save(dmg_handle_t const handle, dmg_data_t *const data)
{
    /* TODO */
    return DMG_FAILURE;
    /* ---- */
}

dmg_error_e dmg_system_setup(dmg_handle_t handle, const dmg_data_t *const data, const dmg_output_f output, dmg_palette_e palette)
{
    /* TODO */
    return DMG_FAILURE;
    /* ---- */
}

void dmg_system_teardown(dmg_handle_t const handle)
{
    /* TODO */
}

DMG_TEST(test_get_error)
{
    /* TODO */
    DMG_PASS();
    /* ---- */
}

DMG_TEST(test_get_version)
{
    /* TODO */
    DMG_PASS();
    /* ---- */
}

DMG_TEST(test_initialize)
{
    /* TODO */
    DMG_PASS();
    /* ---- */
}

DMG_TEST(test_input)
{
    /* TODO */
    DMG_PASS();
    /* ---- */
}

DMG_TEST(test_load)
{
    /* TODO */
    DMG_PASS();
    /* ---- */
}

DMG_TEST(test_run)
{
    /* TODO */
    DMG_PASS();
    /* ---- */
}

DMG_TEST(test_save)
{
    /* TODO */
    DMG_PASS();
    /* ---- */
}

DMG_TEST(test_uninitialize)
{
    /* TODO */
    DMG_PASS();
    /* ---- */
}

DMG_RUN(test_get_error, test_get_version, test_initialize, test_input,
    test_load, test_run, test_save, test_uninitialize);
