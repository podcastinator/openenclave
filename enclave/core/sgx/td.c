// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#include "td.h"
#include <openenclave/bits/safecrt.h>
#include <openenclave/corelibc/string.h>
#include <openenclave/enclave.h>
#include <openenclave/internal/calls.h>
#include <openenclave/internal/fault.h>
#include <openenclave/internal/globals.h>
#include <openenclave/internal/rdrand.h>
#include <openenclave/internal/sgxtypes.h>
#include <openenclave/internal/utils.h>
#include "asmdefs.h"
#include "thread.h"

#if __linux__
#include "linux/threadlocal.h"
#endif

#define TD_FROM_TCS (5 * OE_PAGE_SIZE)

OE_STATIC_ASSERT(OE_OFFSETOF(td_t, self_addr) == td_self_addr);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, unused_dtv) == td_unused_dtv);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, reserved1) == td_reserved1);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, reserved2) == td_reserved2);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, unused_sysinfo) == td_unused_sysinfo);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, stack_guard) == td_stack_guard);
OE_STATIC_ASSERT(
    OE_OFFSETOF(td_t, unused_pointer_guard) == td_unused_pointer_guard);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, magic) == td_magic);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, last_sp) == td_last_sp);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, ssa_frame_size) == td_ssa_frame_size);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, next) == td_next);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, cxx_thread_info) == td_cxx_thread_info);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, exception_code) == td_exception_code);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, exception_flags) == td_exception_flags);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, exception_address) == td_exception_address);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, depth) == td_depth);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, host_rcx) == td_host_rcx);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, host_rsp) == td_host_rsp);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, host_rbp) == td_host_rbp);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, host_previous_rsp) == td_host_previous_rsp);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, host_previous_rbp) == td_host_previous_rbp);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, oret_func) == td_oret_func);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, oret_result) == td_oret_result);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, padding) == td_padding);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, oret_arg) == td_oret_arg);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, callsites) == td_callsites);
OE_STATIC_ASSERT(OE_OFFSETOF(td_t, simulate) == td_simulate);
OE_STATIC_ASSERT(
    OE_OFFSETOF(td_t, thread_specific_data) == td_thread_specific_data);

// Static asserts for consistency with
// debugger/pythonExtension/gdb_sgx_plugin.py
#if defined(__linux__)
OE_STATIC_ASSERT(td_callsites == 0x1a0);
OE_STATIC_ASSERT(OE_OFFSETOF(Callsite, ocall_context) == 0x40);
OE_STATIC_ASSERT(TD_FROM_TCS == 0x5000);
OE_STATIC_ASSERT(sizeof(oe_ocall_context_t) == (2 * sizeof(uintptr_t)));
#endif

/*
**==============================================================================
**
** oe_get_thread_data()
**
**     Gets a pointer to the thread data structure from the GS segment.
**     The td_t data structure is a concatenation of the oe_thread_data_t with
**     extended fields, and this method returns the td->base offset with as the
**     appropriate type.
**
**==============================================================================
*/

oe_thread_data_t* oe_get_thread_data()
{
    return oe_get_td();
}

/*
**==============================================================================
**
** td_push_callsite()
**
**     Insert the Callsite structure for the current ECALL at the
**     front of the td_t.callsites list.
**
**==============================================================================
*/

void td_push_callsite(td_t* td, Callsite* callsite)
{
    callsite->next = td->callsites;
    td->callsites = callsite;
    td->depth++;
}

/*
**==============================================================================
**
**     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**     According to the implementation of Windows debugger and the previous
**     design of this structure, the debugger need the GS segment register
**     to find td_t. Since td_t is moved to current FS page, now GS segment
**     register needs to point to this page. Do not change the GS segment
**     resigter until it is solved on Windows debugger.
**     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**
** td_from_tcs()
**
**     This function calculates the address of the td_t (thread data structure)
**     relative to the TCS (Thread Control Structure) page. The td_t resides in
**     a page pointed to by the FS (segment register). This page occurs 5 pages
**     after the TCS page. The layout is as follows:
**
**         +----------------------------+
**         | TCS Page                   |
**         +----------------------------+
**         | SSA (State Save Area) 0    |
**         +----------------------------+
**         | SSA (State Save Area) 1    |
**         +----------------------------+
**         | Guard Page                 |
**         +----------------------------+
**         | Thread local storage       |
**         +----------------------------+
**         | FS/GS Page (td_t + tsp)    |
**         +----------------------------+
**
**     This layout is determined by the enclave builder. See:
**
**         ../host/sgx/create.c (_add_control_pages)
**
**     The FS segment register is set by the EENTER instruction and the td_t
**     page is zero filled upon initial enclave entry. Software sets the
**     contents of the td_t when it first determines that td_t.self_addr is
**     zero.
**
**==============================================================================
*/

td_t* td_from_tcs(void* tcs)
{
    return (td_t*)((uint8_t*)tcs + TD_FROM_TCS);
}

/*
**==============================================================================
**
** td_to_tcs()
**
**     Compute a TCS pointer from a td_t.
**
**==============================================================================
*/

void* td_to_tcs(const td_t* td)
{
    return (uint8_t*)td - TD_FROM_TCS;
}

/*
**==============================================================================
**
** oe_get_td()
**
**     Returns a pointer to the thread data structure for the current thread.
**     This structure resides in the GS segment. Offset zero of this segment
**     contains the oe_thread_data_t.self_addr field (a back pointer to the
**     structure itself). This field is zero until the structure is initialized
**     by __oe_handle_main (which happens immediately an EENTER).
**
**==============================================================================
*/

td_t* oe_get_td()
{
    td_t* td;

    asm("mov %%fs:0, %0" : "=r"(td));

    return td;
}

/*
**==============================================================================
**
** td_initialized()
**
**     Returns TRUE if this thread data structure (td_t) is initialized. An
**     initialized td_t meets the following conditions:
**
**         (1) td is not null
**         (2) td->self_addr == td
**         (3) td->magic == TD_MAGIC
**
**==============================================================================
*/

bool td_initialized(td_t* td)
{
    if (td && td->magic == TD_MAGIC && td->self_addr == (uint64_t)td)
        return true;

    return false;
}
