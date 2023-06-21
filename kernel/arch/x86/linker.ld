ENTRY(_start)

_kernel_boot = 0x00100000;
_kernel_higher_half = 0xC0000000;

SECTIONS
{
	. = _kernel_boot;
	_kernel_start = .;

	.multiboot.data :
	{
		*(.multiboot.data)
	}

	.multiboot.text :
	{
		*(.multiboot.text)
	}

	. += _kernel_higher_half;
	.text ALIGN(4K) : AT(ADDR(.text) - _kernel_higher_half)
	{
		_text_start = .;
		*(.text)
		*(.text.*)
		_text_end = .;
	}
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - _kernel_higher_half)
	{
		_rodata_start = .;
		*(.rodata)
		*(.rodata.*)
		_rodata_end = .;
	}
	.data ALIGN(4K) : AT(ADDR(.data) - _kernel_higher_half)
	{
		_data_start = .;
		*(.data)
		*(.data.*)
		_data_end = .;
	}
	.bss ALIGN(4K) : AT(ADDR(.bss) - _kernel_higher_half)
	{
		_bss_start = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		_bss_end = .;
	}
	_kernel_end = .;
}
