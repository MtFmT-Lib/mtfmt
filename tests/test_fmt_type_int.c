// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_fmt_type_int.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   格式化的整数值类型测试
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

void fmt_integer_i8(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(
        &s, "@{0:i8}#{1:i8}@", 2, (int8_t)(123), (int8_t)(0xff)
    ));
    ASSERT_EQUAL_STRING(&s, "@123#-1@");
    mstr_free(&s);
}

void fmt_integer_i16(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(
        &s, "@{0:i16}#{1:i16}@", 2, (int16_t)(123), (int16_t)(0xffff)
    ));
    ASSERT_EQUAL_STRING(&s, "@123#-1@");
    mstr_free(&s);
}

void fmt_integer_i32(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(
        &s,
        "@{0:i32}#{1:i32}@",
        2,
        (int32_t)(123),
        (int32_t)(0xffffffff)
    ));
    ASSERT_EQUAL_STRING(&s, "@123#-1@");
    mstr_free(&s);
}

void fmt_integer_u8(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(&s, "@{0:u8}@", 2, (uint8_t)(0xff)));
    ASSERT_EQUAL_STRING(&s, "@255@");
    mstr_free(&s);
}

void fmt_integer_u16(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(&s, "@{0:u16}@", 2, (uint16_t)(0xffff)));
    ASSERT_EQUAL_STRING(&s, "@65535@");
    mstr_free(&s);
}

void fmt_integer_u32(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(&s, "@{0:u32}@", 2, (uint32_t)(0xffffffff)));
    ASSERT_EQUAL_STRING(&s, "@4294967295@");
    mstr_free(&s);
}
