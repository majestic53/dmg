/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <socket.h>

static const char *socket_get_path(bool master)
{
    return (master ? "/tmp/dmg_0" : "/tmp/dmg_1");
}

static bool socket_path_exists(bool master)
{
    return !access(socket_get_path(master), F_OK);
}

void socket_close(const socket_t *const sock)
{
    if (close(sock->handle) == -1)
    {
        fprintf(stderr, "Failed to close socket -- %i\n", errno);
    }
    unlink(socket_get_path(sock->master));
}

int socket_open(socket_t *const sock)
{
    bool master = !socket_path_exists(true);
    if (!master && (master = socket_path_exists(false)))
    {
        fprintf(stderr, "Client already open\n");
        return EXIT_FAILURE;
    }
    if ((sock->handle = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    {
        fprintf(stderr, "Failed to open socket -- %i\n", errno);
        return EXIT_FAILURE;
    }
    strcpy(sock->address.sun_path, socket_get_path(master));
    sock->address.sun_family = AF_UNIX;
    if (bind(sock->handle, (struct sockaddr *)&sock->address, strlen(sock->address.sun_path) + sizeof (sock->address.sun_family)) == -1)
    {
        fprintf(stderr, "Failed to bind socket -- %i\n", errno);
        return EXIT_FAILURE;
    }
    sock->master = master;
    strcpy(sock->remote.address.sun_path, socket_get_path(!master));
    sock->remote.address.sun_family = AF_UNIX;
    sock->remote.length = strlen(sock->remote.address.sun_path) + sizeof (sock->remote.address.sun_family);
    return EXIT_SUCCESS;
}

int socket_receive(socket_t *const sock, uint8_t *value)
{
    int result = EXIT_FAILURE;
    if (socket_path_exists(!sock->master))
    {
        if (recvfrom(sock->handle, value, sizeof(*value), 0, (struct sockaddr *)&sock->remote.address, &sock->remote.length) == -1)
        {
            fprintf(stderr, "Failed to receive data from socket -- %i\n", errno);
            return EXIT_FAILURE;
        }
        result = EXIT_SUCCESS;
    }
    return result;
}

int socket_send(const socket_t *const sock, uint8_t value)
{
    int result = EXIT_FAILURE;
    if (socket_path_exists(!sock->master))
    {
        if (sendto(sock->handle, &value, sizeof(value), 0, (const struct sockaddr *)&sock->remote.address, sock->remote.length) == -1)
        {
            fprintf(stderr, "Failed to send data to socket -- %i\n", errno);
            return EXIT_FAILURE;
        }
        result = EXIT_SUCCESS;
    }
    return result;
}
