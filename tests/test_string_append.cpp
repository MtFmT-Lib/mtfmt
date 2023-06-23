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

//
// 从这里往下都用C++编写C测试的考虑在于字符串是UTF-8编码的
// C11支持u8"", 但是mtfmt最低要求是C99
// 因此使用C++ (mtfmt的C++ wrapper至少要求C++11)
//

template <std::size_t N>
constexpr mtfmt::unicode_t unicode_char(const char (&u8char)[N])
{
    return mtfmt::string::unicode_char(u8char);
}

extern "C" void string_append(void)
{
    constexpr auto ch = unicode_char(u8"😀");
    // @mstr_append
    mtfmt::string str = u8"Smile";
    ASSERT_EQUAL_VALUE(str.length(), 5);
    ASSERT_EQUAL_VALUE(str.byte_count(), 5);
    str += ':';
    ASSERT_EQUAL_VALUE(str.length(), 6);
    ASSERT_EQUAL_VALUE(str.byte_count(), 6);
    ASSERT_EQUAL_VALUE(str, u8"Smile:");
    str += ch;
    ASSERT_EQUAL_VALUE(str.length(), 7);
    ASSERT_EQUAL_VALUE(str.byte_count(), 10);
    ASSERT_EQUAL_VALUE(str, u8"Smile:😀");
}

extern "C" void string_repeat_append(void)
{
    constexpr auto ch = unicode_char(u8"😀");
    // @mstr_repeat_append
    mtfmt::string str = u8"Smile";
    ASSERT_EQUAL_VALUE(str.length(), 5);
    ASSERT_EQUAL_VALUE(str.byte_count(), 5);
    str += mtfmt::string::repeat_char_t{':', 3};
    ASSERT_EQUAL_VALUE(str, u8"Smile:::");
    ASSERT_EQUAL_VALUE(str.length(), 8);
    ASSERT_EQUAL_VALUE(str.byte_count(), 8);
    str += mtfmt::string::repeat_char_t{ch, 3};
    ASSERT_EQUAL_VALUE(str.length(), 11);
    ASSERT_EQUAL_VALUE(str.byte_count(), 20);
    ASSERT_EQUAL_VALUE(str, u8"Smile:::😀😀😀");
}
