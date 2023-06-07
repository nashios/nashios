#include <assert.h>
#include <syscall.h>
#include <unistd.h>

extern "C"
{
    char **environ;
    char *optarg;
    int optind = 1;
    int opterr = 1;
    int optopt;

    int access(const char *, int) { assert(false); }

    unsigned int alarm(unsigned int) { assert(false); }

    int chdir(const char *) { assert(false); }

    int chown(const char *, uid_t, gid_t) { assert(false); }

    int close(int) { assert(false); }

    ssize_t confstr(int, char *, size_t) { assert(false); }

    char *ctermid(char *) { assert(false); }

    int dup(int) { assert(false); }

    int dup2(int, int) { assert(false); }

    __attribute__((__noreturn__)) void _exit(int) { assert(false); }

    void endusershell(void) { assert(false); }

    int execl(const char *, const char *, ...) { assert(false); }

    int execle(const char *, const char *, ...) { assert(false); }

    int execlp(const char *, const char *, ...) { assert(false); }

    int execv(const char *, char *const[]) { assert(false); }

    int execve(const char *, char *const[], char *const[]) { assert(false); }

    int execvp(const char *, char *const[]) { assert(false); }

    int execvpe(const char *, char *const[], char *const[]) { assert(false); }

    int faccessat(int, const char *, int, int) { assert(false); }

    int fchdir(int) { assert(false); }

    int fchown(int, uid_t, gid_t) { assert(false); }

    int fchownat(int, const char *, uid_t, gid_t, int) { assert(false); }

    int fdatasync(int) { assert(false); }

    int fexecve(int, char *const[], char *const[]) { assert(false); }

    pid_t fork(void) { assert(false); }

    pid_t vfork(void) { assert(false); }

    long fpathconf(int, int) { assert(false); }

    int fsync(int) { assert(false); }

    int ftruncate(int, off_t) { assert(false); }

    char *getcwd(char *, size_t) { assert(false); }

    gid_t getegid(void) { assert(false); }

    uid_t geteuid(void) { assert(false); }

    gid_t getgid(void) { assert(false); }

    int getgroups(int, gid_t[]) { assert(false); }

    long gethostid(void) { assert(false); }

    int gethostname(char *, size_t) { assert(false); }

    int sethostname(const char *, size_t) { assert(false); }

    char *getlogin(void) { assert(false); }

    int getlogin_r(char *, size_t) { assert(false); }

    int getopt(int, char *const[], const char *) { assert(false); }

    char *getpass(const char *) { assert(false); }

    pid_t getpgid(pid_t) { assert(false); }

    pid_t getpgrp(void) { assert(false); }

    pid_t getpid(void)
    {
        auto result = syscall(__NR_getpid);
        return static_cast<pid_t>(result);
    }

    pid_t getppid(void) { assert(false); }

    pid_t getsid(pid_t) { assert(false); }

    uid_t getuid(void) { assert(false); }

    char *getusershell(void) { assert(false); }

    int isatty(int) { assert(false); }

    int lchown(const char *, uid_t, gid_t) { assert(false); }

    int link(const char *, const char *) { assert(false); }

    int linkat(int, const char *, int, const char *, int) { assert(false); }

    int lockf(int, int, off_t) { assert(false); }

    off_t lseek(int, off_t, int) { assert(false); }

    off64_t lseek64(int, off64_t, int) { assert(false); }

    int nice(int) { assert(false); }

    long pathconf(const char *, int) { assert(false); }

    int pause(void) { assert(false); }

    int pipe(int[2]) { assert(false); }

    ssize_t pread(int, void *, size_t, off_t) { assert(false); }

    ssize_t pwrite(int, const void *, size_t, off_t) { assert(false); }

    ssize_t read(int, void *, size_t) { assert(false); }

    ssize_t readlink(const char *__restrict, char *__restrict, size_t) { assert(false); }

    ssize_t readlinkat(int, const char *__restrict, char *__restrict, size_t) { assert(false); }

    int rmdir(const char *) { assert(false); }

    int setegid(gid_t) { assert(false); }

    int seteuid(uid_t) { assert(false); }

    int setgid(gid_t) { assert(false); }

    int setpgid(pid_t, pid_t) { assert(false); }

    pid_t setpgrp(void) { assert(false); }

    int setregid(gid_t, gid_t) { assert(false); }

    int setreuid(uid_t, uid_t) { assert(false); }

    pid_t setsid(void) { assert(false); }

    int setuid(uid_t) { assert(false); }

    void setusershell(void) { assert(false); }

    unsigned int sleep(unsigned int) { assert(false); }

    void swab(const void *__restrict, void *__restrict, ssize_t) { assert(false); }

    int symlink(const char *, const char *) { assert(false); }

    int symlinkat(const char *, int, const char *) { assert(false); }

    void sync(void) { assert(false); }

    unsigned long sysconf(int) { assert(false); }

    pid_t tcgetpgrp(int) { assert(false); }

    int tcsetpgrp(int, pid_t) { assert(false); }

    int truncate(const char *, off_t) { assert(false); }

    char *ttyname(int) { assert(false); }

    int ttyname_r(int, char *, size_t) { assert(false); }

    int unlink(const char *) { assert(false); }

    int unlinkat(int, const char *, int) { assert(false); }

    ssize_t write(int, const void *, size_t) { assert(false); }

    int getpagesize(void) { assert(false); }

    char *get_current_dir_name(void) { assert(false); }

    int usleep(useconds_t) { assert(false); }

    int chroot(const char *) { assert(false); }

    int daemon(int, int) { assert(false); }

    pid_t gettid(void) { assert(false); }

    int getentropy(void *, size_t) { assert(false); }

    int pipe2(int *, int) { assert(false); }

    int setresuid(uid_t, uid_t, uid_t) { assert(false); }

    int setresgid(gid_t, gid_t, gid_t) { assert(false); }

    int getdomainname(char *, size_t) { assert(false); }

    int setdomainname(const char *, size_t) { assert(false); }

    int getresuid(uid_t *, uid_t *, uid_t *) { assert(false); }

    int getresgid(gid_t *, gid_t *, gid_t *) { assert(false); }

    char *crypt(const char *, const char *) { assert(false); }

    void encrypt(char[64], int) { assert(false); }

    int dup3(int, int, int) { assert(false); }

    int vhangup(void) { assert(false); }

    int getdtablesize(void) { assert(false); }

    int syncfs(int) { assert(false); }

    int __cxa_atexit(void (*function)(void *), void *, void *) { assert(false); }
}
