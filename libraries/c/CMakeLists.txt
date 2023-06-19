set(SOURCES
    errno.c
    stdlib.c
    string.c
    unistd.c
    stdio.c
    assert.c
    poll.c
    fcntl.c
    mqueue.c
    ctype.c

    sys/ioctl.c
    sys/mman.c
    sys/stat.c)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "i686")
    set(CRTI_FILE arch/i686/crti.S)
    set(CRTN_FILE arch/i686/crtn.S)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(CRTI_FILE arch/x86_64/crti.S)
    set(CRTN_FILE arch/x86_64/crtn.S)
endif()

add_library(crt0 crt0.c)
add_custom_command(TARGET crt0 COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_OBJECTS:crt0> ${CMAKE_INSTALL_PREFIX}/usr/lib/crt0.o)

add_library(crti ${CRTI_FILE})
add_custom_command(TARGET crti COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_OBJECTS:crti> ${CMAKE_INSTALL_PREFIX}/usr/lib/crti.o)

add_library(crtn ${CRTN_FILE})
add_custom_command(TARGET crtn COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_OBJECTS:crtn> ${CMAKE_INSTALL_PREFIX}/usr/lib/crtn.o)

nashios_libc(libc c)
add_dependencies(libc crti crt0 crtn)

file(WRITE "${CMAKE_STAGING_PREFIX}/usr/lib/libpthread.so" "INPUT(libc.a)")
file(WRITE "${CMAKE_STAGING_PREFIX}/usr/lib/libdl.a" "INPUT(libc.a)")
file(WRITE "${CMAKE_STAGING_PREFIX}/usr/lib/libm.a" "INPUT(libc.a)")
file(WRITE "${CMAKE_STAGING_PREFIX}/usr/lib/libssp.a" "INPUT(libc.a)")
