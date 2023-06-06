#pragma once

typedef __UINT8_TYPE__ __libc_uint8;
typedef __UINT16_TYPE__ __libc_uint16;
typedef __UINT32_TYPE__ __libc_uint32;
typedef __UINT64_TYPE__ __libc_uint64;

typedef __INT8_TYPE__ __libc_int8;
typedef __INT16_TYPE__ __libc_int16;
typedef __INT32_TYPE__ __libc_int32;
typedef __INT64_TYPE__ __libc_int64;

#define __LIBC_INT8_C(x) __INT8_C(x)
#define __LIBC_INT16_C(x) __INT16_C(x)
#define __LIBC_INT32_C(x) __INT32_C(x)
#define __LIBC_INT64_C(x) __libc_int64_C(x)

#define __LIBC_UINT8_C(x) __UINT8_C(x)
#define __LIBC_UINT16_C(x) __UINT16_C(x)
#define __LIBC_UINT32_C(x) __UINT32_C(x)
#define __LIBC_UINT64_C(x) __UINT64_C(x)

#define __LIBC_INTMAX_C(x) __INTMAX_C(x)
#define __LIBC_UINTMAX_C(x) __UINTMAX_C(x)

#define __LIBC_INT8_MAX __INT8_MAX__
#define __LIBC_INT16_MAX __INT16_MAX__
#define __LIBC_INT32_MAX __INT32_MAX__
#define __LIBC_INT64_MAX __libc_int64_MAX__

#define __LIBC_INT8_MIN (-__LIBC_INT8_MAX - 1)
#define __LIBC_INT16_MIN (-__LIBC_INT16_MAX - 1)
#define __LIBC_INT32_MIN (-__LIBC_INT32_MAX - 1)
#define __LIBC_INT64_MIN (-__LIBC_INT64_MAX - 1)

#define __LIBC_UINT8_MAX __UINT8_MAX__
#define __LIBC_UINT16_MAX __UINT16_MAX__
#define __LIBC_UINT32_MAX __UINT32_MAX__
#define __LIBC_UINT64_MAX __UINT64_MAX__

#if defined(__i386__)

typedef __libc_int8 __libc_int_fast8;
#define __LIBC_INT_FAST8_C(x) __LIBC_INT8_C(x)
#define __LIBC_INT_FAST8_MAX __LIBC_INT8_MAX
#define __LIBC_INT_FAST8_MIN __LIBC_INT8_MIN

typedef __libc_int32 __libc_int_fast16;
#define __LIBC_INT_FAST16_C(x) __LIBC_INT32_C(x)
#define __LIBC_INT_FAST16_MAX __LIBC_INT32_MAX
#define __LIBC_INT_FAST16_MIN __LIBC_INT32_MIN

typedef __libc_int32 __libc_int_fast32;
#define __LIBC_INT_FAST32_C(x) __LIBC_INT32_C(x)
#define __LIBC_INT_FAST32_MAX __LIBC_INT32_MAX
#define __LIBC_INT_FAST32_MIN __LIBC_INT32_MIN

typedef __libc_int64 __libc_int_fast64;
#define __LIBC_INT_FAST64_C(x) __LIBC_INT64_C(x)
#define __LIBC_INT_FAST64_MAX __LIBC_INT64_MAX
#define __LIBC_INT_FAST64_MIN __LIBC_INT64_MIN

#else
#error "Missing architecture specific code"
#endif

#if defined(__i386__)

typedef __libc_uint8 __libc_uint_fast8;
#define __LIBC_UINT_FAST8_C(x) __LIBC_UINT8_C(x)
#define __LIBC_UINT_FAST8_MAX __LIBC_UINT8_MAX
#define __LIBC_UINT_FAST8_MIN __LIBC_UINT8_MIN

typedef __libc_uint32 __libc_uint_fast16;
#define __LIBC_UINT_FAST16_C(x) __LIBC_UINT32_C(x)
#define __LIBC_UINT_FAST16_MAX __LIBC_UINT32_MAX
#define __LIBC_UINT_FAST16_MIN __LIBC_UINT32_MIN

typedef __libc_uint32 __libc_uint_fast32;
#define __LIBC_UINT_FAST32_C(x) __LIBC_UINT32_C(x)
#define __LIBC_UINT_FAST32_MAX __LIBC_UINT32_MAX
#define __LIBC_UINT_FAST32_MIN __LIBC_UINT32_MIN

