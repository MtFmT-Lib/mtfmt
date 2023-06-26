// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_match.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串查找操作
 * @version 1.0
 * @date    2023-06-22
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */

#define MSTR_IMP_SOURCES 1

#include "mm_heap.h"
#include "mm_string.h"
#include <stddef.h>
#include <string.h>

MSTR_EXPORT_API(mstr_result_t)
mstr_find(
    const MString* str,
    MStringMatchResult* res,
    const char* pattern,
    usize_t pattern_len
)
{
    return MStr_Err_NoImplemention;
}
