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
}

extern "C" void string_find_large(void)
{
}

extern "C" void string_find_or_error(void)
{
}

extern "C" void string_retain(void)
{
}

extern "C" void string_contain(void)
{
}
