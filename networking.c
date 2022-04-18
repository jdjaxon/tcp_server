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

