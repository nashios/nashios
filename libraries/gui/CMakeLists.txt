set(SOURCES
    window.c)

nashios_lib(libgui gui)
target_link_libraries(libgui libgfx)
