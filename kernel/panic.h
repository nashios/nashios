#pragma once

#include <st/debug.h>

#define PANIC(format, ...)                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        dbgln("Panic: " format, ##__VA_ARGS__);                                                                        \
        panic(__FILE__, __LINE__, __PRETTY_FUNCTION__);                                                                \
    } while (0)

__attribute__((noreturn)) void panic(const char *file, int line, const char *function);
