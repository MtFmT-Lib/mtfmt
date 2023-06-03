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
#include "main.h"
#include "helper.h"
#include "mtfmt.h"
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

int main()
{
    UNITY_BEGIN();
    RUN_TEST(allocate_then_free);

    RUN_TEST(fmt_align_left);
    RUN_TEST(fmt_align_left_fill);
    RUN_TEST(fmt_align_left_long);
    RUN_TEST(fmt_align_right);
    RUN_TEST(fmt_align_right_fill);
    RUN_TEST(fmt_align_right_long);
    RUN_TEST(fmt_align_middle);
    RUN_TEST(fmt_align_middle_fill);
    RUN_TEST(fmt_align_middle_long);
    RUN_TEST(fmt_seq_arg_id);
    RUN_TEST(fmt_seq_arg_id_err);

    RUN_TEST(fmt_behav_signed_bin);
    RUN_TEST(fmt_behav_signed_oct);
    RUN_TEST(fmt_behav_signed_dec);
    RUN_TEST(fmt_behav_signed_hex);

    RUN_TEST(fmt_sign_add);
    RUN_TEST(fmt_sign_sub);
    RUN_TEST(fmt_sign_space);

    RUN_TEST(fmt_sty_bin);
    RUN_TEST(fmt_sty_oct);
    RUN_TEST(fmt_sty_dec);
    RUN_TEST(fmt_sty_hex);
    RUN_TEST(fmt_sty_hex_prefix);

    RUN_TEST(append_char);
    RUN_TEST(equal_string);
    RUN_TEST(reverse_self);
    RUN_TEST(repeat_append_char);

    RUN_TEST(cpp_string_equal);

    return UNITY_END();
}
