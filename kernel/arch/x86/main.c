#include <kernel/arch/x86/boot/multiboot.h>
#include <kernel/arch/x86/boot/serial.h>
#include <kernel/arch/x86/cpu/gdt.h>
#include <kernel/arch/x86/interrupts/idt.h>
#include <kernel/arch/x86/memory/physical.h>
#include <kernel/arch/x86/memory/virtual.h>
#include <kernel/cpu/processor.h>
#include <kernel/init.h>

void arch_main(uint32_t magic, uint32_t address)
{
    DISABLE_INTERRUPTS();

    serial_init(SERIAL_COM1);
    multiboot_init(magic, address);
    gdt_init();
    idt_init();
    physical_mm_init();
    virtual_mm_init();

    kernel_main();
}
