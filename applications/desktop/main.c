#include <gui/window.h>
#include <stdlib.h>

int main()
{
    struct window *window = gui_create_window(1024, 768, 0, 0);
    if (!window)
        return EXIT_FAILURE;

    while (true)
        ;
}
