#define ASM_FILE
#include <kernel/multiboot.h>

.section .multiboot
.align 4
multiboot_header:
	.long MULTIBOOT_HEADER_MAGIC
	.long MULTIBOOT_HEADER_FLAGS
	.long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
#ifndef __ELF__
	.long multiboot_header
	.long _kernel_start
	.long _kernel_end
	.long _kernel_bss_end
	.long _start
#else
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
#endif
	.long 0
	.long 1024
	.long 768
	.long 32

.section .bss
.align 16
stack_bottom:
    .space 16384
stack_top:

.section .text
.global _start
.type _start, @function
_start:
	mov $stack_top, %esp

	pushl $0
	popf

	pushl %ebx
	pushl %eax

	call kernel_main

	cli

1:
	hlt
	jmp 1b

.size _start, . - _start
