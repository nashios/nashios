#pragma once

#include <stddef.h>
#include <stdint.h>

uint8_t io_inb(uint16_t port);
uint16_t io_inw(uint16_t port);
uint32_t io_inl(uint16_t port);
void io_insw(uint16_t port, void *buffer, size_t size);

void io_outb(uint16_t port, uint8_t value);
void io_outw(uint16_t port, uint16_t value);
void io_outl(uint16_t port, uint32_t value);
void io_outsw(uint16_t port, const void *buffer, size_t size);

void io_wait();