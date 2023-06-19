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
    drivers/mouse.c
    
    drivers/char/debug.c
    
    lib/string.c
    lib/ctype.c
    lib/stdio.c
    lib/stdlib.c
    lib/assert.c
    
    system/syscall.c
    
    ipc/mqueue.c)

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
    list(APPEND ASM_SOURCES
        arch/x86/boot/boot.S

        arch/x86/cpu/gdt.S
        
        arch/x86/memory/page_enable.S
        arch/x86/memory/flush_tbl.S
        
        arch/x86/interrupts/idt_flush.S
        arch/x86/interrupts/irq_common.S
        arch/x86/interrupts/isr_common.S
        
        arch/x86/task/tss_flush.S
        arch/x86/task/scheduler_switch.S
        arch/x86/task/scheduler_enter_usermode.S
        arch/x86/task/scheduler_return_usermode.S)

    list(APPEND C_SOURCES
        arch/x86/main.c

        arch/x86/boot/multiboot.c
        arch/x86/boot/serial.c

        arch/x86/cpu/gdt.c
        arch/x86/cpu/io.c
        arch/x86/cpu/processor.c
        
        arch/x86/interrupts/idt.c
        arch/x86/interrupts/pic.c
        
        arch/x86/memory/physical.c
        arch/x86/memory/virtual.c
        
        arch/x86/task/tss.c)

    set(LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/arch/x86/linker.ld)
endif()

add_executable(kernel ${ASM_SOURCES} ${C_SOURCES})
target_compile_options(kernel PRIVATE -ffreestanding -O2)
target_link_options(kernel PRIVATE -T ${LINKER_FILE} -ffreestanding -O2 -nostdlib)
target_compile_definitions(kernel PRIVATE KERNEL=true)
install(TARGETS kernel DESTINATION boot)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_libraries(kernel PRIVATE gcc)
endif()

nashios_install_headers(kernel)
nashios_install_sources(kernel)
