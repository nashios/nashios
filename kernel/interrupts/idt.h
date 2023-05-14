#pragma once

#include <stdint.h>

typedef void (*idt_handler_t)(void);

void idt_init();
void idt_add(uint8_t index, uint16_t flags, uint16_t selector, idt_handler_t handler);