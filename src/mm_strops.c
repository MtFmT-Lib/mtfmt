// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_strops.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   比较麻烦的字符串操作
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
    MStringSubStrResult* res,
    const char* pattern,
    usize_t pattern_size
)
{
    return MStr_Err_NoImplemention;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_retain(MString* str, const char* substr, usize_t substr_len)
{
    return MStr_Err_NoImplemention;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_replace(
    MString* str, const MStringReplaceTarget* target, usize_t target_cnt
)
{
    return MStr_Err_NoImplemention;
}

MSTR_EXPORT_API(void)
mstr_replace_set_target(
    MStringReplaceTarget* rep,
    MStringReplaceOption opt,
    const char* patt,
    const char* target
)
{
    rep->opt = opt;
    rep->substr = patt;
    rep->substr_len = strlen(patt);
    rep->replace_to = target;
    rep->replace_to_len = strlen(target);
}
