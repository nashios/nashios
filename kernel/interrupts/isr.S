.macro ISR_NOERROR index
    .global isr\index
    isr\index:
        cli
        push $0
        push $\index
        jmp isr_common
.endm

.macro ISR_ERROR index
    .global isr\index
    isr\index:
        cli
        push $\index
        jmp isr_common
.endm

ISR_NOERROR 0
ISR_NOERROR 1
ISR_NOERROR 2
ISR_NOERROR 3
ISR_NOERROR 4
ISR_NOERROR 5
ISR_NOERROR 6
ISR_NOERROR 7
ISR_ERROR 8
ISR_NOERROR 9
ISR_ERROR 10
ISR_ERROR 11
ISR_ERROR 12
ISR_ERROR 13
ISR_ERROR 14
ISR_NOERROR 15
ISR_NOERROR 16
ISR_ERROR 17
ISR_NOERROR 18
ISR_NOERROR 19
ISR_NOERROR 20
ISR_NOERROR 21
ISR_NOERROR 22
ISR_NOERROR 23
ISR_NOERROR 24
ISR_NOERROR 25
ISR_NOERROR 26
ISR_NOERROR 27
ISR_NOERROR 28
ISR_NOERROR 29
ISR_ERROR 30
ISR_NOERROR 31
ISR_NOERROR 127

isr_common:
    pusha

    push %ds
    push %es
    push %fs
    push %gs

    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    cld

    push %esp
    call isr_handler
    add $4, %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds

    popa
    add $8, %esp
    iret
