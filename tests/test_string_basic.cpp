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
#include "mtfmt.hpp"
#include "unity.h"
#include <stdio.h>

template <std::size_t N>
constexpr mtfmt::unicode_t unicode_char(const char (&u8char)[N])
{
    return mtfmt::string::unicode_char(u8char);
}

extern "C" void string_copy_create(void)
{
}

extern "C" void string_move_create(void)
{
}

extern "C" void string_length(void)
{
    // ASCII
    mtfmt::string str1 = u8"ASCII";
    ASSERT_EQUAL_VALUE(str1.length(), 5);
    ASSERT_EQUAL_VALUE(str1.byte_count(), 5);
    // UTF-8
    mtfmt::string str2 = u8"😀😀😀";
    ASSERT_EQUAL_VALUE(str2.length(), 3);
    ASSERT_EQUAL_VALUE(str2.byte_count(), 12);
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

extern "C" void string_insert(void)
{
}

extern "C" void string_remove(void)
{
}
