#include <stdlib.h>
#include <unistd.h>

void exit(int status) { _exit(status); }

void _Exit(int status) { exit(status); }

void abort(void) { _exit(1); }
