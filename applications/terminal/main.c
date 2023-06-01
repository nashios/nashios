#include <gui/event_loop.h>

int main()
{
    struct window *window = gui_create_window(800, 600, 100, 100);
    gui_focus_window(window);
    return gui_event_loop(window);
}
