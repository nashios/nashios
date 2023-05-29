#pragma once

#include <kernel/api/posix/sys/types.h>
#include <kernel/arch/i686/memory/virtual.h>
#include <kernel/interrupts/handler.h>
#include <st/plist.h>

#define SCHED_STACK_SIZE 0x2000
#define SCHED_HEAP_SIZE 0x20000
#define SCHED_HEAP_TOP 0x40000000

enum thread_state
{
    THREAD_READY_STATE,
    THREAD_RUNNING_STATE,
    THREAD_WAITING_STATE,
    THREAD_TERMINATED_STATE
};

enum thread_type
{
    THREAD_KERNEL_TYPE,
    THREAD_APPLICATION_TYPE
};

struct process_mm
{
    uint32_t cache;
    uint32_t code_start;
    uint32_t code_end;
    uint32_t data_start;
    uint32_t data_end;
    uint32_t brk_start;
    uint32_t brk_middle;
    uint32_t brk_end;
    struct dlist_head list;
};

struct vfs_file;
struct process_vm
{
    uint32_t start;
    uint32_t end;
    uint32_t flags;
    struct process_mm *memory;
    struct vfs_file *file;
    struct dlist_head list;
};

struct process_files
{
    struct vfs_file *fd[MAX_FD];
};

struct vfs_mount;
struct process_fs
{
    struct vfs_dentry *dentry;
    struct vfs_mount *mount;
};

struct thread;
struct process
{
    const char *name;
    int exit_code;
    pid_t pid;
    struct process *parent;
    struct thread *thread;
    struct process_mm *memory;
    struct process_files *files;
    struct process_fs *fs;
    struct page_directory *directory;
    struct dlist_head sibling;
    struct dlist_head children;
};

struct thread
{
    pid_t tid;
    uint32_t timing;
    uint32_t esp;
    uint32_t kernel_stack;
    uint32_t user_stack;
    enum thread_type type;
    enum thread_state state;
    struct itr_registers registers;
    struct process *process;
    struct plist_node plist;
};

extern struct process *g_scheduler_process;
extern struct thread *g_scheduler_thread;

void scheduler_init(void *init);
void scheduler_schedule();
void scheduler_lock();
void scheduler_unlock();
void scheduler_queue_thread(struct thread *thread);
void scheduler_update_thread(struct thread *thread, enum thread_state state);
void scheduler_open(const char *path);
void scheduler_exit(int code);
struct process *scheduler_fork(struct process *parent);
int scheduler_execve(const char *path, char *const argv[], char *const envp[]);
