set(SOURCES
    main.c)

nashios_bin(desktop)
target_link_libraries(desktop PRIVATE libgui)
