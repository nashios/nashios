#include <kernel/api/posix/errno.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/stdio.h>
#include <kernel/string.h>

static struct dlist_head s_virtual_type_list;

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