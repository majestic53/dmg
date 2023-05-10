/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

static uint8_t dmg_video_background_color(dmg_handle_t const handle, bool map, uint8_t x, uint8_t y)
{
    uint16_t address = (map ? 0x1C00 : 0x1800) + (32 * ((y / 8) & 31)) + ((x / 8) & 31);
    if (handle->video.control.background_data)
    {
        address = (16 * handle->video.ram[address]) + (2 * (y & 7));
    }
    else
    {
        address = (16 * (int8_t)handle->video.ram[address]) + (2 * (y & 7)) + 0x1000;
    }
    x = 1 << (7 - (x & 7));
    return ((handle->video.ram[address + 1] & x) ? 2 : 0) + ((handle->video.ram[address] & x) ? 1 : 0);
}

static uint8_t dmg_video_object_color(dmg_handle_t const handle, const dmg_object_t *object, uint8_t x, uint8_t y)
{
    uint16_t address;
    uint8_t id = object->id;
    if (handle->video.control.object_size)
    {
        if (object->attribute.y_flip)
        {
            if ((y - (object->y - 16)) < 8)
            {
                id |= 1;
            }
            else
            {
                id &= 0xFE;
            }
        }
        else if ((y - (object->y - 16)) < 8)
        {
            id &= 0xFE;
        }
        else
        {
            id |= 1;
        }
    }
    y = (y - object->y) & 7;
    if (object->attribute.x_flip)
    {
        x = 7 - x;
    }
    if (object->attribute.y_flip)
    {
        y = 7 - y;
    }
    address = (16 * id) + (2 * y);
    x = 1 << (7 - x);
    return ((handle->video.ram[address + 1] & x) ? 2 : 0) + ((handle->video.ram[address] & x) ? 1 : 0);
}

static int dmg_video_object_comparator(const void *first, const void *second)
{
    int result;
    const dmg_object_entry_t *entry[] = { first, second };
    if (entry[0]->object->x < entry[1]->object->x)
    {
        result = 1;
    }
    else if (entry[0]->object->x == entry[1]->object->x)
    {
        result = (entry[0]->index < entry[1]->index) ? 1 : 0;
    }
    else
    {
        result = -1;
    }
    return result;
}

static dmg_color_e dmg_video_palette_color(const dmg_palette_t *palette, dmg_color_e color)
{
    dmg_color_e result = 0;
    switch (color)
    {
        case DMG_COLOR_WHITE:
            result = palette->white;
            break;
        case DMG_COLOR_LIGHT_GREY:
            result = palette->light_grey;
            break;
        case DMG_COLOR_DARK_GREY:
            result = palette->dark_grey;
            break;
        case DMG_COLOR_BLACK:
            result = palette->black;
            break;
        default:
            break;
    }
    return result;
}

static void dmg_video_render_background(dmg_handle_t const handle)
{
    for (uint8_t pixel = 0; pixel < 160; ++pixel)
    {
        uint8_t color, map, x = pixel, y = handle->video.line.y;
        if (handle->video.control.window_enabled && (handle->video.window.x <= 166) && (handle->video.window.y <= 143)
                && ((handle->video.window.x - 7) <= x) && (handle->video.window.y <= y))
        {
            map = handle->video.control.window_map;
            x -= (handle->video.window.x - 7);
            y = handle->video.window.counter - handle->video.window.y;
        }
        else
        {
            map = handle->video.control.background_map;
            x += handle->video.scroll.x;
            y += handle->video.scroll.y;
        }
        color = dmg_video_palette_color(&handle->video.background.palette, dmg_video_background_color(handle, map, x, y));
        handle->video.color[handle->video.line.y][pixel] = color;
    }
}

static void dmg_video_render_objects(dmg_handle_t const handle)
{
    uint8_t color, y = handle->video.line.y;
    for (uint32_t index = 0; index < handle->video.object.shown.count; ++index)
    {
        const dmg_object_t *object = handle->video.object.shown.entry[index].object;
        for (uint8_t x = 0; x < 8; ++x)
        {
            if ((object->x < 8) && (x < (8 - object->x)))
            {
                continue;
            }
            else if ((object->x >= 160) && (x >= (8 - (object->x - 160))))
            {
                break;
            }
            if ((color = dmg_video_object_color(handle, object, x, y)) != DMG_COLOR_WHITE)
            {
                if (!object->attribute.priority || (handle->video.color[y][object->x + x - 8] == DMG_COLOR_WHITE))
                {
                    color = dmg_video_palette_color(&handle->video.object.palette[object->attribute.palette], color);
                    handle->video.color[y][object->x + x - 8] = color;
                }
            }
        }
    }
}

