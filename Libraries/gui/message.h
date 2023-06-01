#pragma once

#define GUI_SERVICE_PATH "/dev/mqueue/window_service"
#define GUI_WINDOW_LENGTH 7

enum mq_gui_type
{
    GUI_CREATE_WINDOW,
    GUI_FOCUS_WINDOW,
};

struct mq_gui
{
    enum mq_gui_type type;
    char data[128];
};
