// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_fmt_array.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   数组格式化
 * @version 1.0
 * @date    2023-06-03
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "helper.h"
#include "main.h"
#include "mtfmt.h"
#include "unity.h"
#include <stddef.h>
#include <stdio.h>

// 数组
#define ARRAY_SIZE ((size_t)6)

static const int32_t array[ARRAY_SIZE] = {1, 1, 4, 5, 1, 4};

void fmt_array(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    // 1/4096 ..
    EVAL(mstr_format(&s, "@{[0:i32]}@", 2, array, ARRAY_SIZE));
    ASSERT_EQUAL_STRING(&s, "@1, 1, 4, 5, 1, 4@");
    mstr_free(&s);
}

void fmt_array_userdefind_split(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    // 1/4096 ..
    EVAL(mstr_format(&s, "@{[0:i32|:#]}@", 2, array, ARRAY_SIZE));
    ASSERT_EQUAL_STRING(&s, "@1#1#4#5#1#4@");
    mstr_free(&s);
}

void fmt_array_element_style(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    // 1/4096 ..
    EVAL(mstr_format(&s, "@{[0:i32|:#:x]}@", 2, array, ARRAY_SIZE));
    ASSERT_EQUAL_STRING(&s, "@0x1#0x1#0x4#0x5#0x1#0x4@");
    mstr_free(&s);
}
