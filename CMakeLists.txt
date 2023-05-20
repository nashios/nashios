cmake_minimum_required(VERSION 3.26)
project(nashios ASM C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_SYSTEM_NAME ${CMAKE_PROJECT_NAME})
set(CMAKE_SYSTEM_PROCESSOR ${NASHIOS_PROCESSOR})

set(CMAKE_SYSROOT ${CMAKE_BINARY_DIR}/sysroot)
set(CMAKE_STAGING_PREFIX ${CMAKE_SYSROOT})

SET(TOOLCHAIN_TARGET ${NASHIOS_ARCHITECTURE}-elf)
set(TOOLCHAIN ${TOOLCHAIN_PATH}/cross/bin/${TOOLCHAIN_TARGET})

set(CMAKE_C_COMPILER ${TOOLCHAIN}-gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN}-gcc)
set(CMAKE_LINKER ${TOOLCHAIN}-ld)
set(CMAKE_AR ${TOOLCHAIN}-gcc-ar)
set(CMAKE_NM ${TOOLCHAIN}-gcc-nm)
set(CMAKE_OBJCOPY ${TOOLCHAIN}-objcopy)
set(CMAKE_RANLIB ${TOOLCHAIN}-gcc-ranlib)
set(CMAKE_STRIP ${TOOLCHAIN}-strip)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/meta/configs/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(doxygen COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} VERBATIM)
endif()

function(nashios_install_headers target_name)
    file(GLOB_RECURSE headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
    foreach(header ${headers})
        get_filename_component(subdirectory ${header} DIRECTORY)
        install(FILES ${header} DESTINATION "usr/include/${target_name}/${subdirectory}" OPTIONAL)
    endforeach()
endfunction()

function(nashios_install_sources)
    cmake_path(RELATIVE_PATH CMAKE_CURRENT_SOURCE_DIR BASE_DIRECTORY CMAKE_SOURCE_DIR OUTPUT_VARIABLE relative_dir)
    file(GLOB_RECURSE sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h" "*.c")
    foreach(source ${sources})
        get_filename_component(subdirectory ${source} DIRECTORY)
        install(FILES ${source} DESTINATION usr/src/nashios/${relative_dir}/${subdirectory} OPTIONAL)
    endforeach()
endfunction()

function(nashios_libc target_name fs_name)
    nashios_install_headers("")
    nashios_install_sources()
    add_library(${target_name} ${SOURCES})
    install(TARGETS ${target_name} DESTINATION usr/lib)
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${fs_name})
endfunction()

function(nashios_bin target_name)
    nashios_install_sources()
    add_executable(${target_name} ${SOURCES})
    install(TARGETS ${target_name} DESTINATION bin)
endfunction()

include_directories(.)

add_compile_options(-Wall -Wextra)

add_custom_target(toolchain
    COMMAND "TARGET=${TOOLCHAIN_TARGET}" "TARGET_DIR=${TOOLCHAIN_PATH}" ${CMAKE_SOURCE_DIR}/meta/scripts/setup-toolchain.sh
    USES_TERMINAL)
    
add_custom_target(image
    COMMAND "SOURCE_DIR=${CMAKE_SOURCE_DIR}" ${CMAKE_SOURCE_DIR}/meta/scripts/setup-image.sh
    USES_TERMINAL)

add_custom_target(run
    COMMAND ${CMAKE_SOURCE_DIR}/meta/scripts/run.sh
    USES_TERMINAL)

add_subdirectory(kernel)
add_subdirectory(libraries)
add_subdirectory(services)