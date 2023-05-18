// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    wrapper.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   给online demo准备的wrapper
 * @version 1.0
 * @date    2023-05-17
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "mm_cfg.h"
#include "mm_heap.h"
#include "mm_string.h"
#include <emscripten.h>
#include <stdint.h>
#include <stdlib.h>

MSTR_EXPORT_API(uint32_t) mstr_wasm_version()
{
    return 0x01000000;
}

MSTR_EXPORT_API(size_t) mstr_wasm_string_len(intptr_t str)
{
    const MString* s = (const MString*)str;
    return s->length;
}

MSTR_EXPORT_API(intptr_t) mstr_wasm_new_string()
{
    return (intptr_t)mstr_heap_alloc(sizeof(MString));
}

MSTR_EXPORT_API(void) mstr_wasm_free_string(intptr_t str)
{
    void* pstr = (void*)str;
    mstr_heap_free(pstr);
}

MSTR_EXPORT_API(intptr_t) mstr_wasm_allocate(size_t size)
{
    return (intptr_t)mstr_heap_alloc(size);
}

MSTR_EXPORT_API(void) mstr_wasm_free(intptr_t str)
{
    void* pstr = (void*)str;
    mstr_heap_free(pstr);
}
