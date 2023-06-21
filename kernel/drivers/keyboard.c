#include <kernel/arch/x86/cpu/io.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/interrupts/handler.h>
#include <st/debug.h>

bool keyboard_handler(struct itr_registers *registers)
{
    uint8_t status = io_inb(0x64);
    if (status & 0x01)
    {
        uint8_t scancode = io_inb(0x60);
        dbgln("Keyboard: Scancode: %x", scancode);
    }

    return ITR_CONTINUE;
}

void keyboard_init()
{
    itr_set_handler(33, keyboard_handler);

    dbgln("Keyboard: Initialized");
}
