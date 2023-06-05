// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_monad_cxx.cpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   Monadic result
 * @version 1.0
 * @date    2023-06-05
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "main.h"
#include "mtfmt.hpp"
#include "unity.h"
#include <exception>
#include <stddef.h>
#include <stdio.h>

struct SuccType
{
    int value;
};

struct ErrorType
{
    int value;
};

class TestException : public std::exception
{
public:
    TestException() : std::exception()
    {
    }
};

extern "C" void cpp_monadic_result_or_value(void)
{
    // succ
    mtfmt::result<SuccType, ErrorType> val1 = SuccType{1};
    SuccType r1 = val1.or_value(SuccType{2});
    TEST_ASSERT_TRUE(r1.value == 1);
    // error
    mtfmt::result<SuccType, ErrorType> val2 = ErrorType{1};
    SuccType r2 = val2.or_value(SuccType{2});
    TEST_ASSERT_TRUE(r2.value == 2);
}

extern "C" void cpp_monadic_result_or_exception(void)
{
    // succ
    bool cond1 = false;
    SuccType r1 = SuccType{-1};
    mtfmt::result<SuccType, ErrorType> val1 = SuccType{1};
    try {
        r1 = val1.or_exception([](ErrorType) {
            // exception
            return TestException();
        });
    }
    catch (const TestException&) {
        cond1 = true;
    }
    TEST_ASSERT_FALSE(cond1);
    TEST_ASSERT_TRUE(r1.value == 1);
    // error
    bool cond2 = false;
    SuccType r2 = SuccType{-1};
    mtfmt::result<SuccType, ErrorType> val2 = ErrorType{1};
    try {
        r2 = val2.or_exception([](ErrorType) {
            // exception
            return TestException();
        });
    }
    catch (const TestException&) {
        cond2 = true;
    }
    TEST_ASSERT_TRUE(cond2);
    TEST_ASSERT_TRUE(r2.value == -1);
}
