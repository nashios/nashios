#pragma once

#include <bits/pid_t.h>
#include <bits/size_t.h>
#include <bits/uid_t.h>
#include <stdint.h>
#include <time.h>

#define SA_NOCLDSTOP 1
#define SA_NOCLDWAIT 2
#define SA_SIGINFO 4
#define SA_ONSTACK 0x08000000
#define SA_RESTART 0x10000000
#define SA_NODEFER 0x40000000
#define SA_RESETHAND 0x80000000
#define SA_RESTORER 0x04000000

#define SIGABRT 6
#define SIGFPE 8
#define SIGILL 4
#define SIGINT 2
#define SIGSEGV 11
#define SIGTERM 15
#define SIGPROF 27
#define SIGIO 29
#define SIGPWR 30
#define SIGRTMIN 35
#define SIGRTMAX 64

#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

#define SIGHUP 1
#define SIGQUIT 3
#define SIGTRAP 5
#define SIGIOT SIGABRT
#define SIGBUS 7
#define SIGKILL 9
#define SIGUSR1 10
#define SIGUSR2 12
#define SIGPIPE 13
#define SIGALRM 14
#define SIGSTKFLT 16
#define SIGCHLD 17
#define SIGCONT 18
#define SIGSTOP 19
#define SIGTSTP 20
#define SIGTTIN 21
#define SIGTTOU 22
#define SIGURG 23
#define SIGXCPU 24
#define SIGXFSZ 25
#define SIGVTALRM 26
#define SIGWINCH 28
#define SIGPOLL 29
#define SIGSYS 31
#define SIGUNUSED SIGSYS
#define SIGCANCEL 32

#define MINSIGSTKSZ 2048
#define SIGSTKSZ 8192
#define SS_ONSTACK 1
#define SS_DISABLE 2

#define SIGEV_SIGNAL 0
#define SIGEV_NONE 1
#define SIGEV_THREAD 2

#define SEGV_MAPERR 1
#define SEGV_ACCERR 2

#define BUS_ADRALN 1
#define BUS_ADRERR 2
#define BUS_OBJERR 3
#define BUS_MCEERR_AR 4
#define BUS_MCEERR_AO 5

#define ILL_ILLOPC 1
#define ILL_ILLOPN 2
#define ILL_ILLADR 3
#define ILL_ILLTRP 4
#define ILL_PRVOPC 5
#define ILL_PRVREG 6
#define ILL_COPROC 7
#define ILL_BADSTK 8
#define ILL_BADIADDR 9

#define NSIG 65

#define SI_ASYNCNL (-60)
#define SI_TKILL (-6)
#define SI_SIGIO (-5)
#define SI_ASYNCIO (-4)
#define SI_MESGQ (-3)
#define SI_TIMER (-2)
#define SI_QUEUE (-1)
#define SI_USER 0
#define SI_KERNEL 128

#define REG_R8 0
#define REG_R9 1
#define REG_R10 2
#define REG_R11 3
#define REG_R12 4
#define REG_R13 5
#define REG_R14 6
#define REG_R15 7
#define REG_RDI 8
#define REG_RSI 9
#define REG_RBP 10
#define REG_RBX 11
#define REG_RDX 12
#define REG_RAX 13
#define REG_RCX 14
#define REG_RSP 15
#define REG_RIP 16
#define REG_EFL 17
#define REG_CSGSFS 18
#define REG_ERR 19
#define REG_TRAPNO 20
#define REG_OLDMASK 21
#define REG_CR2 22
#define NGREG 23

