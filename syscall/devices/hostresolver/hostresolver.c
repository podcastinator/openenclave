// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#define _GNU_SOURCE

// clang-format off
#include <openenclave/enclave.h>
// clang-format on

#include <openenclave/internal/syscall/device.h>
#include <openenclave/internal/syscall/raise.h>
#include <openenclave/internal/syscall/sys/socket.h>
#include <openenclave/internal/syscall/netdb.h>
#include <openenclave/internal/syscall/resolver.h>
#include <openenclave/bits/safemath.h>
#include <openenclave/internal/syscall/bits/exports.h>
#include <openenclave/internal/syscall/bits/exports.h>
#include <openenclave/internal/syscall/stdio.h>
#include <openenclave/syscall/module.h>
#include <openenclave/internal/syscall/bits/exports.h>
#include <pthread.h>
#include <netdb.h>
#include "syscall_t.h"

#define RESOLV_MAGIC 0x536f636b

// The host resolver is not actually a device in the file descriptor sense.
typedef struct _resolver
{
    struct _oe_resolver base;
    uint32_t magic;
} resolver_t;

static resolver_t* _cast_resolver(const oe_resolver_t* device)
{
    resolver_t* resolver = (resolver_t*)device;

    if (resolver == NULL || resolver->magic != RESOLV_MAGIC)
        return NULL;

    return resolver;
}

static resolver_t _hostresolver;

static int _hostresolver_getnameinfo(
    oe_resolver_t* dev,
    const struct oe_sockaddr* sa,
    socklen_t salen,
    char* host,
    socklen_t hostlen,
    char* serv,
    socklen_t servlen,
    int flags)
{
    int ret = EAI_FAIL;

    OE_UNUSED(dev);

    errno = 0;

    if (oe_syscall_getnameinfo_ocall(
            &ret, sa, salen, host, hostlen, serv, servlen, flags) != OE_OK)
    {
        goto done;
    }

done:

    return ret;
}

static int _hostresolver_getaddrinfo(
    oe_resolver_t* resolver,
    const char* node,
    const char* service,
    const struct oe_addrinfo* hints,
    struct oe_addrinfo** res)
{
    int ret = EAI_FAIL;
    uint64_t handle = 0;
    struct oe_addrinfo* head = NULL;
    struct oe_addrinfo* tail = NULL;
    struct oe_addrinfo* p = NULL;

    OE_UNUSED(resolver);

    if (res)
        *res = NULL;

    if (!res)
    {
        ret = EAI_SYSTEM;
        OE_RAISE_ERRNO(EINVAL);
    }

    /* Get the handle for enumerating addrinfo structures. */
    {
        int retval = EAI_FAIL;

        if (oe_syscall_getaddrinfo_open_ocall(
                &retval, node, service, hints, &handle) != OE_OK)
        {
            ret = EAI_SYSTEM;
            OE_RAISE_ERRNO(EINVAL);
        }

        if (!handle)
        {
            ret = retval;
            goto done;
        }
    }

    /* Enumerate addrinfo structures. */
    for (;;)
    {
        int retval = 0;
        size_t canonnamelen = 0;

        if (!(p = calloc(1, sizeof(struct oe_addrinfo))))
        {
            ret = EAI_MEMORY;
            goto done;
        }

        /* Determine required size ai_addr and ai_canonname buffers. */
        if (oe_syscall_getaddrinfo_read_ocall(
                &retval,
                handle,
                &p->ai_flags,
                &p->ai_family,
                &p->ai_socktype,
                &p->ai_protocol,
                p->ai_addrlen,
                &p->ai_addrlen,
                NULL,
                canonnamelen,
                &canonnamelen,
                NULL) != OE_OK)
        {
            ret = EAI_SYSTEM;
            OE_RAISE_ERRNO(EINVAL);
        }

        /* If this is the final element in the enumeration. */
        if (retval == 1)
            break;

        /* Expecting that addr and canonname buffers were too small. */
        if (retval != -1 || errno != ENAMETOOLONG)
        {
            ret = EAI_SYSTEM;
            OE_RAISE_ERRNO(errno);
        }

        if (p->ai_addrlen && !(p->ai_addr = calloc(1, p->ai_addrlen)))
        {
            ret = EAI_MEMORY;
            goto done;
        }

        if (canonnamelen && !(p->ai_canonname = calloc(1, canonnamelen)))
        {
            ret = EAI_MEMORY;
            goto done;
        }

        if (oe_syscall_getaddrinfo_read_ocall(
                &retval,
                handle,
                &p->ai_flags,
                &p->ai_family,
                &p->ai_socktype,
                &p->ai_protocol,
                p->ai_addrlen,
                &p->ai_addrlen,
                p->ai_addr,
                canonnamelen,
                &canonnamelen,
                p->ai_canonname) != OE_OK)
        {
            ret = EAI_SYSTEM;
            OE_RAISE_ERRNO(EINVAL);
        }

        /* Append to the list. */
        if (tail)
        {
            tail->ai_next = p;
            tail = p;
        }
        else
        {
            head = p;
            tail = p;
        }

        p = NULL;
    }

    /* Close the enumeration. */
    if (handle)
    {
        int retval = -1;

        if (oe_syscall_getaddrinfo_close_ocall(&retval, handle) != OE_OK)
        {
            ret = EAI_SYSTEM;
            OE_RAISE_ERRNO(EINVAL);
        }

        handle = 0;

        if (retval != 0)
        {
            ret = EAI_SYSTEM;
            OE_RAISE_ERRNO(errno);
        }
    }

    /* If the list is empty. */
    if (!head)
    {
        ret = EAI_SYSTEM;
        OE_RAISE_ERRNO(EINVAL);
    }

    *res = head;
    head = NULL;
    tail = NULL;
    ret = 0;

done:

    if (handle)
    {
        int retval;
        oe_syscall_getaddrinfo_close_ocall(&retval, handle);
    }

    if (head)
        oe_freeaddrinfo(head);

    if (p)
        oe_freeaddrinfo(p);

    return ret;
}

static int _hostresolver_release(oe_resolver_t* resolv_)
{
    int ret = -1;
    resolver_t* resolver = _cast_resolver(resolv_);

    errno = 0;

    if (!resolver)
        OE_RAISE_ERRNO(EINVAL);

    // resolv_ is a static object, there is no need to free
    ret = 0;

done:
    return ret;
}

// clang-format off
static oe_resolver_ops_t _ops =
{
    .getaddrinfo = _hostresolver_getaddrinfo,
    .getnameinfo = _hostresolver_getnameinfo,
    .release = _hostresolver_release
};
// clang-format on

// clang-format off
static resolver_t _hostresolver =
{
    .base.type = OE_RESOLVER_TYPE_HOST,
    .base.ops = &_ops,
    .magic = RESOLV_MAGIC
};
// clang-format on

static pthread_spinlock_t _lock;

static __attribute__((constructor)) void _init_lock(void)
{
    pthread_spin_init(&_lock, PTHREAD_PROCESS_PRIVATE);
}

oe_result_t oe_load_module_host_resolver(void)
{
    oe_result_t result = OE_UNEXPECTED;
    static bool _loaded = false;

    pthread_spin_lock(&_lock);

    if (!_loaded)
    {
        oe_result_t r = oe_load_module_syscall();

        if (r != OE_OK)
        {
            result = r;
            goto done;
        }

        if (oe_register_resolver(&_hostresolver.base) != 0)
            OE_RAISE_ERRNO(errno);

        _loaded = true;
    }

    result = OE_OK;

done:
    pthread_spin_unlock(&_lock);

    return result;
}
