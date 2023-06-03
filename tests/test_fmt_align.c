// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_fmt_align.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   不同格式化对齐方式下的测试
 * @version 1.0
 * @date    2023-05-28
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

void fmt_align_left(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:<8}@", &s, 1, 123));
    ASSERT_EQUAL_STRING(&s, "@123     @");
    mstr_free(&s);
}

void fmt_align_left_fill(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:#<8}@", &s, 1, 123));
    ASSERT_EQUAL_STRING(&s, "@123#####@");
    mstr_free(&s);
}

void fmt_align_left_long(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:#<8}@", &s, 1, (int32_t)123456789l));
    ASSERT_EQUAL_STRING(&s, "@123456789@");
    mstr_free(&s);
}

void fmt_align_right(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:>8}@", &s, 1, 123));
    ASSERT_EQUAL_STRING(&s, "@     123@");
    mstr_free(&s);
}

void fmt_align_right_fill(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:#>8}@", &s, 1, 123));
    ASSERT_EQUAL_STRING(&s, "@#####123@");
    mstr_free(&s);
}

void fmt_align_right_long(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:#>8}@", &s, 1, (int32_t)123456789l));
    ASSERT_EQUAL_STRING(&s, "@123456789@");
    mstr_free(&s);
}

void fmt_align_middle(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:=8}@", &s, 1, 123));
    ASSERT_EQUAL_STRING(&s, "@  123   @");
    mstr_free(&s);
}

void fmt_align_middle_fill(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:#=8}@", &s, 1, 123));
    ASSERT_EQUAL_STRING(&s, "@##123###@");
    mstr_free(&s);
}

void fmt_align_middle_long(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:#=8}@", &s, 1, (int32_t)123456789l));
    ASSERT_EQUAL_STRING(&s, "@123456789@");
    mstr_free(&s);
}
