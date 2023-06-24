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
    mtfmt::string str = u8"S😀s";
    // @mstr_char_at
    ASSERT_EQUAL_VALUE(str[0], 'S');
    ASSERT_EQUAL_VALUE(str[1], unicode_char(u8"😀"));
    ASSERT_EQUAL_VALUE(str[2], 's');
}

extern "C" void string_insert(void)
{
    mtfmt::string str = u8"A😀C";
    // insert中间位置
    mtfmt::string str_test_1 = str;
    str_test_1.insert(1, unicode_char(u8"😊"));
    ASSERT_EQUAL_VALUE(str_test_1, u8"A😊😀C");
    // insert起始位置
    mtfmt::string str_test_2 = str;
    str_test_2.insert(0, unicode_char(u8"😊"));
    ASSERT_EQUAL_VALUE(str_test_2, u8"😊A😀C");
    // insert末尾位置
    mtfmt::string str_test_3 = str;
    str_test_3.insert(2, unicode_char(u8"😊"));
    ASSERT_EQUAL_VALUE(str_test_3, u8"A😀😊C");
    // insert结束位置
    mtfmt::string str_test_4 = str;
    str_test_4.insert(3, unicode_char(u8"😊"));
    ASSERT_EQUAL_VALUE(str_test_4, u8"A😀C😊");
    // insert触发堆分配
    mtfmt::string str_large = u8"😀🍥🌈";
    str_large.insert(1, unicode_char(u8"😊"));
    ASSERT_EQUAL_VALUE(str_large, u8"😀😊🍥🌈");
    str_large.insert(2, unicode_char(u8"😙"));
    ASSERT_EQUAL_VALUE(str_large, u8"😀😊😙🍥🌈");
}

extern "C" void string_remove(void)
{
    mtfmt::string str = u8"A😀C";
    // remove中间位置
    mtfmt::string str_test_1 = str;
    mtfmt::unicode_t ch1 = str_test_1.remove(1).or_value(0);
    ASSERT_EQUAL_VALUE(ch1, unicode_char(u8"😀"));
    ASSERT_EQUAL_VALUE(str_test_1, u8"AC");
    // remove起始位置
    mtfmt::string str_test_2 = str;
    mtfmt::unicode_t ch2 = str_test_2.remove(0).or_value(0);
    ASSERT_EQUAL_VALUE(ch2, unicode_char(u8"A"));
    ASSERT_EQUAL_VALUE(str_test_2, u8"😀C");
    // remove结束位置
    mtfmt::string str_test_3 = str;
    mtfmt::unicode_t ch3 = str_test_3.remove(2).or_value(0);
    ASSERT_EQUAL_VALUE(ch3, unicode_char(u8"C"));
    ASSERT_EQUAL_VALUE(str_test_3, u8"A😀");
}
