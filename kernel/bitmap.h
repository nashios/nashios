#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

uint32_t bitmap_first_free(uint32_t *address, size_t max);
uint32_t bitmap_first_free_size(uint32_t *address, uint32_t max, size_t size);
void bitmap_set(uint32_t *address, uint32_t bit);
void bitmap_unset(uint32_t *address, uint32_t bit);
bool bitmap_test(uint32_t *address, uint32_t bit);
