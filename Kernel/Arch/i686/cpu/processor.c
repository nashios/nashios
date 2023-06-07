#include <Kernel/cpu/processor.h>

void ___enable_interrupts() { asm volatile("sti"); }

void ___disable_interrupts() { asm volatile("cli"); }

void ___pause() { asm volatile("hlt"); }
