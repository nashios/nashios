cmake_minimum_required(VERSION 3.21)
project(NashiOS ASM C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_SYSTEM_NAME NashiOS)
set(CMAKE_SYSTEM_PROCESSOR x86)

set(CMAKE_SYSROOT ${CMAKE_BINARY_DIR}/sysroot)
set(CMAKE_STAGING_PREFIX ${CMAKE_SYSROOT})
set(CMAKE_INSTALL_PREFIX ${CMAKE_SYSROOT})
set(CMAKE_INSTALL_DATAROOTDIR ${CMAKE_SYSROOT}/res)

set(TOOLCHAIN ${CMAKE_BINARY_DIR}/tools/cross/bin/i686-nashios)
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
    COMMAND "SOURCE_DIR=${CMAKE_SOURCE_DIR}" "SYSROOT_DIR=${CMAKE_SYSROOT}" ${CMAKE_SOURCE_DIR}/scripts/setup-toolchain.sh
    USES_TERMINAL)

add_custom_target(image
    COMMAND "SOURCE_DIR=${CMAKE_SOURCE_DIR}" "SYSROOT_DIR=${CMAKE_SYSROOT}" ${CMAKE_SOURCE_DIR}/scripts/setup-image.sh
    USES_TERMINAL)

add_custom_target(run
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/run.sh
    USES_TERMINAL)

include_directories(.)
include_directories(libraries/c)

function(nashios_install_headers target_name)
    file(GLOB_RECURSE headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
    foreach(header ${headers})
        get_filename_component(subdirectory ${header} DIRECTORY)
        install(FILES ${header} DESTINATION usr/include/${target_name}/${subdirectory})
    endforeach()
endfunction()

function(nashios_install_sources)
    cmake_path(RELATIVE_PATH CMAKE_CURRENT_SOURCE_DIR BASE_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE relative_dir)
    file(GLOB_RECURSE sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h" "*.c")
    foreach(source ${sources})
        get_filename_component(subdirectory ${source} DIRECTORY)
        install(FILES ${source} DESTINATION usr/src/nashios/${relative_dir}/${subdirectory} OPTIONAL)
    endforeach()
endfunction()

function(nashios_libc lib_name fs_name)
    nashios_install_headers("")
    nashios_install_sources()
    add_library(${lib_name} ${SOURCES})
    target_link_directories(${lib_name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    install(TARGETS ${lib_name} DESTINATION usr/lib)
    set_target_properties(${lib_name} PROPERTIES OUTPUT_NAME ${fs_name})
endfunction()

function(nashios_bin bin_name)
    add_executable(${bin_name} ${SOURCES})
    install(TARGETS ${bin_name} RUNTIME DESTINATION bin)
endfunction()

add_subdirectory(kernel)
add_subdirectory(libraries)
add_subdirectory(services)
