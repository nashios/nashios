#pragma once

#include <gfx/graphic.h>
#include <gui/message.h>

struct mq_gui_window
{
    int x;
    int y;
    int width;
    int height;
    bool transparent;
    char parent[7];
    char sender[7];
};

struct window
{
    char name[GUI_WINDOW_LENGTH];
    struct graphic *graphic;
};

struct window *gui_create_window(int width, int height, int x, int y);
