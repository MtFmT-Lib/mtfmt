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
    mtfmt::string str = u8"😊😀汉字";
    ASSERT_EQUAL_VALUE(str[0], unicode_char(u8"😊"));
    ASSERT_EQUAL_VALUE(str[1], unicode_char(u8"😀"));
    ASSERT_EQUAL_VALUE(str[2], unicode_char(u8"汉"));
    ASSERT_EQUAL_VALUE(str[3], unicode_char(u8"字"));
}

extern "C" void string_const_iterator(void)
{
    mtfmt::string str = u8"😊😀汉字";
    mtfmt::string str_output;
    for (auto ch : str) {
        str_output.push(ch);
    }
    ASSERT_EQUAL_VALUE(str, str_output);
}

extern "C" void string_reverse_const_iterator(void)
{
    mtfmt::string str = u8"😊😀汉字";
    mtfmt::string str_output;
    auto it = str.rbegin();
    auto it_end = str.rend();
    for (; it != it_end; ++it) {
        str_output.push(*it);
    }
    str_output.reverse();
    ASSERT_EQUAL_VALUE(str, str_output);
}
