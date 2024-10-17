#include <kernel/arch/x86/boot/serial.h>
#include <kernel/arch/x86/cpu/io.h>
#include <kernel/lib/stdio.h>

#define PORT 0x3F8

void serial_init()
{
    outb(PORT + 1, 0x00);
    outb(PORT + 3, 0x80);
    outb(PORT + 0, 0x03);
    outb(PORT + 1, 0x00);
    outb(PORT + 3, 0x03);
    outb(PORT + 2, 0xC7);
    outb(PORT + 4, 0x0B);
    outb(PORT + 4, 0x1E);
    outb(PORT + 0, 0xAE);

    if (inb(PORT + 0) != 0xAE)
        return;

    outb(PORT + 4, 0x0F);

    printf("[Serial]: Initialized");
}

void serial_write_char(char character)
{
    while ((inb(PORT + 5) & 0x50) == 0)
        ;

    outb(PORT, character);
}