// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_fmt_sign.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   不同符号设置下的测试
 * @version 1.0
 * @date    2023-05-30
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "helper.h"
#include "main.h"
#include "mm_fmt.h"
#include "mm_heap.h"
#include "unity.h"
#include <stddef.h>
#include <stdio.h>

void fmt_sign_add(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(
        &s, "@{0:i32:+}@{1:i32:+}@{2:i32:+}@", 3, 123, -123, 0
    ));
    ASSERT_EQUAL_STRING(&s, "@+123@-123@0@");
    mstr_free(&s);
}

void fmt_sign_sub(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(
        &s, "@{0:i32:-}@{1:i32:-}@{2:i32:-}@", 3, 123, -123, 0
    ));
    ASSERT_EQUAL_STRING(&s, "@123@-123@0@");
    mstr_free(&s);
}

void fmt_sign_space(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(
        &s, "@{0:i32: }@{1:i32: }@{2:i32: }@", 3, 123, -123, 0
    ));
    ASSERT_EQUAL_STRING(&s, "@ 123@-123@ 0@");
    mstr_free(&s);
}
