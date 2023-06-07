#include <assert.h>
#include <setjmp.h>

extern "C"
{
    __attribute__((__returns_twice__)) int setjmp(jmp_buf buffer) { assert(false); }

    __attribute__((__noreturn__)) void longjmp(jmp_buf buffer, int value) { assert(false); }

    __attribute__((__returns_twice__)) int sigsetjmp(sigjmp_buf buffer, int savesigs) { assert(false); }

    __attribute__((__noreturn__)) void siglongjmp(sigjmp_buf buffer, int value) { assert(false); }
}
