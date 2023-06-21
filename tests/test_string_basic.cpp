// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_string_basic.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   基础的字符串功能测试(长度等等)
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

extern "C" void string_copy_create(void)
{
}

extern "C" void string_move_create(void)
{
}

extern "C" void string_length(void)
{
    // ASCII
    MString str1;
    EVAL(mstr_create(&str1, u8"ASCII"));
    ASSERT_EQUAL_VALUE(str1.count, 5);
    ASSERT_EQUAL_VALUE(str1.length, 5);
    mstr_free(&str1);
    // UTF-8
    MString str2;
    EVAL(mstr_create(&str2, u8"😀😀😀"));
    ASSERT_EQUAL_VALUE(str2.count, 12);
    ASSERT_EQUAL_VALUE(str2.length, 3);
    mstr_free(&str2);
}

extern "C" void string_char_at(void)
{
    MString str_ch;
    mstr_codepoint_t ch = 0x1f600;
    EVAL(mstr_create(&str_ch, u8"😀"));
    ASSERT_EQUAL_VALUE(mstr_char_at(&str_ch, 0), ch);
    // 测试内容
    MString str;
    EVAL(mstr_create(&str, u8"S😀s"));
    ASSERT_EQUAL_VALUE(mstr_char_at(&str, 0), 'S');
    ASSERT_EQUAL_VALUE(mstr_char_at(&str, 1), ch);
    ASSERT_EQUAL_VALUE(mstr_char_at(&str, 2), 's');
    // free
    mstr_free(&str_ch);
    mstr_free(&str_ch);
}

extern "C" void string_equal(void)
{
    MString src;
    EVAL(mstr_create(&src, "Example"));
    // equal
    ASSERT_EQUAL_STRING(&src, "Example");
    // neq
    ASSERT_NOT_EQUAL_STRING(&src, "Exampl");
    ASSERT_NOT_EQUAL_STRING(&src, "Example1");
    ASSERT_NOT_EQUAL_STRING(&src, "ExAmple");
    mstr_free(&src);
}
