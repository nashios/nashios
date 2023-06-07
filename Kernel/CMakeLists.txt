set(ASM_SOURCES)

set(C_SOURCES
    init.c
    bitmap.c
    panic.c
    
    memory/heap.c
    memory/mmap.c

    interrupts/handler.c
    
    task/scheduler.c
    task/elf.c
    
    filesystem/virtual.c
    filesystem/ext2.c
    filesystem/chardev.c
    filesystem/devfs.c
    filesystem/mqueuefs.c
    filesystem/tmpfs.c
    
    drivers/pci.c
    drivers/ata.c
    drivers/fb.c
    drivers/pit.c
    
    lib/string.c
    lib/ctype.c
    lib/stdio.c
    lib/stdlib.c
    lib/assert.c
    
    system/syscall.c
    
    ipc/mqueue.c)

set(CXX_SOURCES)

if(ARCHITECTURE STREQUAL "i686")
    list(APPEND ASM_SOURCES
        Arch/i686/Boot/boot.S

        Arch/i686/cpu/gdt.S
        
        Arch/i686/memory/page_enable.S
        Arch/i686/memory/flush_tbl.S
        
        Arch/i686/interrupts/idt_flush.S
        Arch/i686/interrupts/irq_common.S
        Arch/i686/interrupts/isr_common.S
        
        Arch/i686/task/tss_flush.S
        Arch/i686/task/scheduler_switch.S
        Arch/i686/task/scheduler_enter_usermode.S
        Arch/i686/task/scheduler_return_usermode.S)

    list(APPEND C_SOURCES
        Arch/i686/Boot/multiboot.c

        Arch/i686/cpu/gdt.c
        Arch/i686/cpu/io.c
        Arch/i686/cpu/processor.c
        
        Arch/i686/interrupts/idt.c
        Arch/i686/interrupts/pic.c
        
        Arch/i686/memory/physical.c
        Arch/i686/memory/virtual.c
        
        Arch/i686/task/tss.c)

    list(APPEND CXX_SOURCES
        Arch/i686/Main.cpp

        Arch/i686/Boot/Serial.cpp)

    set(LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Arch/i686/linker.ld)
endif()

add_executable(Kernel ${ASM_SOURCES} ${C_SOURCES} ${CXX_SOURCES})
target_compile_options(Kernel PRIVATE -ffreestanding -O2)
target_link_options(Kernel PRIVATE -T ${LINKER_FILE} -ffreestanding -O2 -nostdlib)
target_link_libraries(Kernel PRIVATE gcc)
target_compile_definitions(Kernel PRIVATE KERNEL=true)
install(TARGETS Kernel DESTINATION boot)

nashios_install_headers(Kernel)
nashios_install_sources(Kernel)
