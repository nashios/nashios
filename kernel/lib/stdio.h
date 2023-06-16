#pragma once

#include <stdarg.h>
#include <stddef.h>

int vsnprintf(char *str, size_t size, const char *format, va_list ap);
int sprintf(char *s, const char *format, ...);