typedef __libc_uint64 __libc_uint_fast64;
#define __LIBC_UINT_FAST64_C(x) __LIBC_UINT64_C(x)
#define __LIBC_UINT_FAST64_MAX __LIBC_UINT64_MAX
#define __LIBC_UINT_FAST64_MIN __LIBC_UINT64_MIN

#else
#error "Missing architecture specific code"
#endif

typedef __INTMAX_TYPE__ __libc_intmax;
typedef __INTPTR_TYPE__ __libc_intptr;
typedef __PTRDIFF_TYPE__ __libc_ptrdiff;
#define __LIBC_INTMAX_MAX __INTMAX_MAX__
#define __LIBC_INTMAX_MIN (-__INTMAX_MAX__ - 1)
#define __LIBC_INTPTR_MAX __INTPTR_MAX__
#define __LIBC_INTPTR_MIN (-__INTPTR_MAX__ - 1)
#define __LIBC_PTRDIFF_MAX __PTRDIFF_MAX__
#define __LIBC_PTRDIFF_MIN (-__PTRDIFF_MAX__ - 1)

typedef __UINTMAX_TYPE__ __libc_uintmax;
typedef __UINTPTR_TYPE__ __libc_uintptr;
typedef __SIZE_TYPE__ __libc_size;
#define __LIBC_UINTMAX_MAX __UINTMAX_MAX__
#define __LIBC_UINTPTR_MAX __UINTPTR_MAX__
#define __LIBC_SIZE_MAX __SIZE_MAX__

#define __LIBC_WCHAR_MAX __WCHAR_MAX__
#define __LIBC_WCHAR_MIN __WCHAR_MIN__

#define __LIBC_WINT_MAX __WINT_MAX__
#define __LIBC_WINT_MIN __WINT_MIN__

#define __LIBC_SIG_ATOMIC_MAX __SIG_ATOMIC_MAX__
#define __LIBC_SIG_ATOMIC_MIN __SIG_ATOMIC_MIN__

#if defined(__cpp_static_assert)
#define __LIBC_STATIC_ASSERT(c, text) static_assert(c, text)
#elif !defined(__cplusplus)
#define __LIBC_STATIC_ASSERT(c, text) _Static_assert(c, text)
#else
#define __LIBC_STATIC_ASSERT(c, text)
#endif

#define __LIBC_CHECK_TYPE(T1, T2) __LIBC_STATIC_ASSERT(sizeof(T1) == sizeof(T2), #T1 " != " #T2);

__LIBC_CHECK_TYPE(__libc_int8, __INT_LEAST8_TYPE__);
__LIBC_CHECK_TYPE(__libc_int16, __INT_LEAST16_TYPE__);
__LIBC_CHECK_TYPE(__libc_int32, __INT_LEAST32_TYPE__);
__LIBC_CHECK_TYPE(__libc_int64, __INT_LEAST64_TYPE__);

__LIBC_CHECK_TYPE(__libc_uint8, __UINT_LEAST8_TYPE__);
__LIBC_CHECK_TYPE(__libc_uint16, __UINT_LEAST16_TYPE__);
__LIBC_CHECK_TYPE(__libc_uint32, __UINT_LEAST32_TYPE__);
__LIBC_CHECK_TYPE(__libc_uint64, __UINT_LEAST64_TYPE__);

#ifndef __clang__
__LIBC_CHECK_TYPE(__libc_int_fast8, __INT_FAST8_TYPE__);
__LIBC_CHECK_TYPE(__libc_int_fast16, __INT_FAST16_TYPE__);
__LIBC_CHECK_TYPE(__libc_int_fast32, __INT_FAST32_TYPE__);
__LIBC_CHECK_TYPE(__libc_int_fast64, __INT_FAST64_TYPE__);

__LIBC_CHECK_TYPE(__libc_uint_fast8, __UINT_FAST8_TYPE__);
__LIBC_CHECK_TYPE(__libc_uint_fast16, __UINT_FAST16_TYPE__);
__LIBC_CHECK_TYPE(__libc_uint_fast32, __UINT_FAST32_TYPE__);
__LIBC_CHECK_TYPE(__libc_uint_fast64, __UINT_FAST64_TYPE__);
#endif