#ifdef __cplusplus
extern "C"
{
#endif

    union sigval {
        int sival_int;
        void *sival_ptr;
    };

    typedef struct
    {
        int si_signo;
        int si_errno;
        int si_code;
        union {
            char __pad[128 - 2 * sizeof(int) - sizeof(long)];
            struct
            {
                union {
                    struct
                    {
                        pid_t si_pid;
                        uid_t si_uid;
                    } __piduid;
                    struct
                    {
                        int si_timerid;
                        int si_overrun;
                    } __timer;
                } __first;
                union {
                    union sigval si_value;
                    struct
                    {
                        int si_status;
                        clock_t si_utime, si_stime;
                    } __sigchld;
                } __second;
            } __si_common;
            struct
            {
                void *si_addr;
                short si_addr_lsb;
                union {
                    struct
                    {
                        void *si_lower;
                        void *si_upper;
                    } __addr_bnd;
                    unsigned si_pkey;
                } __first;
            } __sigfault;
            struct
            {
                long si_band;
                int si_fd;
            } __sigpoll;
            struct
            {
                void *si_call_addr;
                int si_syscall;
                unsigned si_arch;
            } __sigsys;
        } __si_fields;
    } siginfo_t;

#define si_pid __si_fields.__si_common.__first.__piduid.si_pid
#define si_uid __si_fields.__si_common.__first.__piduid.si_uid
#define si_status __si_fields.__si_common.__second.__sigchld.si_status
#define si_utime __si_fields.__si_common.__second.__sigchld.si_utime
#define si_stime __si_fields.__si_common.__second.__sigchld.si_stime
#define si_value __si_fields.__si_common.__second.si_value
#define si_addr __si_fields.__sigfault.si_addr
#define si_addr_lsb __si_fields.__sigfault.si_addr_lsb
#define si_lower __si_fields.__sigfault.__first.__addr_bnd.si_lower
#define si_upper __si_fields.__sigfault.__first.__addr_bnd.si_upper
#define si_pkey __si_fields.__sigfault.__first.si_pkey
#define si_band __si_fields.__sigpoll.si_band
#define si_fd __si_fields.__sigpoll.si_fd
#define si_timerid __si_fields.__si_common.__first.__timer.si_timerid
#define si_overrun __si_fields.__si_common.__first.__timer.si_overrun
#define si_ptr si_value.sival_ptr
#define si_int si_value.sival_int
#define si_call_addr __si_fields.__sigsys.si_call_addr
#define si_syscall __si_fields.__sigsys.si_syscall
#define si_arch __si_fields.__sigsys.si_arch

    typedef long sigset_t;

    typedef void (*__sighandler)(int);

#define SIG_ERR ((__sighandler)(void *)(-1))
#define SIG_DFL ((__sighandler)(void *)(0))
#define SIG_IGN ((__sighandler)(void *)(1))

    typedef struct __stack
    {
        void *ss_sp;
        int ss_flags;
        size_t ss_size;
    } stack_t;

    struct sigevent
    {
        union sigval sigev_value;
        int sigev_notify;
        int sigev_signo;
        void (*sigev_notify_function)(union sigval);
    };

    struct sigaction
    {
        union {
            void (*sa_handler)(int);
            void (*sa_sigaction)(int, siginfo_t *, void *);

#define sa_handler __sa_handler.sa_handler
#define sa_sigaction __sa_handler.sa_sigaction

        } __sa_handler;
        sigset_t sa_mask;
        int sa_flags;
        void (*sa_restorer)(void);
    };

#if defined(__i386__)
    struct _fpreg
    {
        unsigned short significand[4];
        unsigned short exponent;
    };

    struct _fpxreg
    {
        unsigned short significand[4];
        unsigned short exponent;
        unsigned short padding[3];
    };

    struct _xmmreg
    {
        uint32_t element[4];
    };

    struct _fpx_sw_bytes
    {
        uint32_t magic1;
        uint32_t extended_size;
        uint64_t xfeatures;
        uint32_t xstate_size;
        uint32_t padding[7];
    };

    struct _fpstate
    {
        uint32_t cw;
        uint32_t sw;
        uint32_t tag;
        uint32_t ipoff;
        uint32_t cssel;
        uint32_t dataoff;
        uint32_t datasel;
        struct _fpreg _st[8];
        uint16_t status;
        uint16_t magic;
        uint32_t _fxsr_env[6];
        uint32_t mxcsr;
        uint32_t reserved;
        struct _fpxreg _fxsr_st[8];
        struct _xmmreg _xmm[8];
        union {
            uint32_t padding1[44];
            uint32_t padding[44];
        };

        union {
            uint32_t padding2[12];
            struct _fpx_sw_bytes sw_reserved;
        };
    };

    typedef struct
    {
        unsigned long gregs[NGREG];
        struct _fpstate *fpregs;
        unsigned long __reserved1[8];
    } mcontext_t;

    typedef struct __ucontext
    {
        unsigned long uc_flags;
        struct __ucontext *uc_link;
        stack_t uc_stack;
        mcontext_t uc_mcontext;
        sigset_t uc_sigmask;
    } ucontext_t;

#else
#error "Missing architecture specific code."
#endif

#ifdef __cplusplus
}
#endif
