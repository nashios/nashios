.section .text
.align 4
.global scheduler_return_usermode
scheduler_return_usermode:
	cli

	mov $0x23, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	movl 4(%esp), %eax

	pushl 18 * 4(%eax)
	pushl 17 * 4(%eax)
	pushl 16 * 4(%eax)
	pushl 15 * 4(%eax)
	pushl 14 * 4(%eax)

	movl 4 * 4(%eax), %edi
	movl 5 * 4(%eax), %esi
	movl 6 * 4(%eax), %ebp
	movl 8 * 4(%eax), %ebx
	movl 9 * 4(%eax), %edx
	movl 10 * 4(%eax), %ecx
	movl 11 * 4(%eax), %eax

	iret
