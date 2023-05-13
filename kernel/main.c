#include <kernel/serial.h>
#include <stdbool.h>

void kernel_main()
{
    serial_init(SERIAL_COM1);

    while (true)
        ;
}