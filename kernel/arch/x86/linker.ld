ENTRY(_start)

SECTIONS
{
	_kernel_boot = 0x00100000;
	_kernel_higher_half = 0xC0000000;
	. = _kernel_higher_half + _kernel_boot;

	_kernel_start = .;
	.text ALIGN(4096) : AT(ADDR(.text) - _kernel_higher_half)
	{
		*(.multiboot)
		*(.text .text.*)
	}
	.rodata ALIGN(4096) : AT(ADDR(.rodata) - _kernel_higher_half)
	{
		*(.rodata .rodata.*)
	}
	.data ALIGN(4096) : AT(ADDR(.data) - _kernel_higher_half)
	{
		*(.data .data.*)
	}
	.bss ALIGN(4096) : AT(ADDR(.bss) - _kernel_higher_half)
	{
		*(COMMON)
		*(.bss .bss.*)
	}
	_kernel_end = .;
	_kernel_size = _kernel_end - _kernel_start;
}
