set(SOURCES
    main.c)

nashios_bin(WindowService)
target_link_libraries(WindowService PRIVATE libgui)
