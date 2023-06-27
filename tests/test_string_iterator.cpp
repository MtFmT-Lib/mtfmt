// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_string_iterator.cpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串迭代器
 * @version 1.0
 * @date    2023-06-03
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "helper.h"
#include "main.h"
#include "mtfmt.hpp"
#include "unity.h"
#include <stddef.h>
#include <stdio.h>

template <std::size_t N>
constexpr mtfmt::unicode_t unicode_char(const char (&u8char)[N])
{
    return mtfmt::string::unicode_char(u8char);
}

extern "C" void string_index(void)
{
    // ascii
    mtfmt::string str_ascii = u8"str";
    ASSERT_EQUAL_VALUE(str_ascii[0], unicode_char(u8"s"));
    ASSERT_EQUAL_VALUE(str_ascii[1], unicode_char(u8"t"));
    ASSERT_EQUAL_VALUE(str_ascii[2], unicode_char(u8"r"));
    // unicode
#if _MSTR_USE_UTF_8
    mtfmt::string str_uni = u8"😊😀汉字";
    ASSERT_EQUAL_VALUE(str_uni[0], unicode_char(u8"😊"));
    ASSERT_EQUAL_VALUE(str_uni[1], unicode_char(u8"😀"));
    ASSERT_EQUAL_VALUE(str_uni[2], unicode_char(u8"汉"));
    ASSERT_EQUAL_VALUE(str_uni[3], unicode_char(u8"字"));
#endif // _MSTR_USE_UTF_8
}

extern "C" void string_const_iterator(void)
{
    // ascii
    mtfmt::string str_ascii = u8"iterator";
    mtfmt::string str_ascii_output;
    for (auto ch : str_ascii) {
        str_ascii_output.push(ch);
    }
    ASSERT_EQUAL_VALUE(str_ascii, str_ascii_output);
    // unicode
#if _MSTR_USE_UTF_8
    mtfmt::string str_uni = u8"😊😀汉字";
    mtfmt::string str_uni_output;
    for (auto ch : str_uni) {
        str_uni_output.push(ch);
    }
    ASSERT_EQUAL_VALUE(str_uni, str_uni_output);
#endif // _MSTR_USE_UTF_8
}

extern "C" void string_reverse_const_iterator(void)
{
    // ascii
    mtfmt::string str_ascii = u8"Example";
    mtfmt::string str_ascii_output;
    auto it_ascii = str_ascii.rbegin();
    auto it_ascii_end = str_ascii.rend();
    for (; it_ascii != it_ascii_end; ++it_ascii) {
        str_ascii_output.push(*it_ascii);
    }
    str_ascii_output.reverse();
    ASSERT_EQUAL_VALUE(str_ascii, str_ascii_output);
    // unicode
#if _MSTR_USE_UTF_8
    mtfmt::string str_uni = u8"😊😀汉字";
    mtfmt::string str_uni_output;
    auto it = str_uni.rbegin();
    auto it_end = str_uni.rend();
    for (; it != it_end; ++it) {
        str_uni_output.push(*it);
    }
    str_uni_output.reverse();
    ASSERT_EQUAL_VALUE(str_uni, str_uni_output);
#endif // _MSTR_USE_UTF_8
}
