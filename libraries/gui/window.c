#include <fcntl.h>
#include <gfx/bitmap.h>
#include <gui/window.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

void gui_init_window(struct window *window, struct window *parent, int width, int height, int x, int y,
                     bool transparent)
{
    struct mq_gui *mq_gui = (struct mq_gui *)calloc(1, sizeof(struct mq_gui));
    if (!mq_gui)
        return;
    mq_gui->type = GUI_CREATE_WINDOW;

    struct mq_gui_window *mq_gui_window = (struct mq_gui_window *)mq_gui->data;
    mq_gui_window->x = x;
    mq_gui_window->y = y;
    mq_gui_window->width = width;
    mq_gui_window->height = height;
    mq_gui_window->transparent = transparent;

    char *name = (char *)calloc(GUI_WINDOW_LENGTH, sizeof(char));
    if (!name)
        return;

    sprintf(name, "%d", getpid());
    memcpy(mq_gui_window->sender, name, GUI_WINDOW_LENGTH);
    free(name);

    if (parent)
        memcpy(mq_gui_window->parent, parent->name, GUI_WINDOW_LENGTH);

    struct mq_attr attr = {};
    attr.mq_msgsize = sizeof(struct mq_gui);
    attr.mq_maxmsg = 32;

    int fd = mq_open(GUI_SERVICE_PATH, O_WRONLY, &attr);
    if (fd < 0)
        return;

    if (mq_send(fd, (const char *)mq_gui, sizeof(struct mq_gui), 0) < 0)
        return;

    if (mq_close(fd) < 0)
        return;

    window->graphic = (struct graphic *)calloc(1, sizeof(struct graphic));
    if (!window->graphic)
        return;

    window->graphic->x = x;
    window->graphic->y = y;
    window->graphic->width = width;
    window->graphic->height = height;
    window->graphic->transparent = transparent;
    window->parent = parent;

    dlist_head_init(&window->children);
    if (parent)
        dlist_add_tail(&window->sibling, &parent->children);

    memset(&attr, 0x00, sizeof(struct mq_attr));
    attr.mq_msgsize = GUI_WINDOW_LENGTH;
    attr.mq_maxmsg = 32;

    fd = mq_open(mq_gui_window->sender, O_RDONLY | O_CREAT, &attr);
    if (fd < 0)
        return;

    if (mq_receive(fd, window->name, GUI_WINDOW_LENGTH, 0) < 0)
        return;

    if (mq_close(fd) < 0)
        return;

    fd = shm_open(window->name, O_RDWR, 0);
    if (fd < 0)
        return;

    size_t buffer_size = width * height * 4;
    window->graphic->buffer = (char *)mmap(NULL, buffer_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (window->graphic->buffer == MAP_FAILED)
        return;
}

void gui_set_window_bg_color(struct window *window, uint32_t color)
{
    for (int i = 0; i < window->graphic->height; i++)
    {
        char *buffer = window->graphic->buffer + i * window->graphic->width * 4;
        for (int j = 0; j < window->graphic->width; j++)
        {
            (*(uint32_t *)buffer) = color;
            buffer += 4;
        }
    }
}

void gui_init_window_topbar(struct window *window)
{
    struct window *topbar = (struct window *)calloc(1, sizeof(struct window));
    if (!topbar)
        return;

    gui_init_window(topbar, window, window->graphic->width, 36, 0, 0, false);
    gui_set_window_bg_color(topbar, 0xFFFFFFFF);

    struct window *close = (struct window *)calloc(1, sizeof(struct window));
    if (!close)
        return;

    gui_init_window(close, topbar, 16, 16, topbar->graphic->width - 24, 8, true);

    char *close_bitmap = gfx_bitmap_open("/usr/share/icons/16x16/window-close.bmp");
    if (!close_bitmap)
        return;
    gfx_bitmap_draw(close->graphic, close_bitmap, 0, 0);
    free(close_bitmap);

    struct window *maximize = (struct window *)calloc(1, sizeof(struct window));
    if (!maximize)
        return;

    gui_init_window(maximize, topbar, 16, 16, topbar->graphic->width - 48, 8, true);

    char *maximize_bitmap = gfx_bitmap_open("/usr/share/icons/16x16/window-maximize.bmp");
    if (!maximize_bitmap)
        return;
    gfx_bitmap_draw(maximize->graphic, maximize_bitmap, 0, 0);
    free(maximize_bitmap);

    struct window *minimize = (struct window *)calloc(1, sizeof(struct window));
    if (!minimize)
        return;

    gui_init_window(minimize, topbar, 16, 16, topbar->graphic->width - 72, 8, true);

    char *minimize_bitmap = gfx_bitmap_open("/usr/share/icons/16x16/window-minimize.bmp");
    if (!minimize_bitmap)
        return;
    gfx_bitmap_draw(minimize->graphic, minimize_bitmap, 0, 0);
    free(minimize_bitmap);
}

void gui_init_window_body(struct window *window)
{
    struct window *body = (struct window *)calloc(1, sizeof(struct window));
    if (!body)
        return;

    gui_init_window(body, window, window->graphic->width, window->graphic->height - 36, 0, 36, false);
    gui_set_window_bg_color(body, 0x00000000);
}

struct window *gui_create_unstyled_window(int width, int height, int x, int y)
{
    struct window *window = (struct window *)calloc(1, sizeof(struct window));
    if (!window)
        return NULL;

    gui_init_window(window, NULL, width, height, x, y, false);

    return window;
}

struct window *gui_create_window(int width, int height, int x, int y)
{
    struct window *window = gui_create_unstyled_window(width, height, x, y);
    gui_init_window_topbar(window);
    gui_init_window_body(window);

    return window;
}

void gui_focus_window(struct window *window)
{
    struct mq_gui *mq_gui = (struct mq_gui *)calloc(1, sizeof(struct mq_gui));
    if (!mq_gui)
        return;

    mq_gui->type = GUI_FOCUS_WINDOW;

    struct mq_gui_focus *focus = (struct mq_gui_focus *)mq_gui->data;
    memcpy(focus->sender, window->name, GUI_WINDOW_LENGTH);

    struct mq_attr attr = {.mq_msgsize = sizeof(struct mq_gui), .mq_maxmsg = 32};
    int fd = mq_open(GUI_SERVICE_PATH, O_WRONLY, &attr);
    if (fd < 0)
        return;

    if (mq_send(fd, (char *)mq_gui, sizeof(struct mq_gui), 0) < 0)
        return;

    if (mq_close(fd) < 0)
        return;

    free(mq_gui);
}
