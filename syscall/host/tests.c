// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#include <openenclave/internal/syscall/bits/tests.h>

#if defined(__linux__)
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

void __oe_test_fail(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    abort();
}
