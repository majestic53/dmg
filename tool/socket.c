/*
 * DMG
 * Copyright (C) 2023 David Jolly
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <socket.h>

static const char *dmg_socket_get_path(bool master)
{
    return (master ? "/tmp/dmg_0" : "/tmp/dmg_1");
}

static bool dmg_socket_path_exists(bool master)
{
    return !access(dmg_socket_get_path(master), F_OK);
}

void dmg_socket_close(const socket_t *const sock)
{
    if (close(sock->handle) == -1)
    {
        fprintf(stderr, "Failed to close socket -- %i\n", errno);
    }
    unlink(dmg_socket_get_path(sock->master));
}

dmg_error_e dmg_socket_open(socket_t *const sock)
{
    bool master = !dmg_socket_path_exists(true);
    if (!master && (master = dmg_socket_path_exists(false)))
    {
        fprintf(stderr, "Client already open\n");
        return DMG_FAILURE;
    }
    if ((sock->handle = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    {
        fprintf(stderr, "Failed to open socket -- %i\n", errno);
        return DMG_FAILURE;
    }
    strcpy(sock->address.sun_path, dmg_socket_get_path(master));
    sock->address.sun_family = AF_UNIX;
    if (bind(sock->handle, (struct sockaddr *)&sock->address, strlen(sock->address.sun_path) + sizeof (sock->address.sun_family)) == -1)
    {
        fprintf(stderr, "Failed to bind socket -- %i\n", errno);
        return DMG_FAILURE;
    }
    sock->master = master;
    strcpy(sock->remote.address.sun_path, dmg_socket_get_path(!master));
    sock->remote.address.sun_family = AF_UNIX;
    sock->remote.length = strlen(sock->remote.address.sun_path) + sizeof (sock->remote.address.sun_family);
    return DMG_SUCCESS;
}

void dmg_socket_receive(socket_t *const sock, uint8_t *value)
{
    if (dmg_socket_path_exists(!sock->master))
    {
        recvfrom(sock->handle, value, sizeof(*value), 0, (struct sockaddr *)&sock->remote.address, &sock->remote.length);
    }
}

void dmg_socket_send(const socket_t *const sock, uint8_t value)
{
    if (dmg_socket_path_exists(!sock->master))
    {
        sendto(sock->handle, &value, sizeof(value), 0, (const struct sockaddr *)&sock->remote.address, sock->remote.length);
    }
}
