#include <kernel/arch/x86/cpu/io.h>

void outb(uint16_t port, uint8_t value)
{
    asm volatile("outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

void outw(uint16_t port, uint16_t value)
{
    asm volatile("outw %w0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

void outl(uint16_t port, uint32_t value)
{
    asm volatile("outl %k0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

uint8_t inb(uint16_t port)
{
    uint8_t value;
    asm volatile("inb %w1, %b0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}

uint16_t inw(uint16_t port)
{
    uint8_t value;
    asm volatile("inw %w1, %w0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}

uint32_t inl(uint16_t port)
{
    uint8_t value;
    asm volatile("inl %w1, %k0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}