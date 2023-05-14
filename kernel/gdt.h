#pragma once

#include <stdint.h>

void gdt_init();
void gdt_add(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);