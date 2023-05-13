#include <kernel/io.h>

uint8_t io_inb(uint16_t port)
{
    uint8_t value = 0;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

uint16_t io_inw(uint16_t port)
{
    uint16_t value = 0;
    asm volatile("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

uint32_t io_inl(uint16_t port)
{
    uint32_t value = 0;
    asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void io_outb(uint16_t port, uint8_t value) { asm volatile("outb %0, %1" : : "a"(value), "Nd"(port)); }

void io_outw(uint16_t port, uint16_t value) { asm volatile("outw %0, %1" : : "a"(value), "Nd"(port)); }

void io_outl(uint16_t port, uint32_t value) { asm volatile("outl %0, %1" : : "a"(value), "Nd"(port)); }

void io_wait() { io_outb(0x80, 0x00); }