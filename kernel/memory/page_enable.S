.section .text
.align 4

.global page_enable
page_enable:
    movl 4(%esp), %eax
    movl %eax, %cr3

    movl %cr4, %ecx
    andl $~0x00000010, %ecx
    movl %ecx, %cr4

    movl %cr0, %ecx
    orl $0x80000000, %ecx
    movl %ecx, %cr0
    
    ret