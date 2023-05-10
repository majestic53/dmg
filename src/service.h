/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef DMG_SERVICE_H_
#define DMG_SERVICE_H_

#ifdef SDL2
#include <SDL.h>
#endif /* SDL2 */
#include <common.h>

typedef struct
{
#ifdef SDL2
    uint32_t tick;
    SDL_Cursor *cursor;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Window *window;
    struct
    {
        SDL_AudioDeviceID id;
        SDL_AudioSpec spec;
    } audio;
#endif /* SDL2 */
} dmg_service_t;

dmg_error_e dmg_service_initialize(dmg_handle_t const handle);
bool dmg_service_poll(dmg_handle_t const handle);
uint8_t dmg_service_silence(dmg_handle_t const handle);
dmg_error_e dmg_service_sync(dmg_handle_t const handle);
void dmg_service_uninitialize(dmg_handle_t const handle);

#endif /* DMG_SERVICE_H_ */
