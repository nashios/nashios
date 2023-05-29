.macro isr_noerror number
    .global isr\number
    isr\number:
        cli
        push $0
        push $\number
        jmp isr_common
.endm

.macro isr_error number
    .global isr\number
    isr\number:
        cli
        push $\number
        jmp isr_common
.endm

isr_noerror 0
isr_noerror 1
isr_noerror 2
isr_noerror 3
isr_noerror 4
isr_noerror 5
isr_noerror 6
isr_noerror 7
isr_error 8
isr_noerror 9
isr_error 10
isr_error 11
isr_error 12
isr_error 13
isr_error 14
isr_noerror 15
isr_noerror 16
isr_error 17
isr_noerror 18
isr_noerror 19
isr_noerror 20
isr_error 21
isr_noerror 22
isr_noerror 23
isr_noerror 24
isr_noerror 25
isr_noerror 26
isr_noerror 27
isr_noerror 28
isr_noerror 29
isr_noerror 30
isr_noerror 31
isr_noerror 128

.section .text
.align 4
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
    call itr_handler
    add $4, %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds

    popa
    add $8, %esp
    iret
