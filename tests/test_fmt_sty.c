// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_fmt_sty.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   值格式化样式的测试
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

void fmt_sty_bin(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:b}@", &s, 1, 0xa5));
    ASSERT_EQUAL_STRING(&s, "@10100101@");
    mstr_free(&s);
}

void fmt_sty_oct(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:o}@", &s, 1, 0xa5));
    ASSERT_EQUAL_STRING(&s, "@245@");
    mstr_free(&s);
}

void fmt_sty_dec(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32}@{0:i32:d}@", &s, 1, 0xa5));
    ASSERT_EQUAL_STRING(&s, "@165@165@");
    mstr_free(&s);
}

void fmt_sty_hex(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:h}@{0:i32:H}@", &s, 1, 0xa5));
    ASSERT_EQUAL_STRING(&s, "@a5@A5@");
    mstr_free(&s);
}

void fmt_sty_hex_prefix(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:x}@{0:i32:X}@", &s, 1, 0xa5));
    ASSERT_EQUAL_STRING(&s, "@0xa5@0XA5@");
    mstr_free(&s);
}
