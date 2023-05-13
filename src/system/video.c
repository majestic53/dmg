/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

static uint8_t dmg_video_background_color(dmg_t const dmg, bool map, uint8_t x, uint8_t y)
{
    uint16_t address = (map ? 0x1C00 : 0x1800) + (32 * ((y / 8) & 31)) + ((x / 8) & 31);
    if (dmg->video.control.background_data)
    {
        address = (16 * dmg->video.ram[address]) + (2 * (y & 7));
    }
    else
    {
        address = (16 * (int8_t)dmg->video.ram[address]) + (2 * (y & 7)) + 0x1000;
    }
    x = 1 << (7 - (x & 7));
    return ((dmg->video.ram[address + 1] & x) ? 2 : 0) + ((dmg->video.ram[address] & x) ? 1 : 0);
}

static uint8_t dmg_video_object_color(dmg_t const dmg, const dmg_object_t *object, uint8_t x, uint8_t y)
{
    uint16_t address;
    uint8_t id = object->id;
    if (dmg->video.control.object_size)
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
    return ((dmg->video.ram[address + 1] & x) ? 2 : 0) + ((dmg->video.ram[address] & x) ? 1 : 0);
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

static void dmg_video_render_background(dmg_t const dmg)
{
    for (uint8_t pixel = 0; pixel < 160; ++pixel)
    {
        uint8_t color, map, x = pixel, y = dmg->video.line.y;
        if (dmg->video.control.window_enabled && (dmg->video.window.x <= 166) && (dmg->video.window.y <= 143)
                && ((dmg->video.window.x - 7) <= x) && (dmg->video.window.y <= y))
        {
            map = dmg->video.control.window_map;
            x -= (dmg->video.window.x - 7);
            y = dmg->video.window.counter - dmg->video.window.y;
        }
        else
        {
            map = dmg->video.control.background_map;
            x += dmg->video.scroll.x;
            y += dmg->video.scroll.y;
        }
        color = dmg_video_palette_color(&dmg->video.background.palette, dmg_video_background_color(dmg, map, x, y));
        dmg->video.color[dmg->video.line.y][pixel] = color;
    }
}

static void dmg_video_render_objects(dmg_t const dmg)
{
    uint8_t color, y = dmg->video.line.y;
    for (uint32_t index = 0; index < dmg->video.object.shown.count; ++index)
    {
        const dmg_object_t *object = dmg->video.object.shown.entry[index].object;
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
            if ((color = dmg_video_object_color(dmg, object, x, y)) != DMG_COLOR_WHITE)
            {
                if (!object->attribute.priority || (dmg->video.color[y][object->x + x - 8] == DMG_COLOR_WHITE))
                {
                    color = dmg_video_palette_color(&dmg->video.object.palette[object->attribute.palette], color);
                    dmg->video.color[y][object->x + x - 8] = color;
                }
            }
        }
    }
}

static void dmg_video_sort_objects(dmg_t const dmg)
{
    dmg->video.object.shown.count = 0;
    uint8_t y = dmg->video.line.y, size = dmg->video.control.object_size ? 16 : 8;
    for (uint8_t index = 0; index < 40; ++index)
    {
        const dmg_object_t *object = &dmg->video.object.ram[index];
        if ((y >= (object->y - 16)) && (y < (object->y - 16 + size)))
        {
            dmg_object_entry_t *entry = &dmg->video.object.shown.entry[dmg->video.object.shown.count++];
            entry->object = object;
            entry->index = index;
        }
        if (dmg->video.object.shown.count >= 10)
        {
            break;
        }
    }
    if (dmg->video.object.shown.count)
    {
        qsort(dmg->video.object.shown.entry, dmg->video.object.shown.count, sizeof (*dmg->video.object.shown.entry),
            dmg_video_object_comparator);
    }
}

static void dmg_video_coincidence(dmg_t const dmg)
{
    bool coincidence = dmg->video.status.coincidence;
    dmg->video.status.coincidence = (dmg->video.line.y == dmg->video.line.coincidence);
    if (dmg->video.status.coincidence_interrupt && !coincidence && dmg->video.status.coincidence)
    {
        dmg_processor_interrupt(dmg, DMG_INTERRUPT_LCDC);
    }
}

static void dmg_video_dma(dmg_t const dmg)
{
    if (!dmg->video.dma.delay)
    {
        uint8_t index = dmg->video.dma.destination++;
        if (index < 0xA0)
        {
            ((uint8_t *)dmg->video.object.ram)[index] = dmg_system_read(dmg, dmg->video.dma.source++);
            dmg->video.dma.delay = 4;
        }
        else
        {
            dmg->video.dma.delay = 0;
            dmg->video.dma.destination = 0;
            dmg->video.dma.source = 0;
        }
    }
    --dmg->video.dma.delay;
}

static void dmg_video_hblank(dmg_t const dmg)
{
    if (dmg->video.control.enabled)
    {
        if (dmg->video.control.background_enabled)
        {
            dmg_video_render_background(dmg);
        }
        if (dmg->video.control.object_enabled && dmg->video.object.shown.count)
        {
            dmg_video_render_objects(dmg);
        }
        if (dmg->video.status.hblank_interrupt)
        {
            dmg_processor_interrupt(dmg, DMG_INTERRUPT_LCDC);
        }
    }
    dmg->video.status.mode = 0; /* HBLANK */
}

static void dmg_video_search(dmg_t const dmg)
{
    if (dmg->video.control.enabled && dmg->video.status.search_interrupt)
    {
        dmg_processor_interrupt(dmg, DMG_INTERRUPT_LCDC);
    }
    dmg->video.status.mode = 2; /* SEARCH */
}

