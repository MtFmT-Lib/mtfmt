// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_string_concat.cpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串拼接
 * @version 1.0
 * @date    2023-06-21
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "helper.h"
#include "main.h"
#include "mm_heap.h"
#include "mm_string.h"
#include "unity.h"
#include <stdio.h>

extern "C" void string_concat_object(void)
{
    MString str_rhs;
    EVAL(mstr_create(&str_rhs, u8"😀"));
    // append到一个现有的字符串
    MString str_lhs;
    EVAL(mstr_create(&str_lhs, u8"Smile:"));
    ASSERT_EQUAL_VALUE(str_lhs.count, 6);
    ASSERT_EQUAL_VALUE(str_lhs.length, 6);
    EVAL(mstr_concat(&str_lhs, &str_rhs));
    ASSERT_EQUAL_VALUE(str_lhs.count, 10);
    ASSERT_EQUAL_VALUE(str_lhs.length, 7);
    ASSERT_EQUAL_STRING(&str_lhs, u8"Smile:😀");
    mstr_free(&str_lhs);
    mstr_free(&str_rhs);
}

extern "C" void string_concat_c_str(void)
{
    MString str_lhs;
    EVAL(mstr_create(&str_lhs, u8"Smile:"));
    ASSERT_EQUAL_VALUE(str_lhs.count, 6);
    ASSERT_EQUAL_VALUE(str_lhs.length, 6);
    EVAL(mstr_concat_cstr(&str_lhs, u8"😀"));
    ASSERT_EQUAL_VALUE(str_lhs.count, 10);
    ASSERT_EQUAL_VALUE(str_lhs.length, 7);
    ASSERT_EQUAL_STRING(&str_lhs, u8"Smile:😀");
    mstr_free(&str_lhs);
}

extern "C" void string_concat_c_slice(void)
{
    MString str_lhs;
    EVAL(mstr_create(&str_lhs, u8"Smile:"));
    ASSERT_EQUAL_VALUE(str_lhs.count, 6);
    ASSERT_EQUAL_VALUE(str_lhs.length, 6);
    const char c_str[] = {u8"😀😀😀"};
    // 拼接完整的字符串
    EVAL(mstr_concat_cstr_slice(&str_lhs, c_str + 0, c_str + 4));
    ASSERT_EQUAL_VALUE(str_lhs.count, 10);
    ASSERT_EQUAL_VALUE(str_lhs.length, 7);
    ASSERT_EQUAL_STRING(&str_lhs, u8"Smile:😀");
    // 拼接不完整的字符串
    mstr_result_t res;
    res = mstr_concat_cstr_slice(&str_lhs, c_str + 0, c_str + 3);
    ASSERT_EQUAL_VALUE(res, MStr_Err_UnicodeEncodingNotCompleted);
    mstr_free(&str_lhs);
}
