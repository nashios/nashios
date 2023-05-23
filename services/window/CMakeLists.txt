set(SOURCES
    main.c)

nashios_bin(window_service)
target_link_libraries(window_service libc)
