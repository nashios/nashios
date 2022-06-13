set(SOURCES
    main.c)

nashios_bin(system_service)
target_link_libraries(system_service libc)
