#pragma once

#define POLLIN 0x0001

struct pollfd
{
    int fd;
    short events;
    short revents;
};
