#include <errno.h>
#include <fcntl.h>
#include <gui/window.h>
#include <kernel/api/framebuffer.h>
#include <kernel/api/mouse.h>
#include <mqueue.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#define MOUSE_WIDTH 32
#define MOUSE_HEIGHT 32

#define FD_COUNT 2

#include <gfx/bitmap.h>

struct service_fb
{
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t screen_size;
    char *buffer;
};

struct service_mouse
{
    int fd;
    struct graphic *graphic;
    struct mouse_event event;
};

struct service_gui
{
    int fd;
    struct mq_gui messages;
};

struct service
{
    struct service_fb fb;
    struct service_gui gui;
    struct service_mouse mouse;
    struct dlist_head window_list;
    char *buffer;
};

static struct service s_service = {};
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

struct window *window_find_window_in_window(struct window *window, char *name)
{
    if (!strcmp(window->name, name))
        return window;

    struct window *iter;
    dlist_for_each_entry(iter, &window->children, sibling)
    {
        struct window *parent = window_find_window_in_window(iter, name);
        if (parent)
            return parent;
    }
    return NULL;
}

struct window *window_find_window(char *name)
{
    struct window *iter;
    dlist_for_each_entry(iter, &s_service.window_list, sibling)
    {
        struct window *window = window_find_window_in_window(iter, name);
        if (window)
            return window;
    }
    return NULL;
}

