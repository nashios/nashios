set(ASM_SOURCES
    boot.S
    gdt.S
    
    memory/page_enable.S
    memory/flush_tbl.S
    
    interrupts/idt_flush.S
    interrupts/irq_common.S
    interrupts/isr_common.S)

set(C_SOURCES
    main.c
    io.c
    serial.c
    string.c
    ctype.c
    stdio.c
    multiboot.c
    gdt.c
    bitmap.c
    stdlib.c
    pit.c
    
    memory/physical.c
    memory/virtual.c
    memory/heap.c
    
    interrupts/idt.c
    interrupts/pic.c
    interrupts/irq.c
    interrupts/isr.c)

add_executable(kernel ${ASM_SOURCES} ${C_SOURCES})
target_compile_options(kernel PRIVATE -ffreestanding -O2)
target_link_options(kernel PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld -ffreestanding -O2 -nostdlib)
target_link_libraries(kernel PRIVATE gcc)
install(TARGETS kernel DESTINATION boot)