#ifndef _MQUEUE_H
#define _MQUEUE_H

#include <bits/mqueue.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Create a new message queue or open an existing queue
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_open.html
     *
     * @param name The name of the queue
     * @param oflag The flags to open the queue with
     * @param ... The permissions to open the queue with
     * @return mqd_t On success, returns the message queue descriptor, or (mqd_t)-1 on error
     */
    mqd_t mq_open(const char *name, int oflag, ...);

#ifdef __cplusplus
}
#endif

#endif
