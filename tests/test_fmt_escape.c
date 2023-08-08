// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_fmt_escape.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   转义字符
 * @version 1.0
 * @date    2023-06-03
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "mtfmt.h"
#include "test_helper.h"
#include "test_main.h"
#include "unity.h"
#include <stddef.h>
#include <stdio.h>

void fmt_escape_bracket(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(&s, "@{{{0:i32}}}@", 1, 1234));
    ASSERT_EQUAL_STRING(&s, "@{1234}@");
    mstr_free(&s);
}
