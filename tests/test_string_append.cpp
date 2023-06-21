// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_string_append.cpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串追加字符
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

//
// 从这里往下都用C++编写C测试的考虑在于字符串是UTF-8编码的
// C11支持u8"", 但是mtfmt最低要求是C99
// 因此使用C++ (mtfmt的C++ wrapper至少要求C++11)
//

extern "C" void string_append(void)
{
    MString str_ch;
    EVAL(mstr_create(&str_ch, u8"😀"));
    mstr_codepoint_t ch = mstr_char_at(&str_ch, 0);
    // 测试内容
    MString str;
    EVAL(mstr_create(&str, u8"Smile"));
    ASSERT_EQUAL_VALUE(str.count, 5);
    ASSERT_EQUAL_VALUE(str.length, 5);
    EVAL(mstr_append(&str, ':'));
    EVAL(mstr_create(&str, u8"Smile:"));
    ASSERT_EQUAL_VALUE(str.count, 6);
    ASSERT_EQUAL_VALUE(str.length, 6);
    EVAL(mstr_append(&str, ch));
    ASSERT_EQUAL_VALUE(str.count, 10);
    ASSERT_EQUAL_VALUE(str.length, 7);
    ASSERT_EQUAL_STRING(&str, u8"Smile:😀");
    // free
    mstr_free(&str);
    mstr_free(&str_ch);
}

extern "C" void string_repeat_append(void)
{
    MString str_ch;
    EVAL(mstr_create(&str_ch, u8"😀"));
    mstr_codepoint_t ch = mstr_char_at(&str_ch, 0);
    // 测试内容
    MString str;
    EVAL(mstr_create(&str, u8"Smile"));
    ASSERT_EQUAL_VALUE(str.count, 5);
    ASSERT_EQUAL_VALUE(str.length, 5);
    EVAL(mstr_repeat_append(&str, ':', 3));
    EVAL(mstr_create(&str, u8"Smile:::"));
    ASSERT_EQUAL_VALUE(str.count, 8);
    ASSERT_EQUAL_VALUE(str.length, 8);
    EVAL(mstr_repeat_append(&str, ch, 3));
    ASSERT_EQUAL_VALUE(str.count, 20);
    ASSERT_EQUAL_VALUE(str.length, 11);
    ASSERT_EQUAL_STRING(&str, u8"Smile:::😀😀😀");
    // free
    mstr_free(&str);
    mstr_free(&str_ch);
}
