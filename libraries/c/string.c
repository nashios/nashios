#include <string.h>
#include <stdint.h>

static const char *string_error_list[] = {
    "Success",
    "Operation not permitted",
    "No such file or directory",
    "No such process",
    "Interrupted function call",
    "Input/output error",
    "No such device or address",
    "Argument list too long",
    "Executable file format error",
    "Bad file descriptor",
    "No child process",
    "Resource temporarily unavailable",
    "Not enough space",
    "Permission denied",
    "Bad address",
    "Block device required",
    "Resource busy",
    "File exists",
    "Improper link",
    "No such device",
    "Not a directory",
    "Is a directory",
    "Invalid argument",
    "Too many files open in system",
    "File descriptor value too large or too many open streams",
    "Inappropriate I/O control operation",
    "Text file busy",
    "File too large",
    "No space left on a device",
    "Invalid seek",
    "Read-only file system",
    "Too many links",
    "Broken pipe",
    "Domain error",
    "Result too large or too small",
    "Reserved",
    "Reserved",
    "Reserved",
    "Function not implemented"};
static int string_error_list_size = sizeof(string_error_list) / sizeof(string_error_list[0]);

void *memset(void *s, int c, size_t n)
{
    uint8_t *p_s = s;

    for (size_t i = 0; i < n; i++)
        p_s[i] = c;

    return s;
}

void *memcpy(void *restrict s1, const void *restrict s2, size_t n)
{
    char *p_s1 = s1;
    const char *p_s2 = s2;

    for (size_t i = 0; i < n; i++)
        p_s1[i] = p_s2[i];

    return s1;
}

size_t strlen(const char *str)
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

char *strerror(int errnum)
{
    if (errnum < 0 || errnum >= string_error_list_size)
        return "Unknown error";
    return (char *)string_error_list[errnum];
}
