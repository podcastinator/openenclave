// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#include <errno.h>
#include <openenclave/internal/syscall/raise.h>

void __oe_raise_errno(int err)
{
    errno = err;
}
