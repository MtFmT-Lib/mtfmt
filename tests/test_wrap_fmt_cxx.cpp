// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_wrap_fmt_cxx.cpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   fmt (cpp包装)
 * @version 1.0
 * @date    2023-06-05
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
#include <tuple>

extern "C" void cpp_wrap_fmt(void)
{
    mtfmt::string str = mtfmt::string::format_variable("{0:i32}", 123)
                            .or_value("error");
    ASSERT_EQUAL_VALUE(str, "123");
}

extern "C" void cpp_wrap_fmt_parser(void)
{
}
