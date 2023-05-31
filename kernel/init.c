#include <kernel/cpu/processor.h>
#include <kernel/drivers/ata.h>
#include <kernel/drivers/fb.h>
#include <kernel/drivers/pci.h>
#include <kernel/drivers/pit.h>
#include <kernel/filesystem/devfs.h>
#include <kernel/filesystem/ext2.h>
#include <kernel/filesystem/mqueuefs.h>
#include <kernel/filesystem/tmpfs.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/interrupts/handler.h>
#include <kernel/ipc/mqueue.h>
#include <kernel/system/syscall.h>
#include <kernel/task/scheduler.h>

void kernel_init()
{
    scheduler_unlock();

    pci_init();
    ata_init();
    virtual_fs_init();
    ext2_fs_init();
    devfs_init();
    mqueuefs_init();
    tmpfs_init();
    fb_init();
    syscall_init();
    mq_init();

    scheduler_open("/bin/system_service");

    scheduler_update_thread(g_scheduler_thread, THREAD_WAITING_STATE);
    scheduler_schedule();

    while (true)
        ;
}

void kernel_main()
{
    itr_init();
    pit_init();

    scheduler_init(kernel_init);
    scheduler_schedule();

    ENABLE_INTERRUPTS();
    while (true)
        ;
}
