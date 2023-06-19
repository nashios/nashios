.section .init
.global _init
_init:
   push %rbp
   movq %rsp, %rbp

.section .fini
.global _fini
_fini:
   push %rbp
   movq %rsp, %rbp
