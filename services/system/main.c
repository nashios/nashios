#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void system_mount_all_fs()
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork()");
        return;
    }
    else if (!pid)
    {
        if (execl("/bin/mount", "mount", "-a", NULL) < 0)
        {
            perror("execl()");
            return;
        }
    }
    else
        wait(NULL);
}

int main()
{
    system_mount_all_fs();

    while (1)
        ;
}