static void dmg_video_sort_objects(dmg_handle_t const handle)
{
    handle->video.object.shown.count = 0;
    uint8_t y = handle->video.line.y, size = handle->video.control.object_size ? 16 : 8;
    for (uint8_t index = 0; index < 40; ++index)
    {
        const dmg_object_t *object = &handle->video.object.ram[index];
        if ((y >= (object->y - 16)) && (y < (object->y - 16 + size)))
        {
            dmg_object_entry_t *entry = &handle->video.object.shown.entry[handle->video.object.shown.count++];
            entry->object = object;
            entry->index = index;
        }
        if (handle->video.object.shown.count >= 10)
        {
            break;
        }
    }
    if (handle->video.object.shown.count)
    {
        qsort(handle->video.object.shown.entry, handle->video.object.shown.count, sizeof (*handle->video.object.shown.entry),
            dmg_video_object_comparator);
    }
}

static void dmg_video_coincidence(dmg_handle_t const handle)
{
    bool coincidence = handle->video.status.coincidence;
    handle->video.status.coincidence = (handle->video.line.y == handle->video.line.coincidence);
    if (handle->video.status.coincidence_interrupt && !coincidence && handle->video.status.coincidence)
    {
        dmg_processor_interrupt(handle, DMG_INTERRUPT_LCDC);
    }
}

static void dmg_video_dma(dmg_handle_t const handle)
{
    if (!handle->video.dma.delay)
    {
        uint8_t index = handle->video.dma.destination++;
        if (index < 0xA0)
        {
            ((uint8_t *)handle->video.object.ram)[index] = dmg_system_read(handle, handle->video.dma.source++);
            handle->video.dma.delay = 4;
        }
        else
        {
            handle->video.dma.delay = 0;
            handle->video.dma.destination = 0;
            handle->video.dma.source = 0;
        }
    }
    --handle->video.dma.delay;
}

static void dmg_video_hblank(dmg_handle_t const handle)
{
    if (handle->video.control.enabled)
    {
        if (handle->video.control.background_enabled)
        {
            dmg_video_render_background(handle);
        }
        if (handle->video.control.object_enabled && handle->video.object.shown.count)
        {
            dmg_video_render_objects(handle);
        }
        if (handle->video.status.hblank_interrupt)
        {
            dmg_processor_interrupt(handle, DMG_INTERRUPT_LCDC);
        }
    }
    handle->video.status.mode = 0; /* HBLANK */
}

static void dmg_video_search(dmg_handle_t const handle)
{
    if (handle->video.control.enabled && handle->video.status.search_interrupt)
    {
        dmg_processor_interrupt(handle, DMG_INTERRUPT_LCDC);
    }
    handle->video.status.mode = 2; /* SEARCH */
}

static void dmg_video_transfer(dmg_handle_t const handle)
{
    if (handle->video.control.object_enabled)
    {
        dmg_video_sort_objects(handle);
    }
    handle->video.status.mode = 3; /* TRANSFER */
}

static dmg_error_e dmg_video_vblank(dmg_handle_t const handle)
{
    dmg_error_e result = DMG_SUCCESS;
    if (handle->video.line.y == 144)
    {
        if (handle->video.control.enabled)
        {
            if (handle->video.status.vblank_interrupt)
            {
                dmg_processor_interrupt(handle, DMG_INTERRUPT_LCDC);
            }
            dmg_processor_interrupt(handle, DMG_INTERRUPT_VBLANK);
        }
        result = DMG_COMPLETE;
    }
    handle->video.status.mode = 1; /* VBLANK */
    return result;
}

