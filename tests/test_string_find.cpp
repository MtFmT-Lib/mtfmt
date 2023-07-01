// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_string_find.cpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串查找
 * @version 1.0
 * @date    2023-06-26
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

extern "C" void string_find(void)
{
    mtfmt::string test_str_ascii = u8"Example";
    // 这里用 -2 作为 or_value 的值是考虑到 find 的错误返回值包括 -1
    // ascii: 单个字符
    ASSERT_EQUAL_VALUE(test_str_ascii.find(u8"E").or_value(-2), 0);
    ASSERT_EQUAL_VALUE(test_str_ascii.find(u8"x").or_value(-2), 1);
    ASSERT_EQUAL_VALUE(test_str_ascii.find(u8"e").or_value(-2), 6);
    ASSERT_EQUAL_VALUE(test_str_ascii.find(u8"s").or_value(-2), -1);
    // ascii: 多个字符
    ASSERT_EQUAL_VALUE(test_str_ascii.find(u8"Ex").or_value(-2), 0);
    ASSERT_EQUAL_VALUE(test_str_ascii.find(u8"xa").or_value(-2), 1);
    ASSERT_EQUAL_VALUE(test_str_ascii.find(u8"mp").or_value(-2), 3);
    ASSERT_EQUAL_VALUE(test_str_ascii.find(u8"le").or_value(-2), 5);
    ASSERT_EQUAL_VALUE(
        test_str_ascii.find(test_str_ascii).or_value(-2), 0
    );
    ASSERT_EQUAL_VALUE(
        test_str_ascii.find(u8"ExampleF").or_value(-2), -1
    );
    // ascii: 空
    ASSERT_EQUAL_VALUE(test_str_ascii.find(u8"").or_value(-2), 0);
    // ascii: 不同起始位置
    ASSERT_EQUAL_VALUE(test_str_ascii.find(u8"mp", 0).or_value(-2), 3);
    ASSERT_EQUAL_VALUE(test_str_ascii.find(u8"mp", 1).or_value(-2), 2);
    ASSERT_EQUAL_VALUE(test_str_ascii.find(u8"mp", 3).or_value(-2), 0);
    ASSERT_EQUAL_VALUE(test_str_ascii.find(u8"mp", 4).or_value(-2), -1);
#if _MSTR_USE_UTF_8
    mtfmt::string test_str_unicode = u8"汉字😊🌈🍥English";
    // unicode: 单个字符
    ASSERT_EQUAL_VALUE(test_str_unicode.find(u8"汉").or_value(-2), 0);
    ASSERT_EQUAL_VALUE(test_str_unicode.find(u8"字").or_value(-2), 1);
    ASSERT_EQUAL_VALUE(test_str_unicode.find(u8"🍥").or_value(-2), 4);
    // unicode: 多个字符
    ASSERT_EQUAL_VALUE(test_str_unicode.find(u8"😊").or_value(-2), 2);
    ASSERT_EQUAL_VALUE(test_str_unicode.find(u8"🌈").or_value(-2), 3);
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find(u8"English").or_value(-2), 5
    );
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find(test_str_unicode).or_value(-2), 0
    );
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find(u8"汉字😊🌈🍥EnglishX").or_value(-2), -1
    );
    // unicode: 空
    ASSERT_EQUAL_VALUE(test_str_unicode.find(u8"").or_value(-2), 0);
    // unicode: 不同起始位置
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find(u8"🌈", 0).or_value(-2), 3
    );
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find(u8"🌈", 1).or_value(-2), 2
    );
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find(u8"🌈", 3).or_value(-2), 0
    );
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find(u8"🌈", 4).or_value(-2), -1
    );
#endif // _MSTR_USE_UTF_8
}

extern "C" void string_find_large(void)
{
    mtfmt::string test_str_ascii =
        u8"The mini template formatting library, or MtFmt is a "
        u8"formatter library";
    // 这里用 -2 作为 or_value 的值是考虑到 find 的错误返回值包括 -1
    // ascii: 多多的字符
    ASSERT_EQUAL_VALUE(
        test_str_ascii.find(u8"formatting library").or_value(-2), 18
    );
    ASSERT_EQUAL_VALUE(
        test_str_ascii.find(u8"formatting library", 2).or_value(-2), 16
    );
    ASSERT_EQUAL_VALUE(
        test_str_ascii.find(u8"formatting library", 18).or_value(-2), 0
    );
    ASSERT_EQUAL_VALUE(
        test_str_ascii.find(u8"formatting library", 19).or_value(-2), -1
    );
#if _MSTR_USE_UTF_8
    // unicode: 多多的字符
    mtfmt::string test_str_unicode = u8"汉字😊🌈🍥English汉字😊🌈🍥";
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find(u8"🍥English汉字😊").or_value(-2), 4
    );
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find(u8"🍥English汉字😊", 1).or_value(-2), 3
    );
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find(u8"🍥English汉字😊", 4).or_value(-2), 0
    );
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find(u8"🍥English汉字😊", 5).or_value(-2), -1
    );
#endif // _MSTR_USE_UTF_8
}

extern "C" void string_find_or_error(void)
{
    mtfmt::string test_str_ascii =
        u8"The mini template formatting library, or MtFmt is a "
        u8"formatter library";
    // ascii: 多多的字符
    ASSERT_EQUAL_VALUE(
        test_str_ascii.find_or_err(u8"formatting library")
            .or_value(0xff),
        18
    );
    ASSERT_EQUAL_VALUE(
        test_str_ascii.find_or_err(u8"formatting library", 2)
            .or_value(0xff),
        16
    );
    ASSERT_EQUAL_VALUE(
        test_str_ascii.find_or_err(u8"formatting library", 18)
            .or_value(0xff),
        0
    );
    ASSERT_EQUAL_VALUE(
        test_str_ascii.find_or_err(u8"formatting library", 19)
            .or_value(0xff),
        0xff
    );
#if _MSTR_USE_UTF_8
    mtfmt::string test_str_unicode = u8"汉字😊🌈🍥English";
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find_or_err(u8"🌈🍥").or_value(0xff), 3
    );
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find_or_err(u8"🌈🍥", 1).or_value(0xff), 2
    );
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find_or_err(u8"🌈🍥", 3).or_value(0xff), 0
    );
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find_or_err(u8"🌈🍥", 4).or_value(0xff), 0xff
    );
    ASSERT_EQUAL_VALUE(
        test_str_unicode.find_or_err(u8"🍥🍥", 0).or_value(0xff), 0xff
    );
#endif // _MSTR_USE_UTF_8
}

extern "C" void string_contain(void)
{
    mtfmt::string test_str = u8"😊🌈🍥";
    // 空
    ASSERT_EQUAL_VALUE(test_str.contains(u8""), true);
    // 本身
    ASSERT_EQUAL_VALUE(test_str.contains(test_str), true);
    // 单个和多个字符
    ASSERT_EQUAL_VALUE(test_str.contains(u8"🌈"), true);
    ASSERT_EQUAL_VALUE(test_str.contains(u8"🌈🍥"), true);
    ASSERT_EQUAL_VALUE(test_str.contains(u8"🌈🍥E"), false);
    ASSERT_EQUAL_VALUE(test_str.contains(u8"🌈😊🍥"), false);
    ASSERT_EQUAL_VALUE(test_str.contains(u8"😊🌈🍥🍥"), false);
}
