// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    fmt_sign.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   不同符号设置下的测试
 * @version 1.0
 * @date    2023-05-30
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "helper.h"
#include "mm_fmt.h"
#include "mm_heap.h"
#include "unity.h"
#include <stddef.h>
#include <stdio.h>

#define RUNTIME_HEAP_SIZE 2048

/**
 * @brief 堆
 *
 */
static byte_t heap[RUNTIME_HEAP_SIZE];

void setUp(void)
{
    mstr_heap_init(heap, RUNTIME_HEAP_SIZE);
}

void tearDown(void)
{
}

void fmt_sign_add(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(
        "@{0:i32:+}@{1:i32:+}@{2:i32:+}@", &s, 3, 123, -123, 0
    ));
    ASSERT_EQUAL_STRING(&s, "@+123@-123@0@");
    mstr_free(&s);
}

void fmt_sign_sub(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(
        "@{0:i32:-}@{1:i32:-}@{2:i32:-}@", &s, 3, 123, -123, 0
    ));
    ASSERT_EQUAL_STRING(&s, "@123@-123@0@");
    mstr_free(&s);
}

void fmt_sign_space(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(
        "@{0:i32: }@{1:i32: }@{2:i32: }@", &s, 3, 123, -123, 0
    ));
    ASSERT_EQUAL_STRING(&s, "@ 123@-123@ 0@");
    mstr_free(&s);
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(fmt_sign_add);
    RUN_TEST(fmt_sign_sub);
    RUN_TEST(fmt_sign_space);
    return UNITY_END();
}