dmg_error_e dmg_video_clock(dmg_handle_t const handle)
{
    dmg_error_e result = DMG_SUCCESS;
    if (handle->video.dma.destination)
    {
        dmg_video_dma(handle);
    }
    if (handle->video.control.enabled)
    {
        dmg_video_coincidence(handle);
    }
    if (handle->video.line.y < 144)
    {
        if (!handle->video.line.x)
        {
            dmg_video_search(handle);
        }
        else if (handle->video.line.x == 80)
        {
            dmg_video_transfer(handle);
        }
        else if (handle->video.line.x == 260)
        {
            dmg_video_hblank(handle);
        }
    }
    else if (!handle->video.line.x)
    {
        result = dmg_video_vblank(handle);
    }
    if (++handle->video.line.x == 456)
    {
        if (handle->video.control.window_enabled && (handle->video.window.x <= 166) && (handle->video.window.y <= 143))
        {
            ++handle->video.window.counter;
        }
        if (++handle->video.line.y == 154)
        {
            handle->video.line.y = 0;
            handle->video.window.counter = 0;
        }
        handle->video.line.x = 0;
    }
    return result;
}

dmg_color_e dmg_video_color(dmg_handle_t const handle, uint8_t x, uint8_t y)
{
    return handle->video.color[y][x];
}

uint8_t dmg_video_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x8000 ... 0x9FFF: /* VIDEO RAM */
            if (!handle->video.control.enabled || (handle->video.status.mode < 3))
            { /* HBLANK,VBLANK,SEARCH */
                result = handle->video.ram[address - 0x8000];
            }
            break;
        case 0xFE00 ... 0xFE9F: /* OBJECT RAM */
            if (!handle->video.control.enabled || (handle->video.status.mode < 2))
            { /* HBLANK,VBLANK */
                result = ((uint8_t *)handle->video.object.ram)[address - 0xFE00];
            }
            break;
        case 0xFF40: /* LCDC */
            result = handle->video.control.raw;
            break;
        case 0xFF41: /* STAT */
            result = handle->video.status.raw;
            break;
        case 0xFF42: /* SCY */
            result = handle->video.scroll.y;
            break;
        case 0xFF43: /* SCX */
            result = handle->video.scroll.x;
            break;
        case 0xFF44: /* LY */
            result = handle->video.line.y;
            break;
        case 0xFF45: /* LYC */
            result = handle->video.line.coincidence;
            break;
        case 0xFF47: /* BGP */
            result = handle->video.background.palette.raw;
            break;
        case 0xFF48: /* OBP0 */
            result = handle->video.object.palette[0].raw;
            break;
        case 0xFF49: /* OBP1 */
            result = handle->video.object.palette[1].raw;
            break;
        case 0xFF4A: /* WY */
            result = handle->video.window.y;
            break;
        case 0xFF4B: /* WX */
            result = handle->video.window.x;
            break;
        default:
            break;
    }
    return result;
}

void dmg_video_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0x8000 ... 0x9FFF: /* VIDEO RAM */
            if (!handle->video.control.enabled || (handle->video.status.mode < 3))
            { /* HBLANK,VBLANK,SEARCH */
                handle->video.ram[address - 0x8000] = value;
            }
            break;
        case 0xFE00 ... 0xFE9F: /* OBJECT RAM */
            if (!handle->video.control.enabled || (handle->video.status.mode < 2))
            { /* HBLANK,VBLANK */
                ((uint8_t *)handle->video.object.ram)[address - 0xFE00] = value;
            }
            break;
        case 0xFF40: /* LCDC */
            handle->video.control.raw = value;
            if (!handle->video.control.enabled)
            {
                for (uint8_t y = 0; y < 144; ++y)
                {
                    for (uint8_t x = 0; x < 160; ++x)
                    {
                        handle->video.color[y][x] = DMG_COLOR_WHITE;
                    }
                }
            }
            break;
        case 0xFF41: /* STAT */
            handle->video.status.raw = value;
            break;
        case 0xFF42: /* SCY */
            handle->video.scroll.y = value;
            break;
        case 0xFF43: /* SCX */
            handle->video.scroll.x = value;
            break;
        case 0xFF45: /* LYC */
            handle->video.line.coincidence = value;
            break;
        case 0xFF46: /* DMA */
            handle->video.dma.delay = 4;
            handle->video.dma.destination = 0xFE00;
            handle->video.dma.source = value << 8;
            break;
        case 0xFF47: /* BGP */
            handle->video.background.palette.raw = value;
            break;
        case 0xFF48: /* OBP0 */
            handle->video.object.palette[0].raw = value;
            break;
        case 0xFF49: /* OBP1 */
            handle->video.object.palette[1].raw = value;
            break;
        case 0xFF4A: /* WY */
            handle->video.window.y = value;
            break;
        case 0xFF4B: /* WX */
            handle->video.window.x = value;
            break;
        default:
            break;
    }
}
