// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#ifndef _OE_SYSCALL_FCNTL_H
#define _OE_SYSCALL_FCNTL_H

#include <openenclave/bits/defs.h>
#include <openenclave/internal/syscall/bits/types.h>

OE_EXTERNC_BEGIN

// clang-format off
#define OE_O_RDONLY        000000000
#define OE_O_WRONLY        000000001
#define OE_O_RDWR          000000002
#define OE_O_CREAT         000000100
#define OE_O_EXCL          000000200
#define OE_O_NOCTTY        000000400
#define OE_O_TRUNC         000001000
#define OE_O_APPEND        000002000
#define OE_O_NONBLOCK      000004000
#define OE_O_DSYNC         000010000
#define OE_O_SYNC          004010000
#define OE_O_RSYNC         004010000
#define OE_O_DIRECTORY     000200000
#define OE_O_NOFOLLOW      000400000
#define OE_O_CLOEXEC       002000000
#define OE_O_ASYNC         000020000
#define OE_O_DIRECT        000040000
#define OE_O_LARGEFILE     000000000
#define OE_O_NOATIME       001000000
#define OE_O_PATH          010000000
#define OE_O_TMPFILE       020200000
#define OE_O_NDELAY        O_NONBLOCK
// clang-format on

#define __OE_FLOCK oe_flock
#include <openenclave/internal/syscall/bits/flock.h>
#undef __OE_FLOCK

#define __OE_FLOCK oe_flock64
#include <openenclave/internal/syscall/bits/flock.h>
#undef __OE_FLOCK

struct oe_f_owner_ex
{
    int type;
    oe_pid_t pid;
};

int oe_open(const char* pathname, int flags, oe_mode_t mode);

int oe_open_d(uint64_t devid, const char* pathname, int flags, oe_mode_t mode);

int __oe_fcntl(int fd, int cmd, uint64_t arg);

OE_EXTERNC_END

#endif /* _OE_SYSCALL_FCNTL_H */
