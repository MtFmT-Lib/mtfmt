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
#include "mtfmt.hpp"
#include "unity.h"
#include <stdio.h>

template <std::size_t N>
constexpr mtfmt::unicode_t unicode_char(const char (&u8char)[N])
{
    return mtfmt::string::unicode_char(u8char);
}

extern "C" void string_append(void)
{
    // @mstr_append
    mtfmt::string str = u8"Smile";
    ASSERT_EQUAL_VALUE(str.length(), 5);
    ASSERT_EQUAL_VALUE(str.byte_count(), 5);
    str += ':';
    ASSERT_EQUAL_VALUE(str.length(), 6);
    ASSERT_EQUAL_VALUE(str.byte_count(), 6);
    ASSERT_EQUAL_VALUE(str, u8"Smile:");
#if _MSTR_USE_UTF_8
    constexpr auto ch = unicode_char(u8"😀");
    str += ch;
    ASSERT_EQUAL_VALUE(str.length(), 7);
    ASSERT_EQUAL_VALUE(str.byte_count(), 10);
    ASSERT_EQUAL_VALUE(str, u8"Smile:😀");
#endif // _MSTR_USE_UTF_8
}

extern "C" void string_repeat_append(void)
{
    // @mstr_repeat_append
    mtfmt::string str = u8"Smile";
    ASSERT_EQUAL_VALUE(str.length(), 5);
    ASSERT_EQUAL_VALUE(str.byte_count(), 5);
    str += mtfmt::string::repeat_char_t{':', 3};
    ASSERT_EQUAL_VALUE(str, u8"Smile:::");
    ASSERT_EQUAL_VALUE(str.length(), 8);
    ASSERT_EQUAL_VALUE(str.byte_count(), 8);
#if _MSTR_USE_UTF_8
    constexpr auto ch = unicode_char(u8"😀");
    str += mtfmt::string::repeat_char_t{ch, 3};
    ASSERT_EQUAL_VALUE(str.length(), 11);
    ASSERT_EQUAL_VALUE(str.byte_count(), 20);
    ASSERT_EQUAL_VALUE(str, u8"Smile:::😀😀😀");
#endif // _MSTR_USE_UTF_8
}
