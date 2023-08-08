// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_string_equal.cpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串相等, 包括equal, start_with, end_with
 * @version 1.0
 * @date    2023-06-23
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "mtfmt.hpp"
#include "test_helper.h"
#include "test_main.h"
#include "unity.h"
#include <stdio.h>

extern "C" void string_equal(void)
{
    mtfmt::string str1 = "String";
    mtfmt::string str2 = str1;
    // @mstr_equal
    TEST_ASSERT_TRUE(str1 == "String");
    TEST_ASSERT_TRUE(str2 == "String");
    TEST_ASSERT_TRUE("String" == str1);
    TEST_ASSERT_TRUE("String" == str2);
    TEST_ASSERT_TRUE(str1 == str2);
    TEST_ASSERT_TRUE(str2 == str1);
}

extern "C" void string_not_equal(void)
{
    mtfmt::string str1 = "String1";
    mtfmt::string str2 = "String2";
    // @mstr_equal
    TEST_ASSERT_TRUE(str2 != str1);
    TEST_ASSERT_TRUE(str1 != str2);
    TEST_ASSERT_TRUE(str1 != "String2");
    TEST_ASSERT_TRUE(str2 != "String1");
    TEST_ASSERT_TRUE("String2" != str1);
    TEST_ASSERT_TRUE("String1" != str2);
}

extern "C" void string_end_with(void)
{
    mtfmt::string src = "Example";
    // @mstr_end_with
    ASSERT_EQUAL_VALUE(src, "Example");
    ASSERT_EQUAL_VALUE(src.end_with("ple"), True);
    ASSERT_EQUAL_VALUE(src.end_with("Example"), True);
    ASSERT_EQUAL_VALUE(src.end_with("Exa"), False);
    ASSERT_EQUAL_VALUE(src.end_with("ple!"), False);
    ASSERT_EQUAL_VALUE(src.end_with("Example!"), False);
}

extern "C" void string_start_with(void)
{
    mtfmt::string src = "Example";
    // @mstr_start_with
    ASSERT_EQUAL_VALUE(src, "Example");
    ASSERT_EQUAL_VALUE(src.start_with("Exa"), True);
    ASSERT_EQUAL_VALUE(src.start_with("Example"), True);
    ASSERT_EQUAL_VALUE(src.start_with("xample"), False);
    ASSERT_EQUAL_VALUE(src.start_with("Example!"), False);
}
