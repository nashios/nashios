ENTRY(_start)
 
SECTIONS
{
	kernel_boot = 1M;
	kernel_higher_half = 0xC0000000;

	. = kernel_higher_half + kernel_boot;

	kernel_start = .;
	.text ALIGN(4096) : AT(ADDR(.text) - kernel_higher_half)
	{
		*(.multiboot)
		*(.text)
	}
	.rodata ALIGN(4096) : AT(ADDR(.rodata) - kernel_higher_half)
	{
		*(.rodata)
	}
	.data ALIGN(4096) : AT(ADDR(.data) - kernel_higher_half)
	{
		*(.data)
	}
	.bss ALIGN(4096) : AT(ADDR(.bss) - kernel_higher_half)
	{
		*(COMMON)
		*(.bss)
    }
	kernel_end = .;
}
