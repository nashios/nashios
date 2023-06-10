#pragma once

#include <bits/types.h>
#include <sys/cdefs.h>

___BEGIN_DECLS

struct __jmp_buf_tag
{
};

typedef struct __jmp_buf_tag jmp_buf[1];
typedef struct __jmp_buf_tag sigjmp_buf[1];
typedef __sig_atomic_t sig_atomic_t;

int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);

___END_DECLS
