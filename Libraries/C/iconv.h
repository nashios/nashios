#pragma once

#include <stddef.h>
#include <sys/cdefs.h>

___BEGIN_DECLS

typedef void *iconv_t;

iconv_t iconv_open(const char *tocode, const char *fromcode);
size_t iconv(iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
int iconv_close(iconv_t cd);

___END_DECLS
