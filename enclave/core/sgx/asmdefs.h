// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#ifndef _ASMDEFS_H
#define _ASMDEFS_H

#ifndef __ASSEMBLER__
#include <openenclave/enclave.h>
#endif

#define ENCLU_EGETKEY 1
#define ENCLU_EENTER 2
#define ENCLU_EEXIT 4

#define PAGE_SIZE 4096
#define STATIC_STACK_SIZE 8 * 100
#define OE_WORD_SIZE 8

#define CODE_ERET 0x200000000

/* Use GS register if this flag is set */
#ifdef __ASSEMBLER__
#define OE_ARG_FLAG_GS 0x0001
#endif

/* Offsets into oe_thread_data_t structure */
#define td_self_addr (0)
#define td_unused_dtv (8)
#define td_reserved1 (16)
#define td_reserved2 (24)
#define td_unused_sysinfo (32)
#define td_stack_guard (40)
#define td_unused_pointer_guard (48)
#define td_magic (256)
#define td_last_sp (264)
#define td_ssa_frame_size (272)
#define td_next (280)
#define td_cxx_thread_info (288)
#define td_exception_code (336)
#define td_exception_flags (340)
#define td_exception_address (344)
#define td_depth (352)
#define td_host_rcx (360)
#define td_host_rsp (368)
#define td_host_rbp (376)
#define td_host_previous_rsp (384)
#define td_host_previous_rbp (392)
#define td_oret_func (400)
#define td_oret_result (402)
#define td_padding (404)
#define td_oret_arg (408)
#define td_callsites (416)
#define td_simulate (424)
#define td_thread_specific_data (432)

#define oe_exit_enclave __morestack
#ifndef __ASSEMBLER__
/* This function exits the enclave by initiating the ENCLU-EEXIT instruction.
 * It should not be confused with oe_exit(), which maps to the standard-C
 * exit() function defined in <openenclave/corelibc/stdlib.h>.
 */
void oe_exit_enclave(uint64_t arg1, uint64_t arg2);
#endif

#ifndef __ASSEMBLER__
void __oe_handle_main(
    uint64_t arg1,
    uint64_t arg2,
    uint64_t cssa,
    void* tcs,
    uint64_t* output_arg1,
    uint64_t* output_arg2);

void oe_exception_dispatcher(void* context);
#endif

#ifndef __ASSEMBLER__
void oe_notify_nested_exit_start(
    uint64_t arg1,
    oe_ocall_context_t* ocall_context);
#endif

#endif /* _ASMDEFS_H */