void window_handle_create_window()
{
    struct mq_gui_window *gui_window = (struct mq_gui_window *)s_service.gui.messages.data;
    struct window *window = window_create_window(gui_window->width, gui_window->height, gui_window->x, gui_window->y,
                                                 gui_window->transparent);
    if (!window)
        return;

    dlist_head_init(&window->children);
    if (strlen(gui_window->parent))
    {
        struct window *parent = window_find_window(gui_window->parent);
        window->parent = parent;

        dlist_add_tail(&window->sibling, &parent->children);
    }
    else
        dlist_add_tail(&window->sibling, &s_service.window_list);

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

void window_draw_window(char *buffer, struct window *window, int x, int y, int scanline)
{
    int target_x = x + window->graphic->x;
    int target_y = y + window->graphic->y;

    if (window->graphic->transparent)
        gfx_graphic_alpha_draw(buffer, window->graphic->buffer, scanline, target_x, target_y, window->graphic->width,
                               window->graphic->height);
    else
        gfx_graphic_draw(buffer, window->graphic->buffer, scanline, target_x, target_y, window->graphic->width,
                         window->graphic->height);

    struct window *child;
    dlist_for_each_entry(child, &window->children, sibling)
    {
        window_draw_window(buffer, child, target_x, target_y, scanline);
    }
}

void window_draw()
{
    int scanline = s_service.fb.width * s_service.fb.bpp / 8;

    struct window *window;
    dlist_for_each_entry(window, &s_service.window_list, sibling)
    {
        window_draw_window(s_service.buffer, window, 0, 0, scanline);
    }

    gfx_graphic_alpha_draw(s_service.buffer, s_service.mouse.graphic->buffer, scanline, s_service.mouse.graphic->x,
                           s_service.mouse.graphic->y, s_service.mouse.graphic->width, s_service.mouse.graphic->height);

    memcpy(s_service.fb.buffer, s_service.buffer, s_service.fb.screen_size);
}

void window_handle_focus_window() { window_draw(); }

void window_handle_gui()
{
    memset(&s_service.gui.messages, 0x00, sizeof(struct mq_gui));
    if (mq_receive(s_service.gui.fd, (char *)&s_service.gui.messages, sizeof(struct mq_gui), 0) < 0)
        return;

    switch (s_service.gui.messages.type)
    {
    case GUI_CREATE_WINDOW:
        window_handle_create_window();
        break;
    case GUI_FOCUS_WINDOW:
        window_handle_focus_window();
        break;
    }
}

void window_handle_mouse()
{
    memset(&s_service.mouse.event, 0x00, sizeof(struct mouse_event));
    if (read(s_service.mouse.fd, (char *)&s_service.mouse.event, sizeof(struct mouse_event)) < 0)
        return;

    s_service.mouse.graphic->x += s_service.mouse.event.x;
    s_service.mouse.graphic->y -= s_service.mouse.event.y;

    if (s_service.mouse.graphic->x < 0)
        s_service.mouse.graphic->x = 0;
    else if (s_service.mouse.graphic->x + (int32_t)s_service.mouse.graphic->width > (int32_t)s_service.fb.width)
        s_service.mouse.graphic->x = s_service.fb.width - s_service.mouse.graphic->width;

    if (s_service.mouse.graphic->y < 0)
        s_service.mouse.graphic->y = 0;
    else if (s_service.mouse.graphic->y + (int32_t)s_service.mouse.graphic->height > (int32_t)s_service.fb.height)
        s_service.mouse.graphic->y = s_service.fb.height - s_service.mouse.graphic->height;

    window_draw();
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

    s_service.fb.width = vinfo.xres;
    s_service.fb.height = vinfo.yres;
    s_service.fb.bpp = vinfo.bits_per_pixel;
    s_service.fb.screen_size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    s_service.fb.buffer = (char *)mmap(NULL, s_service.fb.screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (s_service.fb.buffer == MAP_FAILED)
        exit(ENOMEM);
}

void window_init_service()
{
    dlist_head_init(&s_service.window_list);

    s_service.buffer = (char *)calloc(s_service.fb.screen_size, sizeof(char));
    if (!s_service.buffer)
        exit(ENOMEM);

    s_service.mouse.graphic = (struct graphic *)calloc(1, sizeof(struct graphic));
    if (!s_service.mouse.graphic)
        exit(ENOMEM);

    s_service.mouse.graphic->x = s_service.fb.width / 2;
    s_service.mouse.graphic->y = s_service.fb.height / 2;
    s_service.mouse.graphic->width = MOUSE_WIDTH;
    s_service.mouse.graphic->height = MOUSE_HEIGHT;
    s_service.mouse.graphic->buffer = (char *)calloc(MOUSE_WIDTH * MOUSE_HEIGHT * 4, sizeof(char));
    if (!s_service.mouse.graphic->buffer)
        exit(ENOMEM);

    char *mouse_bitmap = gfx_bitmap_open("/usr/share/icons/cursors/arrow.bmp");
    if (!mouse_bitmap)
        exit(ENOENT);

    gfx_bitmap_draw(s_service.mouse.graphic, mouse_bitmap, 0, 0);
    free(mouse_bitmap);
}

void window_init_mq()
{
    struct mq_attr attr = {.mq_msgsize = sizeof(struct mq_gui), .mq_maxmsg = 32};
    s_service.gui.fd = mq_open(GUI_SERVICE_PATH, O_RDONLY | O_CREAT, &attr);
    if (s_service.gui.fd < 0)
        exit(s_service.gui.fd);
}

void window_init_mouse()
{
    s_service.mouse.fd = open("/dev/input/mouse0", O_RDONLY);
    if (s_service.mouse.fd < 0)
        exit(s_service.mouse.fd);
}

int main()
{
    window_init_fb();
    window_init_service();
    window_init_mq();
    window_init_mouse();

    struct pollfd fds[FD_COUNT] = {{.fd = s_service.gui.fd, .events = POLLIN},
                                   {.fd = s_service.mouse.fd, .events = POLLIN}};

    while (true)
    {
        int result = poll(fds, FD_COUNT, 0);
        if (result <= 0)
            continue;

        for (int i = 0; i < FD_COUNT; i++)
        {
            if (!(fds[i].revents & POLLIN))
                continue;

            if (fds[i].fd == s_service.gui.fd)
                window_handle_gui();
            else if (fds[i].fd == s_service.mouse.fd)
                window_handle_mouse();
        }
    }

    return 0;
}
