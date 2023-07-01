// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_replace.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串替换操作
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
mstr_replace_multi(
    MString* str, const MStringReplaceTarget* target, usize_t target_cnt
)
{
    return MStr_Err_NoImplemention;
}

MSTR_EXPORT_API(void)
mstr_replace_set_target(
    MStringReplaceTarget* rep,
    MStringReplaceOption opt,
    const char* pattern,
    const char* target
)
{
    rep->opt = opt;
    rep->substr = pattern;
    rep->substr_len = strlen(pattern);
    rep->replace_to = target;
    rep->replace_to_len = strlen(target);
}

MSTR_EXPORT_API(mstr_result_t)
mstr_replace(MString* str, const char* pattern, const char* replace_to)
{
    return MStr_Err_NoImplemention;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_retain(
    MString* str,
    MStringReplaceOption opt,
    const char* pattern,
    usize_t pattern_cnt
)
{
    return MStr_Err_NoImplemention;
}
