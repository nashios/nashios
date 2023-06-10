#pragma once

#include <sys/cdefs.h>

___BEGIN_DECLS

void (*signal(int sig, void (*func)(int)))(int);
int raise(int sig);

___END_DECLS
