#pragma once

#include <stdint.h>

#if defined(__i386__)
struct __libc_jmpbuf_register_state
{
    uint32_t ebx;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t esp;
    uint32_t eip;
};
#else
#error "Missing architecture specific code"
#endif
