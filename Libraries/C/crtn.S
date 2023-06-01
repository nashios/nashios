.section .init, "ax", @progbits
    popl %ebp
    ret

.section .fini, "ax", @progbits
    popl %ebp
    ret
