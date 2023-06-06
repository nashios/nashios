#include <assert.h>
#include <fcntl.h>

extern "C"
{
    int creat(const char *, mode_t) { assert(false); }

    int fallocate(int, int, off_t, off_t) { assert(false); }

    int fcntl(int, int, ...) { assert(false); }

    int open(const char *, int, ...) { assert(false); }

    int openat(int, const char *, int, ...) { assert(false); }

    int posix_fadvise(int, off_t, off_t, int) { assert(false); }

    int posix_fallocate(int, off_t, off_t) { assert(false); }

    int name_to_handle_at(int, const char *, struct file_handle *, int *, int) { assert(false); }

    int open_by_handle_at(int, struct file_handle *, int) { assert(false); }

    ssize_t splice(int, off_t *, int, off_t *, size_t, unsigned int) { assert(false); }

    ssize_t vmsplice(int, const struct iovec *, size_t, unsigned int) { assert(false); }
}
