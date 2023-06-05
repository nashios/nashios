set(SOURCES
    main.c)

nashios_bin(Desktop)
target_link_libraries(Desktop PRIVATE libgui)
