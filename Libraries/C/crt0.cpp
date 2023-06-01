#include <stdlib.h>

extern "C"
{
    extern int main(int argc, char **argv, char **envp);

    void _start(int argc, char **argv, char **envp)
    {
        int status = main(argc, argv, envp);
        exit(status);
    }
}
