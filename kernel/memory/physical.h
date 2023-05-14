#pragma once

#include <stddef.h>

void physical_mm_init();
void *physical_mm_allocate();
void *physical_mm_allocate_size(size_t size);