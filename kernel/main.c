#include <kernel/assert.h>
#include <kernel/drivers/ata.h>
#include <kernel/drivers/fb.h>
#include <kernel/drivers/pci.h>
#include <kernel/filesystem/ext2.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/gdt.h>
#include <kernel/interrupts/idt.h>
#include <kernel/memory/physical.h>
#include <kernel/memory/virtual.h>
#include <kernel/multiboot.h>
#include <kernel/pit.h>
#include <kernel/processor.h>
#include <kernel/serial.h>
#include <kernel/stdio.h>
#include <kernel/task/scheduler.h>

void kernel_init()
{
    scheduler_unlock();

    pci_init();
    ata_init();
    virtual_fs_init();
    ext2_fs_init();
    fb_init();

    scheduler_open("/bin/system_service");

    scheduler_update_thread(g_scheduler_thread, THREAD_WAITING_STATE);
    scheduler_schedule();

    while (true)
        ;
}

void kernel_main(uint32_t magic, uint32_t address)
{
    DISABLE_INTERRUPTS();

    serial_init(SERIAL_COM1);
    multiboot_init(magic, address);
    gdt_init();
    idt_init();
    pit_init();
    physical_mm_init();
    virtual_mm_init();
    scheduler_init(kernel_init);
    scheduler_schedule();

    ENABLE_INTERRUPTS();
    while (true)
        ;
}
