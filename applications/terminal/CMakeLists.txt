set(SOURCES
    main.c)

nashios_bin(terminal)
target_link_libraries(terminal libc)
