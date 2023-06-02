// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    fmt_behav.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   格式化行为
 * @version 1.0
 * @date    2023-06-02
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

void fmt_behav_signed_bin(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:b}@{1:i32:b}@", &s, 2, 10, -10));
    ASSERT_EQUAL_STRING(&s, "@1010@-1010@");
    mstr_free(&s);
}

void fmt_behav_signed_oct(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:o}@{1:i32:o}@", &s, 2, 10, -10));
    ASSERT_EQUAL_STRING(&s, "@12@-12@");
    mstr_free(&s);
}

void fmt_behav_signed_dec(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:d}@{1:i32:d}@", &s, 2, 10, -10));
    ASSERT_EQUAL_STRING(&s, "@10@-10@");
    mstr_free(&s);
}

void fmt_behav_signed_hex(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format("@{0:i32:h}@{1:i32:h}@", &s, 2, 10, -10));
    ASSERT_EQUAL_STRING(&s, "@a@-a@");
    mstr_free(&s);
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(fmt_behav_signed_bin);
    RUN_TEST(fmt_behav_signed_oct);
    RUN_TEST(fmt_behav_signed_dec);
    RUN_TEST(fmt_behav_signed_hex);
    return UNITY_END();
}
