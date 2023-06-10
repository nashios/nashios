#ifndef _MQUEUE_H
#define _MQUEUE_H

#include <bits/mqueue.h>
#include <stddef.h>
#include <sys/types.h>
#include <time.h>

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

    /**
     * @brief Send a message to a message queue
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_timedsend.html
     *
     * @param mqdes The message queue descriptor
     * @param msg_ptr The message to be sent
     * @param msg_len The length of the message
     * @param msg_prio The priority of the message
     * @param abstime The timeout
     * @return int On success, returns 0, or -1 on error
     */
    int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio,
                     const struct timespec *abstime);

    /**
     * @brief Send a message to a message queue
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_send.html
     *
     * @param mqdes The message queue descriptor
     * @param msg_ptr The message to be sent
     * @param msg_len The length of the message
     * @param msg_prio The priority of the message
     * @return int On success, returns 0, or -1 on error
     */
    int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio);

    /**
     * @brief Receive a message from a message queue
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_timedreceive.html
     *
     * @param mqdes The message queue descriptor
     * @param msg_ptr The buffer where the message is to be stored
     * @param msg_len The size of the buffer
     * @param msg_prio The priority of the message
     * @param abstime The timeout
     * @return ssize_t On success, returns the number of bytes in the received message, or -1 on error
     */
    ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio,
                            const struct timespec *abstime);

    /**
     * @brief Receive a message from a message queue
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_receive.html
     *
     * @param mqdes The message queue descriptor
     * @param msg_ptr The buffer where the message is to be stored
     * @param msg_len The size of the buffer
     * @param msg_prio The priority of the message
     * @return ssize_t On success, returns the number of bytes in the received message, or -1 on error
     */
    ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio);

    /**
     * @brief Close a message queue descriptor
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_close.html
     *
     * @param mqdes The message queue descriptor
     * @return int On success, returns 0, or -1 on error
     */
    int mq_close(mqd_t mqdes);

#ifdef __cplusplus
}
#endif

#endif
