set(ASM_SOURCES
    boot.S
    gdt.S

    interrupts/idt.S
    interrupts/irq.S
    interrupts/isr.S
    
    memory/page_enable.S
    memory/flush_tbl.S
    
    task/tss_flush.S
    task/sched_switch.S)

set(C_SOURCES
    main.c
    serial.c
    stdio.c
    string.c
    gdt.c
    bitmap.c
    stdlib.c

    interrupts/pic.c
    interrupts/idt.c
    interrupts/irq.c
    interrupts/isr.c
    
    system/sys.c
    
    boot/multiboot.c
    
    memory/physical.c
    memory/virtual.c
    memory/heap.c
    
    task/tss.c
    task/scheduler.c
    
    drivers/pci.c
    drivers/ata.c)

add_executable(kernel ${ASM_SOURCES} ${C_SOURCES})
target_link_libraries(kernel PRIVATE gcc)
target_compile_options(kernel PRIVATE -ffreestanding -O2)
target_link_options(kernel PRIVATE -ffreestanding -O2 -nostdlib)
target_link_options(kernel PRIVATE LINKER:-T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
install(TARGETS kernel DESTINATION boot)

nashios_install_headers(kernel)
nashios_install_sources(kernel)
