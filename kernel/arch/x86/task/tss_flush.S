.section .text
.align 4
.global tss_flush
tss_flush:
	mov $0x2B, %ax
	ltr %ax
	ret
