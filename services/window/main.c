#include <fcntl.h>
#include <gui/window.h>
#include <mqueue.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define FD_COUNT 1

static struct mq_gui s_window_gui_buffer = {};
static int s_window_gui_fd = -1;
static int s_window_id = 0;

struct window *window_create_window(int width, int height, int x, int y, bool transparent)
{
    char *name = (char *)calloc(GUI_WINDOW_LENGTH, sizeof(char));
    if (!name)
        return NULL;
    sprintf(name, "%06d", s_window_id++);

    struct window *window = (struct window *)calloc(1, sizeof(struct window));
    if (!window)
        return NULL;
    memcpy(window->name, name, GUI_WINDOW_LENGTH);

    int fd = shm_open(name, O_RDWR | O_CREAT, 0);
    if (fd < 0)
        return NULL;

    int screen_size = height * width * 4;
    if (ftruncate(fd, screen_size) < 0)
        return NULL;

    window->graphic = (struct graphic *)calloc(1, sizeof(struct graphic));
    if (!window->graphic)
        return NULL;

    window->graphic->x = x;
    window->graphic->y = y;
    window->graphic->width = width;
    window->graphic->height = height;
    window->graphic->transparent = transparent;
    window->graphic->buffer = (char *)mmap(NULL, screen_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (window->graphic->buffer == MAP_FAILED)
        return NULL;

    return window;
}

void window_handle_create_window()
{
    struct mq_gui_window *gui_window = (struct mq_gui_window *)s_window_gui_buffer.data;
    struct window *window = window_create_window(gui_window->width, gui_window->height, gui_window->x, gui_window->y,
                                                 gui_window->transparent);
    if (!window)
        return;

    struct mq_attr attr = {
        .mq_msgsize = GUI_WINDOW_LENGTH,
        .mq_maxmsg = 32,
    };

    int fd = mq_open(gui_window->sender, O_WRONLY | O_CREAT, &attr);
    if (fd < 0)
        return;

    if (mq_send(fd, window->name, GUI_WINDOW_LENGTH, 0) < 0)
        return;

    if (mq_close(fd) < 0)
        return;
}

void window_handle_gui()
{
    memset(&s_window_gui_buffer, 0x00, sizeof(struct mq_gui));
    if (mq_receive(s_window_gui_fd, (char *)&s_window_gui_buffer, sizeof(struct mq_gui), 0) < 0)
        return;

    switch (s_window_gui_buffer.type)
    {
    case GUI_CREATE_WINDOW:
        window_handle_create_window();
        break;
    }
}

int main()
{
    struct mq_attr attr = {.mq_msgsize = sizeof(struct mq_gui), .mq_maxmsg = 32};
    s_window_gui_fd = mq_open(GUI_SERVICE_PATH, O_RDONLY | O_CREAT, &attr);
    if (s_window_gui_fd < 0)
        return s_window_gui_fd;

    struct pollfd fds[FD_COUNT] = {{.fd = s_window_gui_fd, .events = POLLIN}};

    while (true)
    {
        int result = poll(fds, FD_COUNT, 0);
        if (result <= 0)
            continue;

        for (int i = 0; i < FD_COUNT; i++)
        {
            if (!(fds[i].revents & POLLIN))
                continue;

            if (fds[i].fd == s_window_gui_fd)
                window_handle_gui();
        }
    }

    return 0;
}
