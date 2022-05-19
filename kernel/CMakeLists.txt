set(ASM_SOURCES
    boot.S)

set(C_SOURCES
    main.c
    serial.c)

add_executable(kernel ${ASM_SOURCES} ${C_SOURCES})
target_link_libraries(kernel PRIVATE gcc)
target_compile_options(kernel PRIVATE -ffreestanding -O2)
target_link_options(kernel PRIVATE -ffreestanding -O2 -nostdlib)
target_link_options(kernel PRIVATE LINKER:-T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
