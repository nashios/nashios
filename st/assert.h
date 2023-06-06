#pragma once

#if KERNEL
#include <Kernel/lib/assert.h>
#else
#include <assert.h>
#endif

#define assert_not_reached() assert(false)
#define assert_todo() assert(false)
