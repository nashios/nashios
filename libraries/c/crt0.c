#include <stdlib.h>

extern int main(int argc, char *argv[], char *envp[]);

void _start(int argc, char *argv[], char *envp[])
{
    int result = main(argc, argv, envp);
    exit(result);
}
