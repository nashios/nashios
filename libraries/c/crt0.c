#include <stdlib.h>
#include <unistd.h>

extern int main(int argc, char *argv[], char *envp[]);

void _start(int argc, char *argv[], char *envp[])
{
    environ = envp;

    int result = main(argc, argv, environ);
    exit(result);
}
