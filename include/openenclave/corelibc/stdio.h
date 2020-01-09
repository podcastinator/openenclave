// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#ifndef _OE_STDIO_H
#define _OE_STDIO_H

#include <openenclave/bits/defs.h>
#include <openenclave/bits/types.h>
#include <openenclave/corelibc/stdarg.h>
#include <openenclave/corelibc/stdint.h>
#include <openenclave/corelibc/unistd.h>

OE_EXTERNC_BEGIN

#define OE_BUFSIZ 8192
#define OE_EOF (-1)

int oe_vsnprintf(char* str, size_t size, const char* format, oe_va_list ap);

OE_PRINTF_FORMAT(3, 4)
int oe_snprintf(char* str, size_t size, const char* format, ...);

int oe_vprintf(const char* format, oe_va_list ap);

OE_PRINTF_FORMAT(1, 2)
int oe_printf(const char* format, ...);

OE_EXTERNC_END

#endif /* _OE_STDIO_H */
