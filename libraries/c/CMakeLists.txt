set(SOURCES 
    stdlib.c
    unistd.c
    errno.c)

add_library(crt0 STATIC crt0.c)
add_custom_command(
    TARGET crt0
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_OBJECTS:crt0> ${CMAKE_INSTALL_PREFIX}/usr/lib/crt0.o)

nashios_libc(libc c)
add_dependencies(libc crt0)
