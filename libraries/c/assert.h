#pragma once

#define assert(expression) ((expression) ? (void)0 : _assert(#expression, __FILE__, __PRETTY_FUNCTION__, __LINE__))
#define assert_not_reached() _assert("NOT REACHED", __FILE__, __PRETTY_FUNCTION__, __LINE__);
#define assert_todo() _assert("TODO", __FILE__, __PRETTY_FUNCTION__, __LINE__);

void _assert(const char *message, const char *file, const char *function, int line);
