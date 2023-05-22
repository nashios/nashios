extern int main(int argc, char **argv, char **envp);

void _start(int argc, char **argv, char **envp)
{
    while (1)
        ;
    main(argc, argv, envp);
    while (1)
        ;
}
