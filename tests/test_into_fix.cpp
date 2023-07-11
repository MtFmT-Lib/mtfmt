// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_into_fix.cpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   定点数格式化的测试
 * @version 1.0
 * @date    2023-06-30
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

extern "C" void test_qtoa_signed(void)
{
    // @mstr_fmt_iqtoa
    mtfmt::string str = u8"";
    str.append_from(mtfmt::fixed_value(1), 2);
    ASSERT_EQUAL_VALUE(str, u8"0.25");
    // 负数
    // sign的处理和整数是同一个部分, 理论上可以不管它啦
    mtfmt::string str_neg = u8"";
    str_neg.append_from(mtfmt::fixed_value(-1), 2);
    ASSERT_EQUAL_VALUE(str_neg, u8"-0.25");
    // 零
    mtfmt::string str_zero = u8"";
    str_zero.append_from(mtfmt::fixed_value(0), 2);
    ASSERT_EQUAL_VALUE(str_zero, u8"0");
}

extern "C" void test_qtoa_unsigned(void)
{
    // @mstr_fmt_uqtoa
    mtfmt::string str = u8"";
    str.append_from(mtfmt::fixed_value(1u), 2);
    ASSERT_EQUAL_VALUE(str, u8"0.25");
    // 零
    mtfmt::string str_zero = u8"";
    str_zero.append_from(mtfmt::fixed_value(0u), 2);
    ASSERT_EQUAL_VALUE(str_zero, u8"0");
}
