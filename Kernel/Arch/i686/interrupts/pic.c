#include <Kernel/Arch/i686/cpu/io.h>
#include <Kernel/Arch/i686/interrupts/pic.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21

#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define PIC_EOI 0x20

#define PIC_ICW1_ICW4 0x01
#define PIC_ICW1_INIT 0x10

#define PIC_ICW4_8086 0x01

void pic_remap(uint8_t offset1, uint8_t offset2)
{
    uint8_t mask1 = io_inb(PIC1_DATA);
    uint8_t mask2 = io_inb(PIC1_DATA);

    io_outb(PIC1_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();
    io_outb(PIC2_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();
    io_outb(PIC1_DATA, offset1);
    io_wait();
    io_outb(PIC2_DATA, offset2);
    io_wait();
    io_outb(PIC1_DATA, 4);
    io_wait();
    io_outb(PIC2_DATA, 2);
    io_wait();
    io_outb(PIC1_DATA, PIC_ICW4_8086);
    io_wait();
    io_outb(PIC2_DATA, PIC_ICW4_8086);
    io_wait();
    io_outb(PIC1_DATA, mask1);
    io_outb(PIC2_DATA, mask2);
}

void pic_send_eoi(uint8_t irq)
{
    if (irq >= 40)
        io_outb(PIC2_COMMAND, PIC_EOI);
    io_outb(PIC1_COMMAND, PIC_EOI);
}

void pic_set_mask(uint8_t irq)
{
    uint16_t port = 0;
    if (irq < 8)
        port = PIC1_DATA;
    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }

    uint8_t value = io_inb(port) | (1 << irq);
    io_outb(port, value);
}

void pic_unset_mask(uint8_t irq)
{
    uint16_t port = 0;
    if (irq < 8)
        port = PIC1_DATA;
    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }

    uint8_t value = io_inb(port) & ~(1 << irq);
    io_outb(port, value);
}
