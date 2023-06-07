#include <Kernel/Init.hpp>

extern "C"
{
#include <Kernel/cpu/processor.h>
#include <Kernel/drivers/ata.h>
#include <Kernel/drivers/fb.h>
#include <Kernel/drivers/pci.h>
#include <Kernel/drivers/pit.h>
#include <Kernel/filesystem/chardev.h>
#include <Kernel/filesystem/devfs.h>
#include <Kernel/filesystem/ext2.h>
#include <Kernel/filesystem/mqueuefs.h>
#include <Kernel/filesystem/tmpfs.h>
#include <Kernel/filesystem/virtual.h>
#include <Kernel/interrupts/handler.h>
#include <Kernel/ipc/mqueue.h>
#include <Kernel/system/syscall.h>
#include <Kernel/task/scheduler.h>
}

namespace Kernel
{
    void Init::main()
    {
        itr_init();
        pit_init();

        scheduler_init((void *)stage2);
        scheduler_schedule();

        ENABLE_INTERRUPTS();
        while (true)
            ;
    }

    void Init::stage2()
    {
        scheduler_unlock();

        pci_init();
        ata_init();
        virtual_fs_init();
        ext2_fs_init();
        devfs_init();
        chardev_init();
        mqueuefs_init();
        tmpfs_init();
        fb_init();
        syscall_init();
        mq_init();

        scheduler_open("/bin/SystemService");

        scheduler_update_thread(g_scheduler_thread, THREAD_WAITING_STATE);
        scheduler_schedule();

        while (true)
            ;
    }
} // namespace Kernel
