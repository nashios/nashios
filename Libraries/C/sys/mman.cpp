#include <assert.h>
#include <sys/mman.h>

extern "C"
{
    void *mmap(void *, size_t, int, int, int, off_t) { assert(false); }

    int mprotect(void *, size_t, int) { assert(false); }

    int munmap(void *, size_t) { assert(false); }

    int mlock(const void *, size_t) { assert(false); }

    int mlockall(int) { assert(false); }

    int munlock(const void *, size_t) { assert(false); }

    int munlockall(void) { assert(false); }

    int posix_madvise(void *, size_t, int) { assert(false); }

    int msync(void *, size_t, int) { assert(false); }

    int shm_open(const char *, int, mode_t) { assert(false); }

    int shm_unlink(const char *) { assert(false); }

    void *mremap(void *, size_t, size_t, int, ...) { assert(false); }

    int remap_file_pages(void *, size_t, int, size_t, int) { assert(false); }

    int memfd_create(const char *, unsigned int) { assert(false); }

    int madvise(void *, size_t, int) { assert(false); }

    int mincore(void *, size_t, unsigned char *) { assert(false); }
}
