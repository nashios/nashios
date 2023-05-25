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

	mov 4(%esp), %eax

	push 18 * 4(%eax)
	push 17 * 4(%eax)
	push 16 * 4(%eax)
	push 15 * 4(%eax)
	push 14 * 4(%eax)

	mov 4 * 4(%eax), %edi
	mov 5 * 4(%eax), %esi
	mov 6 * 4(%eax), %ebp
	mov 8 * 4(%eax), %ebx
	mov 9 * 4(%eax), %edx
	mov 10 * 4(%eax), %ecx
	mov 11 * 4(%eax), %eax

	iret
