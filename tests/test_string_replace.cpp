// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_string_replace.cpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串替换的测试
 * @version 1.0
 * @date    2023-06-24
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "mtfmt.hpp"
#include "test_helper.h"
#include "test_main.h"
#include "unity.h"
#include <stddef.h>
#include <stdio.h>

template <std::size_t N>
constexpr mtfmt::unicode_t unicode_char(const char (&u8char)[N])
{
    return mtfmt::string::unicode_char(u8char);
}

extern "C" void string_retain_all(void)
{
    mtfmt::string str_ascii = "Exa#mmp#ml#ee";
    auto result_ascii = str_ascii.retain("#m");
    ASSERT_EQUAL_VALUE(result_ascii.is_succ(), true);
    ASSERT_EQUAL_VALUE(str_ascii, "Exampl#ee");
    ASSERT_EQUAL_VALUE(str_ascii.length(), 9);
    ASSERT_EQUAL_VALUE(str_ascii.byte_count(), 9);
#if _MSTR_USE_UTF_8
    // Unicode retain
    mtfmt::string str_unicode = u8"😀😊😔🍥😊😔😀";
    auto result_unicode = str_unicode.retain(u8"😔");
    ASSERT_EQUAL_VALUE(result_unicode.is_succ(), true);
    ASSERT_EQUAL_VALUE(str_unicode, u8"😀😊🍥😊😀");
    ASSERT_EQUAL_VALUE(str_unicode.length(), 5);
    ASSERT_EQUAL_VALUE(str_unicode.byte_count(), 20);
#endif // _MSTR_USE_UTF_8
}

extern "C" void string_retain_endwith(void)
{
    mtfmt::string str_ascii = "StartWithEndWith";
    auto result_ascii =
        str_ascii.retain("StartWith", MStringReplaceOption_StartWith);
    ASSERT_EQUAL_VALUE(result_ascii.is_succ(), true);
    ASSERT_EQUAL_VALUE(str_ascii, "EndWith");
    ASSERT_EQUAL_VALUE(str_ascii.length(), 7);
    ASSERT_EQUAL_VALUE(str_ascii.byte_count(), 7);
#if _MSTR_USE_UTF_8
    mtfmt::string str_unicode = u8"😔🍥😀";
    auto result_unicode =
        str_unicode.retain(u8"😔", MStringReplaceOption_StartWith);
    ASSERT_EQUAL_VALUE(result_unicode.is_succ(), true);
    ASSERT_EQUAL_VALUE(str_unicode, u8"🍥😀");
    ASSERT_EQUAL_VALUE(str_unicode.length(), 2);
    ASSERT_EQUAL_VALUE(str_unicode.byte_count(), 8);
#endif // _MSTR_USE_UTF_8
}

extern "C" void string_retain_startwith(void)
{
    mtfmt::string str_ascii = "StartWithEndWith";
    auto result_ascii =
        str_ascii.retain("EndWith", MStringReplaceOption_EndWith);
    ASSERT_EQUAL_VALUE(result_ascii.is_succ(), true);
    ASSERT_EQUAL_VALUE(str_ascii, "StartWith");
    ASSERT_EQUAL_VALUE(str_ascii.length(), 9);
    ASSERT_EQUAL_VALUE(str_ascii.byte_count(), 9);
#if _MSTR_USE_UTF_8
    mtfmt::string str_unicode = u8"🍥😀😔";
    auto result_unicode =
        str_unicode.retain(u8"😔", MStringReplaceOption_EndWith);
    ASSERT_EQUAL_VALUE(result_unicode.is_succ(), true);
    ASSERT_EQUAL_VALUE(str_unicode, u8"🍥😀");
    ASSERT_EQUAL_VALUE(str_unicode.length(), 2);
    ASSERT_EQUAL_VALUE(str_unicode.byte_count(), 8);
#endif // _MSTR_USE_UTF_8
}
