.set MULTIBOOT_HEADER_MAGIC, 0x1BADB002

#ifdef __ELF__
.set MULTIBOOT_AOUT_KLUDGE, 0x00000000
#else
.set MULTIBOOT_AOUT_KLUDGE, 0x00010000
#endif

.set MULTIBOOT_PAGE_ALIGN, 0x00000001
.set MULTIBOOT_MEMORY_INFO, 0x00000002
.set MULTIBOOT_VIDEO_MODE, 0x00000004

.set MULTIBOOT_HEADER_FLAGS, MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE | MULTIBOOT_AOUT_KLUDGE

.section .multiboot.data, "aw"
.align 4
multiboot_header:
	.long MULTIBOOT_HEADER_MAGIC
	.long MULTIBOOT_HEADER_FLAGS
	.long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
#ifndef __ELF__
	.long multiboot_header
	.long _start
	.long _data_end
	.long _bss_end
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

.section .bss, "aw", @nobits
stack_bottom:
    .space 16384
stack_top:

.set VIRTUAL_ADDRESS, 0xC0000000
.set PAGE_NUMBER, (VIRTUAL_ADDRESS >> 22)

.section .data
.align 4096
boot_directory:
	.long 0x00000083
	.fill (PAGE_NUMBER - 1), 4, 0x00
	.long 0x00000083
	.fill (1024 - PAGE_NUMBER - 1), 4, 0x00

.section .multiboot.text, "a"
.global _start
.type _start, @function
_start:
	movl $(boot_directory - VIRTUAL_ADDRESS), %ecx
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
.type higher_half, @function
higher_half:
	movl $0x00, (boot_directory)
	invlpg (0)

	mov $stack_top, %esp

	pushl $0
	popf

	add $VIRTUAL_ADDRESS, %ebx
	pushl %ebx
	pushl %eax

	call arch_main

	cli

1:
	hlt
	jmp 1b
