#pragma once

#include <gfx/graphic.h>
#include <gui/message.h>
#include <st/dlist.h>

struct mq_gui_window
{
    int x;
    int y;
    int width;
    int height;
    bool transparent;
    char parent[GUI_WINDOW_LENGTH];
    char sender[GUI_WINDOW_LENGTH];
};

struct mq_gui_focus
{
    char sender[GUI_WINDOW_LENGTH];
};

struct window
{
    char name[GUI_WINDOW_LENGTH];
    struct graphic *graphic;
    struct window *parent;
    struct dlist_head sibling;
    struct dlist_head children;
};

struct window *gui_create_window(const char *title, int width, int height, int x, int y);
struct window *gui_create_unstyled_window(int width, int height, int x, int y);
void gui_focus_window(struct window *window);
