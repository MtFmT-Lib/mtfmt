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
#include "helper.h"
#include "main.h"
#include "mtfmt.hpp"
#include "unity.h"
#include <stddef.h>
#include <stdio.h>

template <std::size_t N>
constexpr mtfmt::unicode_t unicode_char(const char (&u8char)[N])
{
    return mtfmt::string::unicode_char(u8char);
}

extern "C" void string_replace(void)
{
}
