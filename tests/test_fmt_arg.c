// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_fmt_arg.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   不同格式化参数下的测试
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

void fmt_seq_arg_id(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(&s, "@{0:i32}-{1:i32}-{0:i32}@", 2, 123, 456));
    ASSERT_EQUAL_STRING(&s, "@123-456-123@");
    mstr_free(&s);
}

void fmt_seq_arg_id_err(void)
{
    MString s;
    mstr_result_t result = MStr_Ok;
    EVAL(mstr_create_empty(&s));
    // ERR: 参数太少
    result = mstr_format(&s, "@{0:i32}-{1:i32}@", 1, 123);
    TEST_ASSERT_TRUE(result == MStr_Err_InvaildArgumentID);
    mstr_clear(&s);
    // ERR: 参数顺序不对
    result = mstr_format(&s, "@{1:i32}-{0:i32}@", 2, 123, 456);
    TEST_ASSERT_TRUE(result == MStr_Err_UnusedArgumentID);
    mstr_clear(&s);
    // ERR: 参数顺序类型
    result = mstr_format(&s, "@{0:i32}-{0:u32}@", 1, 123);
    TEST_ASSERT_TRUE(result == MStr_Err_InvaildArgumentType);
    mstr_clear(&s);
    // free
    mstr_free(&s);
}
