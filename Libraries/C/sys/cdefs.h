#pragma once

#ifdef __cplusplus
#ifndef ___BEGIN_DECLS
#define ___BEGIN_DECLS                                                                                                 \
    extern "C"                                                                                                         \
    {
#define ___END_DECLS }
#endif
#else
#ifndef ___BEGIN_DECLS
#define ___BEGIN_DECLS
#define ___END_DECLS
#endif
#endif
