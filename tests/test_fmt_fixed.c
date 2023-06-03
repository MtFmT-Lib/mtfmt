// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_fmt_fixed.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   定点数相关的test
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

void fmt_quat_value_sign(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    // 1/4096 ..
    EVAL(mstr_format("@{0:q12}@{1:q12}@", &s, 2, 1, -1));
    ASSERT_EQUAL_STRING(&s, "@0.000244140625@-0.000244140625@");
    // 无符号版本
    // 1048575 * 4096 = 0xfffff000
    mstr_clear(&s);
    EVAL(mstr_format("@{0:q12u}@{1:q12u}@", &s, 2, 4096, -4096));
    ASSERT_EQUAL_STRING(&s, "@1@1048575@");
    mstr_free(&s);
}

void fmt_quat_value_dualprec(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    // 1/4096 ..
    EVAL(mstr_format("@{0:q12}@{1:q31}@", &s, 2, 1, 1));
    ASSERT_EQUAL_STRING(
        &s, "@0.000244140625@0.0000000000000000000002166527565@"
    );
    mstr_free(&s);
}

void fmt_quat_value_singprec(void)
{
}
