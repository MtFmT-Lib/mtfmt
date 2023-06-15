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

    printf("Build configure: 0x%08x\n", mstr_configure());

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

    RUN_TEST(fmt_quat_value_sign);
    RUN_TEST(fmt_quat_value_dualprec);
    RUN_TEST(fmt_quat_value_singprec);

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

    RUN_TEST(fmt_array);
    RUN_TEST(fmt_array_element_style);
    RUN_TEST(fmt_array_userdefind_split);

    RUN_TEST(fmt_chrono_default_f);
    RUN_TEST(fmt_chrono_default_g);
    RUN_TEST(fmt_chrono_userdef_year);
    RUN_TEST(fmt_chrono_userdef_month);
    RUN_TEST(fmt_chrono_userdef_day);
    RUN_TEST(fmt_chrono_userdef_hour);
    RUN_TEST(fmt_chrono_userdef_minute);
    RUN_TEST(fmt_chrono_userdef_second);
    RUN_TEST(fmt_chrono_userdef_subsecond);
    RUN_TEST(fmt_chrono_userdef_week);

    RUN_TEST(fmt_escape_bracket);

    RUN_TEST(sync_io_write);

    RUN_TEST(cpp_string_equal);
    RUN_TEST(cpp_string_not_equal);
    RUN_TEST(cpp_string_push);
    RUN_TEST(cpp_string_concat);

    RUN_TEST(cpp_monadic_result_and_then);
    RUN_TEST(cpp_monadic_result_or_value);
    RUN_TEST(cpp_monadic_result_or_exception);

    RUN_TEST(cpp_wrap_fmt);

    return UNITY_END();
}
