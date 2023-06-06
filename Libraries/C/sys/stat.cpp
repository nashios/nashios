#include <assert.h>
#include <sys/stat.h>

extern "C"
{
    int chmod(const char *, mode_t) { assert(false); }

    int fchmod(int, mode_t) { assert(false); }

    int fchmodat(int, const char *, mode_t, int) { assert(false); }

    int fstat(int fd, struct stat *result) { assert(false); }

    int fstatat(int, const char *__restrict, struct stat *__restrict, int) { assert(false); }

    int futimens(int fd, const struct timespec times[2]) { assert(false); }

    int lstat(const char *__restrict, struct stat *__restrict) { assert(false); }

    int mkdir(const char *, mode_t) { assert(false); }

    int mkdirat(int, const char *, mode_t) { assert(false); }

    int mkfifo(const char *, mode_t) { assert(false); }

    int mkfifoat(int, const char *, mode_t) { assert(false); }

    int mknod(const char *, mode_t, dev_t) { assert(false); }

    int mknodat(int, const char *, mode_t, dev_t) { assert(false); }

    int stat(const char *__restrict, struct stat *__restrict) { assert(false); }

    mode_t umask(mode_t) { assert(false); }

    int utimensat(int, const char *, const struct timespec times[2], int) { assert(false); }
}
