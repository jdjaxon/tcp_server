/** @file networking.h
 *
 * @brief Networking library with utility functions for general communications.
 */

#ifndef NETWORKING_H
#define NETWORKING_H

#include <stdint.h>
#include <stdbool.h>

#define FAIL         -1
#define BACKLOG      100
#define BUF_LEN      4096
#define SOCK_TIMEOUT 10


int  start_listener(const char *);
bool write_all(int, const void *, uint64_t);
bool read_all(int, void *, uint64_t);
bool is_valid_port(char *);

#endif /* NETWORKING_H */


/*** end of file ***/

