#include <kernel/gdt.h>
#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/irq.h>
#include <kernel/interrupts/isr.h>
#include <kernel/memory/physical.h>
#include <kernel/memory/virtual.h>
#include <kernel/multiboot.h>
#include <kernel/pit.h>
#include <kernel/processor.h>
#include <kernel/serial.h>

void kernel_main(uint32_t magic, uint32_t address)
{
    DISABLE_INTERRUPTS();

    serial_init(SERIAL_COM1);
    multiboot_init(magic, address);
    gdt_init();
    idt_init();
    irq_init();
    isr_init();
    pit_init();
    physical_mm_init();
    virtual_mm_init();

    ENABLE_INTERRUPTS();

    while (true)
        ;
}