set(SOURCES
    bitmap.c
    graphic.c)

nashios_lib(libgfx gfx)
target_link_libraries(libgfx PRIVATE libc)
