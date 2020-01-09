// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#ifndef _OE_SYSCALL_SYS_SOCKET_H
#define _OE_SYSCALL_SYS_SOCKET_H

#include <openenclave/bits/defs.h>
#include <openenclave/bits/types.h>
#include <openenclave/internal/syscall/bits/types.h>
#include <openenclave/internal/syscall/sys/uio.h>

OE_EXTERNC_BEGIN

#define __OE_SOCKADDR oe_sockaddr
#include <openenclave/internal/syscall/sys/bits/sockaddr.h>
#undef __OE_SOCKADDR

#define __OE_SOCKADDR_STORAGE oe_sockaddr_storage
#include <openenclave/internal/syscall/sys/bits/sockaddr_storage.h>
#undef __OE_SOCKADDR_STORAGE

#define __OE_IOVEC oe_iovec
#define __OE_MSGHDR oe_msghdr
#include <openenclave/internal/syscall/sys/bits/msghdr.h>
#undef __OE_IOVEC
#undef __OE_MSGHDR

void oe_set_default_socket_devid(uint64_t devid);

uint64_t oe_get_default_socket_devid(void);

int oe_socket(int domain, int type, int protocol);

int oe_socket_d(uint64_t devid, int domain, int type, int protocol);

int oe_socketpair(int domain, int type, int protocol, int rtnfd[2]);

int oe_accept(int sockfd, struct oe_sockaddr* addr, oe_socklen_t* addrlen);

int oe_bind(int sockfd, const struct oe_sockaddr* addr, oe_socklen_t namelen);

int oe_connect(
    int sockfd,
    const struct oe_sockaddr* addr,
    oe_socklen_t namelen);

int oe_shutdown(int sockfd, int how);

int oe_listen(int sockfd, int backlog);

int oe_setsockopt(
    int sockfd,
    int level,
    int optname,
    const void* optval,
    oe_socklen_t optlen);

int oe_getsockopt(
    int sockfd,
    int level,
    int optname,
    void* optval,
    oe_socklen_t* optlen);

ssize_t oe_send(int sockfd, const void* buf, size_t len, int flags);

ssize_t oe_recv(int sockfd, void* buf, size_t len, int flags);

ssize_t oe_sendto(
    int sockfd,
    const void* buf,
    size_t len,
    int flags,
    const struct oe_sockaddr* dest_addr,
    oe_socklen_t addrlen);

ssize_t oe_recvfrom(
    int sockfd,
    void* buf,
    size_t len,
    int flags,
    const struct oe_sockaddr* src_addr,
    oe_socklen_t* addrlen);

ssize_t oe_sendmsg(int sockfd, const struct oe_msghdr* buf, int flags);

ssize_t oe_recvmsg(int sockfd, struct oe_msghdr* buf, int flags);

int oe_getpeername(int sockfd, struct oe_sockaddr* addr, oe_socklen_t* addrlen);

int oe_getsockname(int sockfd, struct oe_sockaddr* addr, oe_socklen_t* addrlen);

OE_EXTERNC_END

#endif /* _OE_SYSCALL_SYS_SOCKET_H */
