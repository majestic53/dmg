/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <system.h>

uint8_t dmg_mbc0_read(dmg_handle_t const handle, uint16_t address)
{
    uint8_t result = 0xFF;
    switch (address)
    {
        case 0x0000 ... 0x3FFF: /* ROM 0 */
            result = dmg_cartridge_read_rom(handle, 0, address);
            break;
        case 0x4000 ... 0x7FFF: /* ROM 1 */
            result = dmg_cartridge_read_rom(handle, 1, address - 0x4000);
            break;
        case 0xA000 ... 0xBFFF: /* RAM 0 */
            result = dmg_cartridge_read_ram(handle, 0, address - 0xA000);
            break;
        default:
            break;
    }
    return result;
}

void dmg_mbc0_write(dmg_handle_t const handle, uint16_t address, uint8_t value)
{
    switch (address)
    {
        case 0xA000 ... 0xBFFF: /* RAM 0 */
            dmg_cartridge_write_ram(handle, 0, address - 0xA000, value);
            break;
        default:
            break;
    }
}
