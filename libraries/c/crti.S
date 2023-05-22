.section .init
.align 4
.global _init
_init:
    pushl %ebp
    movl %esp, %ebp

.section .fini
.align 4
.global _fini
_fini:
    pushl %ebp
    movl %esp, %ebp
