/** @file networking.c
 *
 * @brief Library with utility functions for general network communications.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#include "networking.h"

#define PORT_MAX 65535
#define PORT_MIN 1025

/**
 * @brief Starts a socket on a TCP port and returns the file descriptor.
 *
 * @param p_port_str: String of port number
 * @return socket file descriptor on success; -1 on failure.
*/
int
start_listener (const char * p_port_str)
{
    struct addrinfo   hints  = {0};
    struct addrinfo * p_res  = NULL;
    struct addrinfo * p_curr = NULL;

    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(NULL, p_port_str, &hints, &p_res) != 0)
    {
        return FAIL;
    }

    int h_sockfd = 0;

    for (p_curr = p_res; NULL != p_curr; p_curr = p_curr->ai_next)
    {
        h_sockfd = socket(p_curr->ai_family, p_curr->ai_socktype,
                          p_curr->ai_protocol);

        if  (h_sockfd < 0)
        {
            perror("socket");
            continue;
        }

        int yes = 1;
        if (setsockopt(h_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)))
        {
            perror("setsockopt");
        }

        if (bind(h_sockfd, p_curr->ai_addr, p_curr->ai_addrlen) < 0)
        {
            perror("bind");
            close(h_sockfd);
            continue;
        }

        if (listen(h_sockfd, BACKLOG) < 0)
        {
            perror("listen");
            close(h_sockfd);
            continue;
        }

        break;
    }

    freeaddrinfo(p_res);

    if (!p_curr)
    {
        return FAIL;
    }

    return h_sockfd;
} /* start_listener */

/**
 * @brief Extends the write function to include error handling for partial
 * writes and write failures.
 *
 * @param h_fd: A file descriptor.
 * @param p_buf: Pointer to write buffer.
 * @param count: Size of the provided buffer.
 * @return true on success; false otherwise.
 */
bool
write_all (int h_fd, const void * p_buf, uint64_t count)
{
    if ((h_fd < 0) || (!p_buf && count > 0))
    {
        return false;
    }

    uint64_t bytes_sent = 0;

    while (bytes_sent < count)
    {
        ssize_t retval = write(h_fd, p_buf + bytes_sent, count - bytes_sent);

        if (retval <= 0)
        {
            perror("write");
            break;
        }
        else
        {
            bytes_sent += retval;
        }
    }

    return bytes_sent == count;
} /* write_all */

/**
 * @brief Extends the read function to include error handling for partial
 * reads and read failures.
 *
 * @param h_fd: A file descriptor from which to read.
 * @param p_buf: Pointer to buffer.
 * @param count: Size of the buffer and number of bytes to be read.
 * @return true on success; false on failure.
 */
bool
read_all (int h_fd, void * p_buf, uint64_t count)
{
    if ((h_fd < 0) || (!p_buf && count > 0))
    {
        return false;
    }

    uint64_t bytes_rx = 0;

    while (bytes_rx < count)
    {
        ssize_t retval = read(h_fd, p_buf + bytes_rx, count - bytes_rx);

        if (retval <= 0)
        {
            perror("read");
            break;
        }
        else
        {
            bytes_rx += retval;
        }
    }

    return bytes_rx == count;
} /* read_all */

/**
 * @brief Validates a provided port number.
 *
 * @param p_port_str: String of port number.
 * @return true on success; false on failure.
 */
bool
is_valid_port (char * p_port_str)
{
    if (!p_port_str)
    {
        return false;
    }

    errno = 0;
    char * p_end = NULL;
    long retval = strtol(p_port_str, &p_end, 10);

    if (p_port_str == p_end ||
        ((0 == retval) && (0 != errno)) ||
        (0 != *p_end) ||
        (retval > PORT_MAX) ||
        (retval < PORT_MIN))
    {
        fprintf(stderr, "%s: invalid port\n", __func__);
        return false;
    }

    return true;
} /* validate_port */


/*** end of file ***/

