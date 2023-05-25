set(SOURCES
    errno.c
    stdlib.c
    unistd.c
    string.c
    assert.c)

add_library(crt0 crt0.c)
add_custom_command(TARGET crt0 COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_OBJECTS:crt0> ${CMAKE_INSTALL_PREFIX}/usr/lib/crt0.o)

add_library(crti crti.S)
add_custom_command(TARGET crti COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_OBJECTS:crti> ${CMAKE_INSTALL_PREFIX}/usr/lib/crti.o)

add_library(crtn crtn.S)
add_custom_command(TARGET crtn COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_OBJECTS:crtn> ${CMAKE_INSTALL_PREFIX}/usr/lib/crtn.o)

nashios_libc(libc c)
add_dependencies(libc crti crt0 crtn)
