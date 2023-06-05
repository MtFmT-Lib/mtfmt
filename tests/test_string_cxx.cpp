// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_string_cxx.cpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串(cxx)
 * @version 1.0
 * @date    2023-06-03
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "main.h"
#include "mtfmt.hpp"
#include "unity.h"
#include <stddef.h>
#include <stdio.h>

extern "C" void cpp_string_equal(void)
{
    mtfmt::string str1("String");
    mtfmt::string str2 = str1;
    TEST_ASSERT_TRUE(str1 == "String");
    TEST_ASSERT_TRUE(str2 == "String");
    TEST_ASSERT_TRUE("String" == str1);
    TEST_ASSERT_TRUE("String" == str2);
    TEST_ASSERT_TRUE(str1 == str2);
    TEST_ASSERT_TRUE(str2 == str1);
}

extern "C" void cpp_string_not_equal(void)
{
    mtfmt::string str1("String1");
    mtfmt::string str2("String2");
    TEST_ASSERT_TRUE(str2 != str1);
    TEST_ASSERT_TRUE(str1 != str2);
    TEST_ASSERT_TRUE(str1 != "String2");
    TEST_ASSERT_TRUE(str2 != "String1");
    TEST_ASSERT_TRUE("String2" != str1);
    TEST_ASSERT_TRUE("String1" != str2);
}

extern "C" void cpp_string_push(void)
{
    mtfmt::string str;
    TEST_ASSERT_TRUE(str == "");
    // 'M'
    str.push('M');
    TEST_ASSERT_TRUE(str == "M");
    // '@' * 7
    str.push('@', 7);
    TEST_ASSERT_TRUE(str == "M@@@@@@@");
}

extern "C" void cpp_string_concat(void)
{
    mtfmt::string str;
    TEST_ASSERT_TRUE(str == "");
    // 'MtF'
    str.concat("MtF");
    TEST_ASSERT_TRUE(str == "MtF");
    // 'mt''
    str += "mt";
    TEST_ASSERT_TRUE(str == "MtFmt");
}
