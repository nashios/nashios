.global page_enable
page_enable:
    movl 4(%esp), %eax
    movl %eax, %cr3
    
    movl %cr0, %eax
    or $0x80000000, %eax
    movl %eax, %cr0
    
    ret
