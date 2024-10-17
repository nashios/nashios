#include <kernel/arch/lib/stdio.h>
#include <kernel/arch/x86/boot/serial.h>

void arch_printf(const char *message, size_t length)
{
    for (size_t i = 0; i < length; i++)
        serial_write_char(message[i]);
}