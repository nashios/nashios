#include <kernel/multiboot.h>
#include <kernel/serial.h>
#include <stdbool.h>

void kernel_main(uint32_t magic, uint32_t address)
{
    serial_init(SERIAL_COM1);
    multiboot_init(magic, address);

    while (true)
        ;
}