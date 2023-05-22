#pragma once

#define DIV_ROUND_UP(number, divisor) (((number) + (divisor)-1) / (divisor))

#define ALIGN_DOWN(base, size) ((base) & -((__typeof__(base))(size)))
#define ALIGN_UP(base, size) ALIGN_DOWN((base) + (size)-1, (size))

#define MIN(x, y)                                                                                                      \
    ({                                                                                                                 \
        typeof(x) p_x = (x);                                                                                           \
        typeof(y) p_y = (y);                                                                                           \
        (void)(&p_x == &p_y);                                                                                          \
        p_x < p_y ? p_x : p_y;                                                                                         \
    })

#define MAX(x, y)                                                                                                      \
    ({                                                                                                                 \
        typeof(x) p_x = (x);                                                                                           \
        typeof(y) p_y = (y);                                                                                           \
        (void)(&p_x == &p_y);                                                                                          \
        p_x > p_y ? p_x : p_y;                                                                                         \
    })

#define MIN_T(type, x, y)                                                                                              \
    ({                                                                                                                 \
        type __x = (x);                                                                                                \
        type __y = (y);                                                                                                \
        __x < __y ? __x : __y;                                                                                         \
    })

#define MAX_T(type, x, y)                                                                                              \
    ({                                                                                                                 \
        type __x = (x);                                                                                                \
        type __y = (y);                                                                                                \
        __x > __y ? __x : __y;                                                                                         \
    })
