// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#ifndef _OE_SYSCALL_ERRNO_H
#define _OE_SYSCALL_ERRNO_H

#include <openenclave/bits/defs.h>
#include <openenclave/internal/syscall/bits/exports.h>

OE_EXTERNC_BEGIN

#define OE_EPERM 1
#define OE_ENOENT 2
#define OE_ESRCH 3
#define OE_EINTR 4
#define OE_EIO 5
#define OE_ENXIO 6
#define OE_E2BIG 7
#define OE_ENOEXEC 8
#define OE_EBADF 9
#define OE_ECHILD 10
#define OE_EAGAIN 11
#define OE_ENOMEM 12
#define OE_EACCES 13
#define OE_EFAULT 14
#define OE_ENOTBLK 15
#define OE_EBUSY 16
#define OE_EEXIST 17
#define OE_EXDEV 18
#define OE_ENODEV 19
#define OE_ENOTDIR 20
#define OE_EISDIR 21
#define OE_EINVAL 22
#define OE_ENFILE 23
#define OE_EMFILE 24
#define OE_ENOTTY 25
#define OE_ETXTBSY 26
#define OE_EFBIG 27
#define OE_ENOSPC 28
#define OE_ESPIPE 29
#define OE_EROFS 30
#define OE_EMLINK 31
#define OE_EPIPE 32
#define OE_EDOM 33
#define OE_ERANGE 34
#define OE_EDEADLK 35
#define OE_ENAMETOOLONG 36
#define OE_ENOLCK 37
#define OE_ENOSYS 38
#define OE_ENOTEMPTY 39
#define OE_ELOOP 40
#define OE_EWOULDBLOCK OE_EAGAIN
#define OE_ENOMSG 42
#define OE_EIDRM 43
#define OE_ECHRNG 44
#define OE_EL2NSYNC 45
#define OE_EL3HLT 46
#define OE_EL3RST 47
#define OE_ELNRNG 48
#define OE_EUNATCH 49
#define OE_ENOCSI 50
#define OE_EL2HLT 51
#define OE_EBADE 52
#define OE_EBADR 53
#define OE_EXFULL 54
#define OE_ENOANO 55
#define OE_EBADRQC 56
#define OE_EBADSLT 57
#define OE_EDEADLOCK OE_EDEADLK
#define OE_EBFONT 59
#define OE_ENOSTR 60
#define OE_ENODATA 61
#define OE_ETIME 62
#define OE_ENOSR 63
#define OE_ENONET 64
#define OE_ENOPKG 65
#define OE_EREMOTE 66
#define OE_ENOLINK 67
#define OE_EADV 68
#define OE_ESRMNT 69
#define OE_ECOMM 70
#define OE_EPROTO 71
#define OE_EMULTIHOP 72
#define OE_EDOTDOT 73
#define OE_EBADMSG 74
#define OE_EOVERFLOW 75
#define OE_ENOTUNIQ 76
#define OE_EBADFD 77
#define OE_EREMCHG 78
#define OE_ELIBACC 79
#define OE_ELIBBAD 80
#define OE_ELIBSCN 81
#define OE_ELIBMAX 82
#define OE_ELIBEXEC 83
#define OE_EILSEQ 84
#define OE_ERESTART 85
#define OE_ESTRPIPE 86
#define OE_EUSERS 87
#define OE_ENOTSOCK 88
#define OE_EDESTADDRREQ 89
#define OE_EMSGSIZE 90
#define OE_EPROTOTYPE 91
#define OE_ENOPROTOOPT 92
#define OE_EPROTONOSUPPORT 93
#define OE_ESOCKTNOSUPPORT 94
#define OE_EOPNOTSUPP 95
#define OE_ENOTSUP OE_EOPNOTSUPP
#define OE_EPFNOSUPPORT 96
#define OE_EAFNOSUPPORT 97
#define OE_EADDRINUSE 98
#define OE_EADDRNOTAVAIL 99
#define OE_ENETDOWN 100
#define OE_ENETUNREACH 101
#define OE_ENETRESET 102
#define OE_ECONNABORTED 103
#define OE_ECONNRESET 104
#define OE_ENOBUFS 105
#define OE_EISCONN 106
#define OE_ENOTCONN 107
#define OE_ESHUTDOWN 108
#define OE_ETOOMANYREFS 109
#define OE_ETIMEDOUT 110
#define OE_ECONNREFUSED 111
#define OE_EHOSTDOWN 112
#define OE_EHOSTUNREACH 113
#define OE_EALREADY 114
#define OE_EINPROGRESS 115
#define OE_ESTALE 116
#define OE_EUCLEAN 117
#define OE_ENOTNAM 118
#define OE_ENAVAIL 119
#define OE_EISNAM 120
#define OE_EREMOTEIO 121
#define OE_EDQUOT 122
#define OE_ENOMEDIUM 123
#define OE_EMEDIUMTYPE 124
#define OE_ECANCELED 125
#define OE_ENOKEY 126
#define OE_EKEYEXPIRED 127
#define OE_EKEYREVOKED 128
#define OE_EKEYREJECTED 129
#define OE_EOWNERDEAD 130
#define OE_ENOTRECOVERABLE 131
#define OE_ERFKILL 132
#define OE_EHWPOISON 133
#define OE_EPROCLIM 134
// clang-format on

#define oe_errno *__oe_errno_location()

OE_EXTERNC_END

#endif // _OE_SYSCALL_ERRNO_H
