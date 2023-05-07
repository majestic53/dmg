/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdbool.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/un.h>

typedef struct
{
    int handle;
    bool master;
    struct sockaddr_un address;
    struct {
        struct sockaddr_un address;
        socklen_t length;
    } remote;
} socket_t;

void socket_close(const socket_t *const sock);
int socket_open(socket_t *const sock);
int socket_receive(socket_t *const sock, uint8_t *value);
int socket_send(const socket_t *const sock, uint8_t value);

#endif /* SOCKET_H_ */
