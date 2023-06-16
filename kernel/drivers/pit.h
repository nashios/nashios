#pragma once

#include <stdint.h>

extern uint32_t g_pit_ticks;
extern uint32_t g_pit_subticks;

void pit_init();
