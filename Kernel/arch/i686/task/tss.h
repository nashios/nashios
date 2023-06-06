#pragma once

#include <stdint.h>

void tss_init();
void tss_add(uint32_t index, uint32_t ss, uint32_t esp);
void tss_set_stack(uint32_t esp);
