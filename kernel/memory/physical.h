#pragma once

#include <stddef.h>
#include <stdint.h>

#define KERNEL_HIGHER_HALF ((uint32_t)&_kernel_higher_half)

extern void *_kernel_higher_half;

void physical_mm_init();
void *physical_mm_allocate();
void *physical_mm_allocate_size(size_t size);