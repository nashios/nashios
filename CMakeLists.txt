cmake_minimum_required(VERSION 3.16)
project(NashiOS ASM C)

set(CMAKE_SYSTEM_NAME NashiOS)
set(CMAKE_SYSTEM_PROCESSOR x86)

set(CMAKE_SYSROOT ${CMAKE_BINARY_DIR}/sysroot)
set(CMAKE_STAGING_PREFIX ${CMAKE_SYSROOT})

set(TOOLCHAIN ${CMAKE_BINARY_DIR}/tools/cross/bin/i686-elf)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN}-gcc)
set(CMAKE_C_COMPILER ${TOOLCHAIN}-gcc)
set(CMAKE_LINKER ${TOOLCHAIN}-ld)
set(CMAKE_AR ${TOOLCHAIN}-ar)
set(CMAKE_OBJCOPY ${TOOLCHAIN}-objcopy)
set(CMAKE_RANLIB ${TOOLCHAIN}-ranlib)
set(CMAKE_STRIP ${TOOLCHAIN}-strip)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

add_compile_options(-Wall -Wextra)

add_custom_target(toolchain
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/setup-toolchain.sh
    USES_TERMINAL)

add_custom_target(run
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/run.sh
    USES_TERMINAL)

add_subdirectory(kernel)
