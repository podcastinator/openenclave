// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#ifndef _OE_SYSCALL_INTERNAL_EXPORTS_H
#define _OE_SYSCALL_INTERNAL_EXPORTS_H

#include <openenclave/bits/defs.h>
#include <openenclave/bits/result.h>
#include <openenclave/bits/types.h>
#include <openenclave/edger8r/enclave.h>

/* The syscall layer depends on these non-exported OE symbols. */

OE_EXTERNC_BEGIN

typedef struct _oe_table_id
{
    uint64_t d1;
    uint64_t d2;
} oe_table_id_t;

oe_result_t oe_call_host_function_by_table_id(
    const oe_table_id_t* table_id,
    size_t function_id,
    const void* input_buffer,
    size_t input_buffer_size,
    void* output_buffer,
    size_t output_buffer_size,
    size_t* output_bytes_written,
    bool switchless);

oe_result_t oe_register_ecall_function_table(
    const oe_table_id_t* table_id,
    const oe_ecall_func_t* ecalls,
    size_t num_ecalls);

typedef oe_result_t (*oe_syscall_hook_t)(
    long number,
    long arg1,
    long arg2,
    long arg3,
    long arg4,
    long arg5,
    long arg6,
    long* ret);

void oe_register_syscall_hook(oe_syscall_hook_t hook);

#if 0
int memcmp(const void* vl, const void* vr, size_t n);

void* memcpy(void* dest, const void* src, size_t n);

void* memmove(void* dest, const void* src, size_t n);

void* memset(void* dest, int c, size_t n);

int oe_strcmp(const char* s1, const char* s2);

int oe_strncmp(const char* s1, const char* s2, size_t n);

size_t oe_strlen(const char* s);

char* oe_strdup(const char* s);
#endif

size_t oe_strlcpy(char* dest, const char* src, size_t size);

size_t oe_strlcat(char* dest, const char* src, size_t size);

char* oe_strtok_r(char* str, const char* delim, char** saveptr);

void* oe_malloc(size_t size);

void oe_free(void* ptr);

void* oe_calloc(size_t nmemb, size_t size);

void* oe_realloc(void* ptr, size_t size);

void oe_abort(void);

int oe_atexit(void (*function)(void));

int oe_host_printf(const char* fmt, ...);

extern int* __oe_errno_location(void);

int oe_sleep_msec(uint64_t milliseconds);

OE_EXTERNC_END

#endif /* _OE_SYSCALL_INTERNAL_EXPORTS_H */
