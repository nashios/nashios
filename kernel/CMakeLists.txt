set(C_SOURCES
    init/main.c
    
    lib/string.c)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
    set(ASM_SOURCES
        arch/x86/boot/start.S)
    
    list(APPEND C_SOURCES
        arch/x86/boot/main.c
        arch/x86/boot/serial.c

        arch/x86/cpu/io.c)
endif()

add_executable(kernel ${ASM_SOURCES} ${C_SOURCES})
target_compile_options(kernel PRIVATE -ffreestanding -O2)
target_link_options(kernel PRIVATE -ffreestanding -O2 -nostdlib)

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    target_link_libraries(kernel PRIVATE gcc)
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
    target_link_options(kernel PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/arch/x86/boot/linker.ld)
endif()

install(TARGETS kernel DESTINATION boot)