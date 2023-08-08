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
#include "mtfmt.hpp"
#include "test_helper.h"
#include "test_main.h"
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
#if _MSTR_USE_UTF_8
    // UTF-8
    mtfmt::string str2 = u8"😀😀😀";
    ASSERT_EQUAL_VALUE(str2.length(), 3);
    ASSERT_EQUAL_VALUE(str2.byte_count(), 12);
#endif // _MSTR_USE_UTF_8
}

extern "C" void string_char_at(void)
{
    // @mstr_char_at
    mtfmt::string str_ascii = u8"MtF";
    ASSERT_EQUAL_VALUE(str_ascii[0], 'M');
    ASSERT_EQUAL_VALUE(str_ascii[1], 't');
    ASSERT_EQUAL_VALUE(str_ascii[2], 'F');
    // unicode
#if _MSTR_USE_UTF_8
    mtfmt::string str_unicode = u8"S😀s";
    ASSERT_EQUAL_VALUE(str_unicode[0], 'S');
    ASSERT_EQUAL_VALUE(str_unicode[1], unicode_char(u8"😀"));
    ASSERT_EQUAL_VALUE(str_unicode[2], 's');
#endif // _MSTR_USE_UTF_8
}

extern "C" void string_insert(void)
{
    // @mstr_insert
    // ascii
    mtfmt::string str_ascii = u8"abc";
    // insert中间位置
    mtfmt::string str_ascii_test_1 = str_ascii;
    str_ascii_test_1.insert(1, unicode_char(u8"$"));
    ASSERT_EQUAL_VALUE(str_ascii_test_1, u8"a$bc");
    // insert起始位置
    mtfmt::string str_ascii_test_2 = str_ascii;
    str_ascii_test_2.insert(0, unicode_char(u8"$"));
    ASSERT_EQUAL_VALUE(str_ascii_test_2, u8"$abc");
    // insert末尾位置
    mtfmt::string str_ascii_test_3 = str_ascii;
    str_ascii_test_3.insert(2, unicode_char(u8"$"));
    ASSERT_EQUAL_VALUE(str_ascii_test_3, u8"ab$c");
    // insert结束位置
    mtfmt::string str_ascii_test_4 = str_ascii;
    str_ascii_test_4.insert(3, unicode_char(u8"$"));
    ASSERT_EQUAL_VALUE(str_ascii_test_4, u8"abc$");
    // unicode
#if _MSTR_USE_UTF_8
    mtfmt::string str_unicode = u8"A😀C";
    // insert中间位置
    mtfmt::string str_uni_test_1 = str_unicode;
    str_uni_test_1.insert(1, unicode_char(u8"😊"));
    ASSERT_EQUAL_VALUE(str_uni_test_1, u8"A😊😀C");
    // insert起始位置
    mtfmt::string str_uni_test_2 = str_unicode;
    str_uni_test_2.insert(0, unicode_char(u8"😊"));
    ASSERT_EQUAL_VALUE(str_uni_test_2, u8"😊A😀C");
    // insert末尾位置
    mtfmt::string str_uni_test_3 = str_unicode;
    str_uni_test_3.insert(2, unicode_char(u8"😊"));
    ASSERT_EQUAL_VALUE(str_uni_test_3, u8"A😀😊C");
    // insert结束位置
    mtfmt::string str_uni_test_4 = str_unicode;
    str_uni_test_4.insert(3, unicode_char(u8"😊"));
    ASSERT_EQUAL_VALUE(str_uni_test_4, u8"A😀C😊");
    // insert触发堆分配
    mtfmt::string str_uni_large = u8"😀🍥🌈";
    str_uni_large.insert(1, unicode_char(u8"😊"));
    ASSERT_EQUAL_VALUE(str_uni_large, u8"😀😊🍥🌈");
    str_uni_large.insert(2, unicode_char(u8"😙"));
    ASSERT_EQUAL_VALUE(str_uni_large, u8"😀😊😙🍥🌈");
#endif // _MSTR_USE_UTF_8
}

extern "C" void string_remove(void)
{
    mtfmt::string str_ascii = u8"ABC";
    // remove中间位置
    mtfmt::string str_ascii_test_1 = str_ascii;
    mtfmt::unicode_t ascii_ch1 = str_ascii_test_1.remove(1).or_value(0);
    ASSERT_EQUAL_VALUE(ascii_ch1, unicode_char(u8"B"));
    ASSERT_EQUAL_VALUE(str_ascii_test_1, u8"AC");
    // remove起始位置
    mtfmt::string str_ascii_test_2 = str_ascii;
    mtfmt::unicode_t ascii_ch2 = str_ascii_test_2.remove(0).or_value(0);
    ASSERT_EQUAL_VALUE(ascii_ch2, unicode_char(u8"A"));
    ASSERT_EQUAL_VALUE(str_ascii_test_2, u8"BC");
    // remove结束位置
    mtfmt::string str_ascii_test_3 = str_ascii;
    mtfmt::unicode_t ascii_ch3 = str_ascii_test_3.remove(2).or_value(0);
    ASSERT_EQUAL_VALUE(ascii_ch3, unicode_char(u8"C"));
    ASSERT_EQUAL_VALUE(str_ascii_test_3, u8"AB");
    // unicode
#if _MSTR_USE_UTF_8
    mtfmt::string str_unicode = u8"A😀C";
    // remove中间位置
    mtfmt::string str_unicode_test_1 = str_unicode;
    mtfmt::unicode_t unicode_ch1 =
        str_unicode_test_1.remove(1).or_value(0);
    ASSERT_EQUAL_VALUE(unicode_ch1, unicode_char(u8"😀"));
    ASSERT_EQUAL_VALUE(str_unicode_test_1, u8"AC");
    // remove起始位置
    mtfmt::string str_unicode_test_2 = str_unicode;
    mtfmt::unicode_t unicode_ch2 =
        str_unicode_test_2.remove(0).or_value(0);
    ASSERT_EQUAL_VALUE(unicode_ch2, unicode_char(u8"A"));
    ASSERT_EQUAL_VALUE(str_unicode_test_2, u8"😀C");
    // remove结束位置
    mtfmt::string str_unicode_test_3 = str_unicode;
    mtfmt::unicode_t unicode_ch3 =
        str_unicode_test_3.remove(2).or_value(0);
    ASSERT_EQUAL_VALUE(unicode_ch3, unicode_char(u8"C"));
    ASSERT_EQUAL_VALUE(str_unicode_test_3, u8"A😀");
#endif // _MSTR_USE_UTF_8
}
