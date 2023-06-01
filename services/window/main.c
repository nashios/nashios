#include <errno.h>
#include <fcntl.h>
#include <gui/window.h>
#include <kernel/api/framebuffer.h>
#include <mqueue.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#define FD_COUNT 1

#include <gfx/bitmap.h>

struct service_fb
{
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t screen_size;
    char *buffer;
};

struct service
{
    struct service_fb fb;
    struct dlist_head window_list;
    char *buffer;
};

static struct mq_gui s_window_gui_buffer = {};
static struct service s_service = {};
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

    dlist_head_init(&window->children);
    dlist_add_tail(&window->sibling, &s_service.window_list);

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

void window_draw_window(char *buffer, struct window *window, int x, int y)
{
    int target_x = x + window->graphic->x;
    int target_y = y + window->graphic->y;

    if (window->graphic->transparent)
        gfx_graphic_alpha_draw(buffer, window->graphic->buffer, s_service.fb.pitch, target_x, target_y,
                               window->graphic->width, window->graphic->height);
    else
        gfx_graphic_draw(buffer, window->graphic->buffer, s_service.fb.pitch, target_x, target_y,
                         window->graphic->width, window->graphic->height);

    struct window *child;
    dlist_for_each_entry(child, &window->children, sibling) { window_draw_window(buffer, child, target_x, target_y); }
}

void window_draw()
{
    struct window *window;
    dlist_for_each_entry(window, &s_service.window_list, sibling)
    {
        window_draw_window(s_service.buffer, window, 0, 0);
    }

    memcpy(s_service.fb.buffer, s_service.buffer, s_service.fb.screen_size);
}

void window_handle_focus_window() { window_draw(); }

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
    case GUI_FOCUS_WINDOW:
        window_handle_focus_window();
        break;
    }
}

void window_init_fb()
{
    int fd = open("/dev/fb0", O_RDWR);
    if (fd < 0)
        exit(fd);

    struct fb_var_screeninfo vinfo = {};

    int result = ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
    if (result < 0)
        exit(EINVAL);

    s_service.fb.width = vinfo.width;
    s_service.fb.height = vinfo.height;
    s_service.fb.pitch = vinfo.width * vinfo.bits_per_pixel;
    s_service.fb.screen_size = s_service.fb.height * s_service.fb.pitch;

    char *buffer = (char *)mmap(NULL, s_service.fb.screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buffer == MAP_FAILED)
        exit(ENOMEM);

    s_service.fb.buffer = buffer;
}

void window_init_service()
{
    dlist_head_init(&s_service.window_list);

    char *buffer = (char *)calloc(s_service.fb.screen_size, sizeof(char));
    if (!buffer)
        exit(ENOMEM);

    s_service.buffer = buffer;
}

void window_init_mq()
{
    struct mq_attr attr = {.mq_msgsize = sizeof(struct mq_gui), .mq_maxmsg = 32};
    int fd = mq_open(GUI_SERVICE_PATH, O_RDONLY | O_CREAT, &attr);
    if (fd < 0)
        exit(fd);

    s_window_gui_fd = fd;
}

int main()
{
    window_init_fb();
    window_init_service();
    window_init_mq();

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
