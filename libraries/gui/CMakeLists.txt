set(SOURCES
    window.c
    event_loop.c)

nashios_lib(libgui gui)
target_link_libraries(libgui libgfx)
