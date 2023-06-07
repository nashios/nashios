.section .text
.global flush_tbl
flush_tbl:
    movl 4(%esp), %eax
    invlpg (%eax)
    ret