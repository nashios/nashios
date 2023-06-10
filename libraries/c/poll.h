#ifndef _POLL_H
#define _POLL_H

#include <bits/poll.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Input/output multiplexing
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/poll.html
     *
     * @param fds The file descriptors to poll
     * @param nfds The number of file descriptors to poll
     * @param timeout The timeout in milliseconds
     * @return int On success, returns the number of file descriptors that have events or errors, 0 if the timeout
     * expires, or -1 on error
     */
    int poll(struct pollfd *fds, nfds_t nfds, int timeout);

#ifdef __cplusplus
}
#endif

#endif
