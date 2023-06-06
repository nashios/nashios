#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#include <bits/time_t.h>
#include <bits/blkcnt_t.h>
#include <bits/blksize_t.h>
#include <bits/dev_t.h>
#include <bits/fd_set.h>
#include <bits/fsblkcnt_t.h>
#include <bits/fsfilcnt_t.h>
#include <bits/gid_t.h>
#include <bits/id_t.h>
#include <bits/ino_t.h>
#include <bits/mode_t.h>
#include <bits/nlink_t.h>
#include <bits/off_t.h>
#include <bits/pid_t.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>
#include <bits/suseconds_t.h>
#include <bits/uid_t.h>
#include <stdint.h>
#include <sys/select.h>

typedef unsigned int u_int;
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned long int u_long;
typedef char *caddr_t;
typedef off64_t loff_t;

typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;

typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;

typedef int64_t quad_t;
typedef uint64_t u_quad_t;

#endif
