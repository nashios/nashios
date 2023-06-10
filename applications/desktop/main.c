#include <gfx/bitmap.h>
#include <gui/event_loop.h>
#include <stdlib.h>

int main()
{
    struct window *window = gui_create_unstyled_window(1024, 768, 0, 0);
    if (!window)
        return EXIT_FAILURE;

    char *buffer = gfx_bitmap_open("/usr/share/backgrounds/church_in_the_valley.bmp");
    if (!buffer)
        return EXIT_FAILURE;
    gfx_bitmap_draw(window->graphic, buffer, 0, 0);
    free(buffer);

    gui_focus_window(window);

    return gui_event_loop(window);
}
