#include <kernel/arch/i686/cpu/io.h>

uint8_t io_inb(uint16_t port)
{
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}

uint16_t io_inw(uint16_t port)
{
    uint16_t value;
    asm volatile("inw %1, %0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}

uint32_t io_inl(uint16_t port)
{
    uint32_t value;
    asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}

void io_insw(uint16_t port, void *buffer, size_t size)
{
    asm volatile("rep insw" : "+D"(buffer), "+c"(size) : "d"(port) : "memory");
}

void io_outb(uint16_t port, uint8_t value) { asm volatile("outb %0, %1" : : "a"(value), "Nd"(port) : "memory"); }

void io_outw(uint16_t port, uint16_t value) { asm volatile("outw %0, %1" : : "a"(value), "Nd"(port) : "memory"); }

void io_outl(uint16_t port, uint32_t value) { asm volatile("outl %0, %1" : : "a"(value), "Nd"(port) : "memory"); }

void io_outsw(uint16_t port, const void *buffer, size_t size)
{
    asm volatile("rep outsw" : "+S"(buffer), "+c"(size) : "d"(port) : "memory");
}

void io_wait() { io_outb(0x80, 0x00); }
