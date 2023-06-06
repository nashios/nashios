extern "C"
{
#include <Kernel/arch/i686/boot/multiboot.h>
#include <Kernel/arch/i686/boot/serial.h>
#include <Kernel/arch/i686/cpu/gdt.h>
#include <Kernel/arch/i686/interrupts/idt.h>
#include <Kernel/arch/i686/memory/physical.h>
#include <Kernel/arch/i686/memory/virtual.h>
#include <Kernel/cpu/processor.h>
#include <Kernel/init.h>
}

namespace Kernel
{
    extern "C" void arch_main(uint32_t magic, uint32_t address)
    {
        DISABLE_INTERRUPTS();

        serial_init(SERIAL_COM1);
        multiboot_init(magic, address);
        gdt_init();
        idt_init();
        physical_mm_init();
        virtual_mm_init();

        Kernel_main();
    }
} // namespace Kernel
