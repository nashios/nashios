#pragma once

#include <kernel/interrupts/handler.h>

void isr_init();
void isr_set_handler(uint8_t index, itr_handler_t handler);
void isr_unset_handler(uint8_t index);