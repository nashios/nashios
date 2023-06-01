#pragma once

#include <kernel/lib/stdio.h>

#define PANIC(...)                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        printf("Panic: Message = ");                                                                                   \
        printf(__VA_ARGS__);                                                                                           \
        panic(__FILE__, __LINE__, __PRETTY_FUNCTION__);                                                                \
    } while (0)

__attribute__((noreturn)) void panic(const char *file, int line, const char *function);
