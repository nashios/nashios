.section .text
.align 4
.global scheduler_switch
scheduler_switch:
    cli
    pusha

    movl (8 + 1) * 4(%esp), %eax
    movl %esp, (%eax)

    movl (8 + 2) * 4(%esp), %eax
    movl (8 + 3) * 4(%esp), %ebx
    movl %eax, %esp
    movl %ebx, %cr3

    popa
    sti
    ret