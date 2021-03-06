/**
 * @file kernel/main.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Kernel entrypoint
 * @version 0.1
 * @date 2022-05-19
 *
 * @copyright Copyright (C) 2022 Saullo Bretas Silva
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include <kernel/boot/multiboot.h>
#include <kernel/drivers/ata.h>
#include <kernel/drivers/pci.h>
#include <kernel/filesystem/ext2.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/gdt.h>
#include <kernel/interrupts/idt.h>
#include <kernel/memory/physical.h>
#include <kernel/memory/virtual.h>
#include <kernel/serial.h>
#include <kernel/system/sys.h>
#include <kernel/system/syscall.h>
#include <kernel/task/scheduler.h>
#include <stdbool.h>

void kernel_init()
{
    pci_init();
    ata_init();
    virt_fs_init();
    ext2_fs_init();

    if (virt_fs_mount("/dev/hda", "/", "ext2", 0, NULL) < 0)
        PANIC("Failed to mount root filesystem", NULL);

    syscall_init();

    sched_open("/bin/system_service");

    sched_update_thread(sched_current_thread(), THREAD_WAIT);
    sched_schedule();

    while (true)
        ;
}

void kernel_main(uint32_t magic, uint32_t address)
{
    sys_cli();

    serial_init();
    gdt_init();
    idt_init();
    multiboot_init(magic, address);
    phys_mm_init();
    virt_mm_init();
    sched_init(kernel_init);
    sched_schedule();

    sys_sti();

    while (true)
        ;
}
