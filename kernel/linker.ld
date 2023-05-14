ENTRY(_start)

SECTIONS
{
	_kernel_boot = 0x00100000;
	. = _kernel_boot;

	_kernel_start = .;
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
	.bss BLOCK(4K) : ALIGN(4K)
	{
		_kernel_bss_start = .;
		*(COMMON)
		*(.bss)
		_kernel_bss_end = .;
	}
	_kernel_end = .;
	_kernel_size = _kernel_end - _kernel_start;
}