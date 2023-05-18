#include <kernel/api/posix/errno.h>
#include <kernel/drivers/ata.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/math.h>
#include <kernel/stdio.h>
#include <kernel/stdlib.h>
#include <kernel/string.h>

static struct dlist_head s_virtual_type_list;

char *virtual_fs_read_block(const char *source, sector_t sector, uint32_t size)
{
    struct ata_device *device = ata_find_device(source);
    if (!device)
        return NULL;

    size_t target_size = DIV_ROUND_UP(size, VFS_BYTES_P_SECTOR) * VFS_BYTES_P_SECTOR;
    char *buffer = (char *)calloc(target_size, sizeof(char));
    if (!buffer)
        return NULL;

    uint8_t sectors = DIV_ROUND_UP(size, VFS_BYTES_P_SECTOR);
    ata_read(device, sector, sectors, (uint16_t *)buffer);

    return buffer;
}

struct vfs_type *virtual_fs_find_type(const char *name)
{
    struct vfs_type *type;
    dlist_for_each_entry(type, &s_virtual_type_list, list)
    {
        if (strcmp(type->name, name) == 0)
            return type;
    }
    return NULL;
}

int virtual_fs_set_type(struct vfs_type *type)
{
    struct vfs_type *found = virtual_fs_find_type(type->name);
    if (found)
        return -EBUSY;

    dlist_add_tail(&type->list, &s_virtual_type_list);
    printf("Virtual FS: Registered filesystem type name = %s\n", type->name);

    return 0;
}

void virtual_fs_init()
{
    dlist_head_init(&s_virtual_type_list);

    printf("Virtual FS: Initialized\n");
}