.section .text
.align 4

.global gdt_flush
gdt_flush:
    mov 4(%esp), %eax
    lgdt (%eax)

    ljmp $0x08, $flush

flush:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    ret