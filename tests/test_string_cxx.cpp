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
}
