#include <fcntl.h>
#include <gui/event_loop.h>
#include <mqueue.h>
#include <poll.h>

#define FD_COUNT 1

int gui_event_loop(struct window *window)
{
    struct mq_attr attr = {.mq_msgsize = 0, .mq_maxmsg = 32};
    int fd = mq_open(window->name, O_RDONLY | O_CREAT, &attr);
    if (fd < 0)
        return fd;

    struct pollfd fds[FD_COUNT] = {{.fd = fd, .events = POLLIN}};

    while (true)
    {
        int result = poll(fds, FD_COUNT, 0);
        if (result <= 0)
            continue;

        for (int i = 0; i < FD_COUNT; i++)
        {
            if (!(fds[i].revents & POLLIN))
                continue;

            if (fds[i].fd == fd)
                mq_receive(fd, NULL, 0, 0);
        }
    }

    return 0;
}
