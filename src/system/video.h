/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_VIDEO_H_
#define DMG_VIDEO_H_

#include <common.h>

typedef enum
{
    DMG_COLOR_WHITE = 0,
    DMG_COLOR_LIGHT_GREY,
    DMG_COLOR_DARK_GREY,
    DMG_COLOR_BLACK,
    DMG_COLOR_MAX,
} dmg_color_e;

typedef struct
{
    uint8_t y;
    uint8_t x;
    uint8_t id;
    union
    {
        struct
        {
            uint8_t : 4;
            uint8_t palette : 1;
            uint8_t x_flip : 1;
            uint8_t y_flip : 1;
            uint8_t priority : 1;
        };
        uint8_t raw;
    } attribute;
} dmg_object_t;

typedef struct
{
    uint8_t index;
    const dmg_object_t *object;
} dmg_object_entry_t;

typedef union
{
    struct
    {
        uint8_t white : 2;
        uint8_t light_grey : 2;
        uint8_t dark_grey : 2;
        uint8_t black : 2;
    };
    uint8_t raw;
} dmg_palette_t;

typedef struct
{
    dmg_color_e color[144][160];
    uint8_t ram[0x2000];
    struct
    {
        dmg_palette_t palette;
    } background;
    union
    {
        struct
        {
            uint8_t background_enabled : 1;
            uint8_t object_enabled : 1;
            uint8_t object_size : 1;
            uint8_t background_map : 1;
            uint8_t background_data : 1;
            uint8_t window_enabled : 1;
            uint8_t window_map : 1;
            uint8_t enabled : 1;
        };
        uint8_t raw;
    } control;
    struct
    {
        uint8_t delay;
        uint16_t destination;
        uint16_t source;
    } dma;
    struct
    {
        uint8_t coincidence;
        uint16_t x;
        uint8_t y;
    } line;
    struct
    {
        dmg_palette_t palette[2];
        dmg_object_t ram[40];
        struct
        {
            uint8_t count;
            dmg_object_entry_t entry[10];
        } shown;
    } object;
    struct
    {
        uint8_t x;
        uint8_t y;
    } scroll;
    union
    {
        struct
        {
            uint8_t mode : 2;
            uint8_t coincidence : 1;
            uint8_t hblank_interrupt : 1;
            uint8_t vblank_interrupt : 1;
            uint8_t search_interrupt : 1;
            uint8_t coincidence_interrupt : 1;
        };
        uint8_t raw;
    } status;
    struct
    {
        uint8_t counter;
        uint8_t x;
        uint8_t y;
    } window;
} dmg_video_t;

bool dmg_video_clock(dmg_handle_t const handle);
dmg_color_e dmg_video_color(dmg_handle_t const handle, uint8_t x, uint8_t y);
uint8_t dmg_video_read(dmg_handle_t const handle, uint16_t address);
void dmg_video_write(dmg_handle_t const handle, uint16_t address, uint8_t value);

#endif /* DMG_VIDEO_H_ */
