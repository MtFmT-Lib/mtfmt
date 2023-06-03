// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_string.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串测试
 * @version 1.0
 * @date    2023-05-28
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "helper.h"
#include "main.h"
#include "mm_heap.h"
#include "mm_string.h"
#include "unity.h"
#include <stdio.h>

void equal_string(void)
{
    MString src;
    EVAL(mstr_create(&src, "Example"));
    // equal
    ASSERT_EQUAL_STRING(&src, "Example");
    // neq
    ASSERT_NOT_EQUAL_STRING(&src, "Exampl");
    ASSERT_NOT_EQUAL_STRING(&src, "Example1");
    ASSERT_NOT_EQUAL_STRING(&src, "ExAmple");
    mstr_free(&src);
}

void append_char(void)
{
    MString src;
    EVAL(mstr_create(&src, "Example"));
    // append
    EVAL(mstr_append(&src, '-'));
    ASSERT_EQUAL_STRING(&src, "Example-");
    mstr_free(&src);
}

void repeat_append_char(void)
{
    MString src;
    EVAL(mstr_create(&src, "Large Example|"));
    // append
    EVAL(mstr_repeat_append(&src, '#', 8));
    ASSERT_EQUAL_STRING(&src, "Large Example|########");
    // append(触发heap alloc)
    EVAL(mstr_repeat_append(&src, '@', 8));
    ASSERT_EQUAL_STRING(&src, "Large Example|########@@@@@@@@");
    mstr_free(&src);
}

void reverse_self(void)
{
    // 偶数长度
    MString src1;
    EVAL(mstr_create(&src1, "1234567890"));
    ASSERT_EQUAL_STRING(&src1, "1234567890");
    EVAL(mstr_reverse_self(&src1));
    ASSERT_EQUAL_STRING(&src1, "0987654321");
    mstr_free(&src1);
    // 奇数长度
    MString src2;
    EVAL(mstr_create(&src2, "123456789"));
    ASSERT_EQUAL_STRING(&src2, "123456789");
    EVAL(mstr_reverse_self(&src2));
    ASSERT_EQUAL_STRING(&src2, "987654321");
    mstr_free(&src2);
}
