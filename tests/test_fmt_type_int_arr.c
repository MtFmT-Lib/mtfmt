// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_fmt_type_int_arr.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   格式化的整数数组的类型测试
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

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void fmt_integer_array_i8(void)
{
    MString s;
    const int8_t arr[] = {123, (int8_t)(0xff)};
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(&s, "@{[0:i8]}@", 2, arr, ARRAY_SIZE(arr)));
    ASSERT_EQUAL_STRING(&s, "@123, -1@");
    mstr_free(&s);
}

void fmt_integer_array_i16(void)
{
    MString s;
    const int16_t arr[] = {123, (int16_t)(0xffff)};
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(&s, "@{[0:i16]}@", 2, arr, ARRAY_SIZE(arr)));
    ASSERT_EQUAL_STRING(&s, "@123, -1@");
    mstr_free(&s);
}

void fmt_integer_array_i32(void)
{
    MString s;
    const int32_t arr[] = {123, (int32_t)(0xffffffff)};
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(&s, "@{[0:i32]}@", 2, arr, ARRAY_SIZE(arr)));
    ASSERT_EQUAL_STRING(&s, "@123, -1@");
    mstr_free(&s);
}

void fmt_integer_array_u8(void)
{
    MString s;
    const uint8_t arr[] = {123, 0xff};
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(&s, "@{[0:u8]}@", 2, arr, ARRAY_SIZE(arr)));
    ASSERT_EQUAL_STRING(&s, "@123, 255@");
    mstr_free(&s);
}

void fmt_integer_array_u16(void)
{
    MString s;
    const uint16_t arr[] = {123, 0xffff};
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(&s, "@{[0:u16]}@", 2, arr, ARRAY_SIZE(arr)));
    ASSERT_EQUAL_STRING(&s, "@123, 65535@");
    mstr_free(&s);
}

void fmt_integer_array_u32(void)
{
    MString s;
    const uint32_t arr[] = {123, 0xffffffff};
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(&s, "@{[0:u32]}@", 2, arr, ARRAY_SIZE(arr)));
    ASSERT_EQUAL_STRING(&s, "@123, 4294967295@");
    mstr_free(&s);
}
