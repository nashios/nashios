set(SOURCES
    assert.cpp
    ctype.cpp
    errno.cpp
    fcntl.cpp
    locale.cpp
    stdio.cpp
    stdlib.cpp
    string.cpp
    unistd.cpp
    wchar.cpp
    
    sys/poll.cpp
    sys/select.cpp
    sys/stat.cpp
    sys/time.cpp)

add_library(crt0 STATIC crt0.cpp)
add_custom_command(TARGET crt0 COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_OBJECTS:crt0> ${CMAKE_INSTALL_PREFIX}/crt0.o)

add_library(crti STATIC crti.S)
add_custom_command(TARGET crti COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_OBJECTS:crti> ${CMAKE_INSTALL_PREFIX}/crti.o)

add_library(crtn STATIC crtn.S)
add_custom_command(TARGET crtn COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_OBJECTS:crtn> ${CMAKE_INSTALL_PREFIX}/crtn.o)

nashios_libc(LibC c)
add_dependencies(LibC crti crt0 crtn)

file(WRITE "${CMAKE_STAGING_PREFIX}/usr/lib/libpthread.a" "INPUT(libc.a)")
file(WRITE "${CMAKE_STAGING_PREFIX}/usr/lib/libdl.a" "INPUT(libc.a)")
file(WRITE "${CMAKE_STAGING_PREFIX}/usr/lib/libm.a" "INPUT(libc.a)")
file(WRITE "${CMAKE_STAGING_PREFIX}/usr/lib/libssp.a" "INPUT(libc.a)")
