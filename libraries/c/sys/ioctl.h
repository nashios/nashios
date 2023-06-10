#ifndef _SYS_IOCTL_H
#define _SYS_IOCTL_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Controls a stream device.
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/ioctl.html
     *
     * @param fildes The file descriptor of the device to control
     * @param request The request to perform
     * @param ... The arguments to the request
     * @return int On success, zero is returned. On error, -1 is returned, and errno is set appropriately.
     */
    int ioctl(int fildes, unsigned long request, ...);

#ifdef __cplusplus
}
#endif

#endif
