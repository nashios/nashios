#ifndef _STDLIB_H
#define _STDLIB_H

#ifdef __cplusplus
extern "C"
{
#endif

    void abort(void);
    int atexit(void (*)(void));
    int atoi(const char *);
    void free(void *);
    char *getenv(const char *);
    void *malloc(size_t);

    /**
     * @brief Exit the program with the given status.
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/exit.html
     *
     * @param status The exit status.
     */
    void exit(int status);

#ifdef __cplusplus
}
#endif

#endif
