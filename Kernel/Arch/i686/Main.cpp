#include <Kernel/Arch/i686/Boot/Serial.hpp>
#include <Kernel/Init.hpp>

extern "C"
{
#include <Kernel/Arch/i686/Boot/multiboot.h>
#include <Kernel/Arch/i686/cpu/gdt.h>
#include <Kernel/Arch/i686/interrupts/idt.h>
#include <Kernel/Arch/i686/memory/physical.h>
#include <Kernel/Arch/i686/memory/virtual.h>
#include <Kernel/cpu/processor.h>
}

namespace Kernel::Arch::i686
{
    extern "C" void arch_main(uint32_t magic, uint32_t address)
    {
        DISABLE_INTERRUPTS();

        Boot::Serial::init(Boot::Serial::com1);
        multiboot_init(magic, address);
        gdt_init();
        idt_init();
        physical_mm_init();
        virtual_mm_init();

        Init::main();
    }
} // namespace Kernel::Arch::i686
