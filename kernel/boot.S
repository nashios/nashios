#define ASM_FILE 1
#include <kernel/boot/multiboot.h>

.set KERNEL_VIRTUAL_BASE, 0xC0000000
.set KERNEL_PAGE_NUMBER, (KERNEL_VIRTUAL_BASE >> 22)

.section .multiboot
.align 4
multiboot_header:
    .long MULTIBOOT_HEADER_MAGIC
    .long MULTIBOOT_HEADER_FLAGS
    .long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
#ifndef __ELF__
    .long multiboot_header
    .long _start
    .long _edata
    .long _end
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
    .skip 16384
stack_top:

.section .data
.align 0x1000
boot_dir:
    .int 0x00000083
    .fill KERNEL_PAGE_NUMBER - 1, 4, 0

    .int 0x00000083
    .fill 1024 - KERNEL_PAGE_NUMBER - 1, 4, 0

.section .text
.global _start
.type _start, @function
_start:
    movl $boot_dir - KERNEL_VIRTUAL_BASE, %ecx
    movl %ecx, %cr3

    movl %cr4, %ecx
    or $0x00000010, %ecx
    movl %ecx, %cr4

    movl %cr0, %ecx
    or $0x80000000, %ecx
    movl %ecx, %cr0

    lea (higher_half), %ecx
    jmp *%ecx

.section .text
higher_half:
    movl $0, (boot_dir)
    invlpg (0)

	mov $stack_top, %esp

    pushl $0
    popf

    add $KERNEL_VIRTUAL_BASE, %ebx
    pushl %ebx
    pushl %eax

	call kernel_main

	cli

1:
    hlt
    jmp 1b

.size _start, . - _start
