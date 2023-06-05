set(SOURCES
    main.c)

nashios_bin(Terminal)
target_link_libraries(Terminal PRIVATE libgui)
