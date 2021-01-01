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

#ifndef DMG_TOOL_LAUNCHER_SOCKET_H_
#define DMG_TOOL_LAUNCHER_SOCKET_H_

#include "../common.h"

typedef struct {
	int client;
	int server;
	pthread_t thread_id;
} dmg_launcher_socket_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void dmg_launcher_socket_close(
	__inout dmg_launcher_socket_t *socket
	);

int dmg_launcher_socket_open(
	__inout dmg_launcher_socket_t *socket,
	__in uint16_t port,
	__in bool client
	);

unsigned dmg_launcher_socket_transfer(
	__inout dmg_launcher_socket_t *socket,
	__in bool client,
	__in unsigned in
	);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMG_TOOL_LAUNCHER_SOCKET_H_ */
