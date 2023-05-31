#pragma once

#define POLLIN 0x0001
#define POLLOUT 0x0004

struct pollfd
{
    int fd;
    short events;
    short revents;
};
