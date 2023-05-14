#pragma once

#include <kernel/stdio.h>

#define PANIC(...)                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        printf("Panic: Message = ");                                                                                   \
        printf(__VA_ARGS__);                                                                                           \
        panic(__FILE__, __LINE__, __PRETTY_FUNCTION__);                                                                \
    } while (0)

void panic(const char *file, int line, const char *function);
