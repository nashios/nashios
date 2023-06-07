ENTRY(_start)

SECTIONS
{
	_Kernel_boot = 0x00100000;
	_Kernel_higher_half = 0xC0000000;
	. = _Kernel_higher_half + _Kernel_boot;

	_Kernel_start = .;
	.text ALIGN(4096) : AT(ADDR(.text) - _Kernel_higher_half)
	{
		*(.multiboot)
		*(.text .text.*)
	}
	.rodata ALIGN(4096) : AT(ADDR(.rodata) - _Kernel_higher_half)
	{
		*(.rodata .rodata.*)
	}
	.data ALIGN(4096) : AT(ADDR(.data) - _Kernel_higher_half)
	{
		*(.data .data.*)
	}
	.bss ALIGN(4096) : AT(ADDR(.bss) - _Kernel_higher_half)
	{
		_Kernel_bss_start = .;
		*(COMMON)
		*(.bss .bss.*)
		_Kernel_bss_end = .;
	}
	_Kernel_end = .;
	_Kernel_size = _Kernel_end - _Kernel_start;
}
