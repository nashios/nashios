#pragma once

#include <stdbool.h>

#define ENABLE_INTERRUPTS() ___enable_interrupts();

#define DISABLE_INTERRUPTS() ___disable_interrupts();

#define PAUSE() ___pause();

#define STOP()                                                                                                         \
    {                                                                                                                  \
        DISABLE_INTERRUPTS();                                                                                          \
        while (true)                                                                                                   \
        {                                                                                                              \
            PAUSE();                                                                                                   \
        }                                                                                                              \
    }

extern void ___enable_interrupts();
extern void ___disable_interrupts();
extern void ___pause();
