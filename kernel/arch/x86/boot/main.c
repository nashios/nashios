#include <kernel/arch/x86/boot/serial.h>
#include <kernel/init/main.h>

void arch_main()
{
    serial_init();
    kernel_main();
}