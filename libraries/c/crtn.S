.section .init, "ax", @progbits
    movl %ebp, %esp
    popl %ebp
    ret

.section .fini, "ax", @progbits
    movl %ebp, %esp
    popl %ebp
    ret