static void dmg_video_transfer(dmg_t const dmg)
{
    if (dmg->video.control.object_enabled)
    {
        dmg_video_sort_objects(dmg);
    }
    dmg->video.status.mode = 3; /* TRANSFER */
}

static bool dmg_video_vblank(dmg_t const dmg)
{
    bool result = false;
    if (dmg->video.line.y == 144)
    {
        if (dmg->video.control.enabled)
        {
            if (dmg->video.status.vblank_interrupt)
            {
                dmg_processor_interrupt(dmg, DMG_INTERRUPT_LCDC);
            }
            dmg_processor_interrupt(dmg, DMG_INTERRUPT_VBLANK);
        }
        result = true;
    }
    dmg->video.status.mode = 1; /* VBLANK */
    return result;
}

bool dmg_video_clock(dmg_t const dmg)
{
    bool result = false;
    if (dmg->video.dma.destination)
    {
        dmg_video_dma(dmg);
    }
    if (dmg->video.control.enabled)
    {
        dmg_video_coincidence(dmg);
    }
    if (dmg->video.line.y < 144)
    {
        if (!dmg->video.line.x)
        {
            dmg_video_search(dmg);
        }
        else if (dmg->video.line.x == 80)
        {
            dmg_video_transfer(dmg);
        }
        else if (dmg->video.line.x == 260)
        {
            dmg_video_hblank(dmg);
        }
    }
    else if (!dmg->video.line.x)
    {
        result = dmg_video_vblank(dmg);
    }
    if (++dmg->video.line.x == 456)
    {
        if (dmg->video.control.window_enabled && (dmg->video.window.x <= 166) && (dmg->video.window.y <= 143))
        {
            ++dmg->video.window.counter;
        }
        if (++dmg->video.line.y == 154)
        {
            dmg->video.line.y = 0;
            dmg->video.window.counter = 0;
        }
        dmg->video.line.x = 0;
    }
    return result;
}

dmg_color_e dmg_video_color(dmg_t const dmg, uint8_t x, uint8_t y)
{
    return dmg->video.color[y][x];
}

uint8_t dmg_video_read(dmg_t const dmg, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x8000 ... 0x9FFF: /* VIDEO RAM */
            if (!dmg->video.control.enabled || (dmg->video.status.mode < 3))
            { /* HBLANK,VBLANK,SEARCH */
                result = dmg->video.ram[address - 0x8000];
            }
            break;
        case 0xFE00 ... 0xFE9F: /* OBJECT RAM */
            if (!dmg->video.control.enabled || (dmg->video.status.mode < 2))
            { /* HBLANK,VBLANK */
                result = ((uint8_t *)dmg->video.object.ram)[address - 0xFE00];
            }
            break;
        case 0xFF40: /* LCDC */
            result = dmg->video.control.raw;
            break;
        case 0xFF41: /* STAT */
            result = dmg->video.status.raw;
            break;
        case 0xFF42: /* SCY */
            result = dmg->video.scroll.y;
            break;
        case 0xFF43: /* SCX */
            result = dmg->video.scroll.x;
            break;
        case 0xFF44: /* LY */
            result = dmg->video.line.y;
            break;
        case 0xFF45: /* LYC */
            result = dmg->video.line.coincidence;
            break;
        case 0xFF47: /* BGP */
            result = dmg->video.background.palette.raw;
            break;
        case 0xFF48: /* OBP0 */
            result = dmg->video.object.palette[0].raw;
            break;
        case 0xFF49: /* OBP1 */
            result = dmg->video.object.palette[1].raw;
            break;
        case 0xFF4A: /* WY */
            result = dmg->video.window.y;
            break;
        case 0xFF4B: /* WX */
            result = dmg->video.window.x;
            break;
        default:
            break;
    }
    return result;
}

void dmg_video_write(dmg_t const dmg, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0x8000 ... 0x9FFF: /* VIDEO RAM */
            if (!dmg->video.control.enabled || (dmg->video.status.mode < 3))
            { /* HBLANK,VBLANK,SEARCH */
                dmg->video.ram[address - 0x8000] = value;
            }
            break;
        case 0xFE00 ... 0xFE9F: /* OBJECT RAM */
            if (!dmg->video.control.enabled || (dmg->video.status.mode < 2))
            { /* HBLANK,VBLANK */
                ((uint8_t *)dmg->video.object.ram)[address - 0xFE00] = value;
            }
            break;
        case 0xFF40: /* LCDC */
            dmg->video.control.raw = value;
            if (!dmg->video.control.enabled)
            {
                for (uint8_t y = 0; y < 144; ++y)
                {
                    for (uint8_t x = 0; x < 160; ++x)
                    {
                        dmg->video.color[y][x] = DMG_COLOR_WHITE;
                    }
                }
            }
            break;
        case 0xFF41: /* STAT */
            dmg->video.status.raw = value;
            break;
        case 0xFF42: /* SCY */
            dmg->video.scroll.y = value;
            break;
        case 0xFF43: /* SCX */
            dmg->video.scroll.x = value;
            break;
        case 0xFF45: /* LYC */
            dmg->video.line.coincidence = value;
            break;
        case 0xFF46: /* DMA */
            dmg->video.dma.delay = 4;
            dmg->video.dma.destination = 0xFE00;
            dmg->video.dma.source = value << 8;
            break;
        case 0xFF47: /* BGP */
            dmg->video.background.palette.raw = value;
            break;
        case 0xFF48: /* OBP0 */
            dmg->video.object.palette[0].raw = value;
            break;
        case 0xFF49: /* OBP1 */
            dmg->video.object.palette[1].raw = value;
            break;
        case 0xFF4A: /* WY */
            dmg->video.window.y = value;
            break;
        case 0xFF4B: /* WX */
            dmg->video.window.x = value;
            break;
        default:
            break;
    }
}
