#pragma once

#include <stdbool.h>

#define DISABLE_INTERRUPTS() asm volatile("cli")
#define ENABLE_INTERRUPTS() asm volatile("sti")
#define PAUSE() asm volatile("hlt")
#define STOP()                                                                                                         \
    {                                                                                                                  \
        DISABLE_INTERRUPTS();                                                                                          \
        while (true)                                                                                                   \
        {                                                                                                              \
            PAUSE();                                                                                                   \
        }                                                                                                              \
    }
