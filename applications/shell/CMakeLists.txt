set(SOURCES
    main.c)

nashios_bin(shell)
target_link_libraries(shell libc)
