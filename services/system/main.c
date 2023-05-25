#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

void system_open(const char *path)
{
    pid_t pid = fork();
    if (!pid)
        execve(path, NULL, NULL);
}

int main()
{
    system_open("/bin/window");
    system_open("/bin/desktop");

    while (true)
        ;
}
