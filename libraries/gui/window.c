#include <fcntl.h>
#include <gui/window.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

struct window *gui_create_window(int width, int height, int x, int y)
{
    struct window *window = (struct window *)calloc(1, sizeof(struct window));
    if (!window)
        return NULL;

    struct mq_gui *mq_gui = (struct mq_gui *)calloc(1, sizeof(struct mq_gui));
    if (!mq_gui)
        return NULL;
    mq_gui->type = GUI_CREATE_WINDOW;

    struct mq_gui_window *mq_gui_window = (struct mq_gui_window *)mq_gui->data;
    mq_gui_window->x = x;
    mq_gui_window->y = y;
    mq_gui_window->width = width;
    mq_gui_window->height = height;
    mq_gui_window->transparent = false;

    char *name = (char *)calloc(GUI_WINDOW_LENGTH, sizeof(char));
    if (!name)
        return NULL;

    sprintf(name, "%d", getpid());
    memcpy(mq_gui_window->sender, name, GUI_WINDOW_LENGTH);
    free(name);

    struct mq_attr attr = {};
    attr.mq_msgsize = sizeof(struct mq_gui);
    attr.mq_maxmsg = 32;

    int fd = mq_open(GUI_SERVICE_PATH, O_WRONLY, &attr);
    if (fd < 0)
        return NULL;

    if (mq_send(fd, (const char *)mq_gui, sizeof(struct mq_gui), 0) < 0)
        return NULL;

    if (mq_close(fd) < 0)
        return NULL;

    window->graphic = (struct graphic *)calloc(1, sizeof(struct graphic));
    if (!window->graphic)
        return NULL;

    window->graphic->x = x;
    window->graphic->y = y;
    window->graphic->width = width;
    window->graphic->height = height;
    window->graphic->transparent = false;

    dlist_head_init(&window->children);

    memset(&attr, 0x00, sizeof(struct mq_attr));
    attr.mq_msgsize = GUI_WINDOW_LENGTH;
    attr.mq_maxmsg = 32;

    fd = mq_open(mq_gui_window->sender, O_RDONLY | O_CREAT, &attr);
    if (fd < 0)
        return NULL;

    if (mq_receive(fd, window->name, GUI_WINDOW_LENGTH, 0) < 0)
        return NULL;

    if (mq_close(fd) < 0)
        return NULL;

    fd = shm_open(window->name, O_RDWR, 0);
    if (fd < 0)
        return NULL;

    size_t buffer_size = width * height * 4;
    window->graphic->buffer = (char *)mmap(NULL, buffer_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (window->graphic->buffer == MAP_FAILED)
        return NULL;

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
