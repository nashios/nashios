#include <kernel/api/mouse.h>
#include <kernel/arch/i686/cpu/io.h>
#include <kernel/arch/i686/interrupts/pic.h>
#include <kernel/cpu/processor.h>
#include <kernel/drivers/mouse.h>
#include <kernel/filesystem/chardev.h>
#include <kernel/filesystem/devfs.h>
#include <kernel/interrupts/handler.h>
#include <kernel/lib/stdio.h>
#include <kernel/lib/stdlib.h>
#include <kernel/lib/string.h>
#include <kernel/task/wait.h>

#define MOUSE_MAX_EVENTS 16

#define MOUSE_PORT 0x60
#define MOUSE_STATUS 0x64

struct mouse_data
{
    bool ready;
    uint8_t head;
    uint8_t tail;
    struct mouse_event events[MOUSE_MAX_EVENTS];
    struct dlist_head list;
};

static struct wait_queue s_mouse_wait_queue = {};
static struct dlist_head s_data_list = {};
static struct mouse_event s_mouse_event = {};
static uint8_t s_mouse_cycle = 0;
static uint8_t s_mouse_byte[3] = {};

int mouse_open(struct vfs_inode *, struct vfs_file *file)
{
    struct mouse_data *mouse = (struct mouse_data *)calloc(1, sizeof(struct mouse_data));
    file->data = mouse;
    dlist_add_tail(&mouse->list, &s_data_list);

    return 0;
}

int mouse_poll(struct vfs_file *file, struct vfs_poll *poll)
{
    struct mouse_data *mouse = (struct mouse_data *)file->data;
    virtual_fs_poll_wait(file, &s_mouse_wait_queue, poll);
    return mouse->ready ? POLLIN : 0;
}

ssize_t mouse_read(struct vfs_file *file, char *buffer, size_t, loff_t)
{
    struct mouse_data *data = (struct mouse_data *)file->data;
    WAIT_EVENT(&s_mouse_wait_queue, data->ready);
    memcpy(buffer, &data->events[data->head], sizeof(struct mouse_event));

    if (data->tail != data->head)
        data->head = (data->head + 1) % MOUSE_MAX_EVENTS;
    else
        data->ready = false;

    return 0;
}

static struct vfs_file_op s_mouse_fop = {.open = mouse_open, .read = mouse_read, .poll = mouse_poll};

static struct chardev s_mouse_chardev = {
    .dev = MKDEV(13, 63), .major = 13, .baseminor = 0, .minorct = 63, .op = &s_mouse_fop};

bool mouse_handler(struct itr_registers *)
{
    uint8_t status = io_inb(MOUSE_STATUS);
    if ((status & 0x01) && (status & 0x20))
    {
        uint8_t data = io_inb(MOUSE_PORT);
        switch (s_mouse_cycle)
        {
        case 0:
            s_mouse_byte[0] = data;
            if (!(data & 0x08))
                break;
            s_mouse_cycle++;
            break;
        case 1:
            s_mouse_byte[1] = data;
            s_mouse_cycle++;
            break;
        case 2:
            s_mouse_byte[2] = data;
            uint8_t buttons = s_mouse_event.buttons;

            s_mouse_cycle = 0;
            uint8_t state = s_mouse_byte[0];
            int move_x = s_mouse_byte[1];
            int move_y = s_mouse_byte[2];

            if (move_x && state & (1 << 4))
                move_x = move_x - 0x100;

            if (move_y && state & (1 << 5))
                move_y = move_y - 0x100;

            if (state & (1 << 6) || state & (1 << 7))
            {
                move_x = 0;
                move_y = 0;
            }

            s_mouse_event.x = move_x;
            s_mouse_event.y = -move_y;

            s_mouse_event.state = s_mouse_event.buttons;
            s_mouse_event.buttons = 0;

            if (state & 0x01)
                s_mouse_event.buttons |= 0x01;

            if (state & 0x02)
                s_mouse_event.buttons |= 0x02;

            if (state & 0x04)
                s_mouse_event.buttons |= 0x04;

            if (s_mouse_event.x != 0 || s_mouse_event.y != 0 || s_mouse_event.buttons != 0 ||
                buttons != s_mouse_event.buttons)
            {
                struct mouse_data *iter;
                dlist_for_each_entry(iter, &s_data_list, list)
                {
                    if (iter->ready == true)
                    {
                        iter->tail = (iter->tail + 1) % MOUSE_MAX_EVENTS;
                        if (iter->tail == iter->head)
                            iter->head = (iter->head + 1) % MOUSE_MAX_EVENTS;
                    }
                    else
                        iter->head = iter->tail;

                    iter->events[iter->tail] = s_mouse_event;
                    iter->ready = true;
                }
                scheduler_wake_up(&s_mouse_wait_queue);
            }
            break;
        }
    }
    return ITR_CONTINUE;
}

void mouse_write(uint8_t value)
{
    io_outb(MOUSE_STATUS, 0xD4);
    io_outb(MOUSE_PORT, value);
}

void mouse_init()
{
    DISABLE_INTERRUPTS();

    dlist_head_init(&s_data_list);
    dlist_head_init(&s_mouse_wait_queue.list);

    chardev_set(&s_mouse_chardev);
    virtual_fs_mknod("/dev/input/mouse0", S_IFCHR, s_mouse_chardev.dev);

    while (io_inb(0x64) & 1)
        io_inb(0x60);

    io_outb(MOUSE_STATUS, 0xA8);
    io_inb(MOUSE_PORT);
    io_outb(MOUSE_STATUS, 0x20);

    uint8_t status = io_inb(0x60) | 3;
    io_outb(MOUSE_STATUS, 0x60);
    io_outb(MOUSE_PORT, status);
    mouse_write(0xF6);
    io_inb(MOUSE_PORT);
    mouse_write(0xF4);
    io_inb(MOUSE_PORT);

    io_outb(MOUSE_PORT, 0xF0);
    io_outb(MOUSE_PORT, 0x02);
    io_inb(MOUSE_PORT);

    itr_set_handler(44, mouse_handler);
    pic_unset_mask(12);

    ENABLE_INTERRUPTS();

    uint8_t tmp = io_inb(0x61);
    io_outb(0x61, tmp | 0x80);
    io_outb(0x61, tmp & 0x7F);
    io_inb(MOUSE_PORT);

    while ((io_inb(0x64) & 1))
        io_inb(0x60);

    printf("Mouse: Initialized\n");
}
