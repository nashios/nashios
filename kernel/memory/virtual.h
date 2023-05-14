#pragma once

#include <kernel/memory/physical.h>

#define PHYS_TO_VIRT(address) (address + KERNEL_HIGHER_HALF)
#define VIRT_TO_PHYS(address) (address - KERNEL_HIGHER_HALF)

void virtual_mm_init();
void virtual_mm_map(uint32_t physical, uint32_t virtual, uint32_t flags);