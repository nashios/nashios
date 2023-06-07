#ifndef _SETJMP_H
#define _SETJMP_H

#include <bits/machine.h>
#include <bits/signal.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct __jmp_buf
    {
        struct __libc_jmpbuf_register_state reg_state;
    } jmp_buf[1];

    typedef struct
    {
        struct __libc_jmpbuf_register_state reg_state;
        int savesigs;
        sigset_t sigset;
    } sigjmp_buf[1];

    __attribute__((__returns_twice__)) int setjmp(jmp_buf buffer);
    __attribute__((__noreturn__)) void longjmp(jmp_buf buffer, int value);
    __attribute__((__returns_twice__)) int sigsetjmp(sigjmp_buf buffer, int savesigs);
    __attribute__((__noreturn__)) void siglongjmp(sigjmp_buf buffer, int value);

#ifdef __cplusplus
}
#endif

#endif
