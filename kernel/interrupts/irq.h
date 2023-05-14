#pragma once

#include <kernel/interrupts/handler.h>
#include <kernel/interrupts/pic.h>

void irq_init();
void irq_set_handler(uint8_t irq, itr_handler_t handler);
void irq_unset_handler(uint8_t irq);