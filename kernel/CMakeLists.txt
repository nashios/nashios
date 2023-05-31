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

if(ARCHITECTURE STREQUAL "i686")
    list(APPEND ASM_SOURCES
        arch/i686/boot/boot.S

        arch/i686/cpu/gdt.S
        
        arch/i686/memory/page_enable.S
        arch/i686/memory/flush_tbl.S
        
        arch/i686/interrupts/idt_flush.S
        arch/i686/interrupts/irq_common.S
        arch/i686/interrupts/isr_common.S
        
        arch/i686/task/tss_flush.S
        arch/i686/task/scheduler_switch.S
        arch/i686/task/scheduler_enter_usermode.S
        arch/i686/task/scheduler_return_usermode.S)

    list(APPEND C_SOURCES
        arch/i686/main.c

        arch/i686/boot/multiboot.c
        arch/i686/boot/serial.c

        arch/i686/cpu/gdt.c
        arch/i686/cpu/io.c
        arch/i686/cpu/processor.c
        
        arch/i686/interrupts/idt.c
        arch/i686/interrupts/pic.c
        
        arch/i686/memory/physical.c
        arch/i686/memory/virtual.c
        
        arch/i686/task/tss.c)

    set(LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/arch/i686/linker.ld)
endif()

add_executable(kernel ${ASM_SOURCES} ${C_SOURCES})
target_compile_options(kernel PRIVATE -ffreestanding -O2)
target_link_options(kernel PRIVATE -T ${LINKER_FILE} -ffreestanding -O2 -nostdlib)
target_link_libraries(kernel PRIVATE gcc)
target_compile_definitions(kernel PRIVATE KERNEL=true)
install(TARGETS kernel DESTINATION boot)

nashios_install_headers(kernel)
nashios_install_sources(kernel)
