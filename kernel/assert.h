#pragma once

#define ASSERT(expression) ((expression) ? (void)0 : assert(#expression, __FILE__, __PRETTY_FUNCTION__, __LINE__))

#define NOT_REACHED() assert("NOT REACHED", __FILE__, __PRETTY_FUNCTION__, __LINE__);
#define TODO() assert("TODO", __FILE__, __PRETTY_FUNCTION__, __LINE__);

void assert(const char *message, const char *file, const char *function, int line);