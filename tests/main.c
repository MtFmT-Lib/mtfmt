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
}

void tearDown(void)
{
}

int main()
{
    UNITY_BEGIN();

    uint32_t cfg = mstr_configure();
    printf(
        "Build configure - version: 0x%x\n", MSTR_CONFIGURE_VER_VAL(cfg)
    );
    printf(
        "Build configure - compile: %d\n", MSTR_CONFIGURE_CC_VAL(cfg)
    );
    printf(
        "Build configure - configure: 0x%x\n",
        MSTR_CONFIGURE_CFG_VAL(cfg)
    );

    // 初始化堆
    mstr_heap_init(heap, RUNTIME_HEAP_SIZE);

    RUN_TEST(allocate_then_free);

    RUN_TEST(monadic_result_object_basic);
    RUN_TEST(monadic_result_copy_non_trivial_type);
    RUN_TEST(monadic_result_copy_assign_non_trivial_type);

    RUN_TEST(monadic_result_conjugate);
    RUN_TEST(monadic_result_flatten);
    RUN_TEST(monadic_result_map);
    RUN_TEST(monadic_result_map_err);
    RUN_TEST(monadic_result_and_then);
    RUN_TEST(monadic_result_or_else);
    RUN_TEST(monadic_result_or_value);
    RUN_TEST(monadic_result_or_exception);

    RUN_TEST(string_copy_create);
    RUN_TEST(string_move_create);
    RUN_TEST(string_length);
    RUN_TEST(string_char_at);
    RUN_TEST(string_insert);
    RUN_TEST(string_remove);

    RUN_TEST(string_equal);
    RUN_TEST(string_not_equal);
    RUN_TEST(string_end_with);
    RUN_TEST(string_start_with);

    RUN_TEST(string_concat_object);
    RUN_TEST(string_concat_c_str);
    RUN_TEST(string_concat_c_slice);

    RUN_TEST(string_append);
    RUN_TEST(string_repeat_append);

    RUN_TEST(string_trans_clear);
    RUN_TEST(string_trans_reverse);

    RUN_TEST(string_index);
    RUN_TEST(string_const_iterator);
    RUN_TEST(string_reverse_const_iterator);

    RUN_TEST(string_find);
    RUN_TEST(string_find_large);
    RUN_TEST(string_find_or_error);
    RUN_TEST(string_contain);

    RUN_TEST(string_retain_all);
    RUN_TEST(string_retain_endwith);
    RUN_TEST(string_retain_startwith);

    RUN_TEST(itoa_int_index);
    RUN_TEST(itoa_int_type);
    RUN_TEST(itoa_int_sign);
    RUN_TEST(itoa_int_from);
    RUN_TEST(itoa_uint_index);
    RUN_TEST(itoa_uint_type);
    RUN_TEST(itoa_uint_from);

    RUN_TEST(qtoa_signed);
    RUN_TEST(qtoa_signed_from);
    RUN_TEST(qtoa_unsigned);
    RUN_TEST(qtoa_unsigned_from);

    RUN_TEST(ftoa_basic);

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

    RUN_TEST(cpp_wrap_fmt);
    RUN_TEST(cpp_wrap_fmt_parser);

    usize_t alloc_cnt, free_cnt, usage_mark;
    mstr_heap_get_allocate_count(&alloc_cnt, &free_cnt);
    usage_mark = RUNTIME_HEAP_SIZE - mstr_heap_get_high_water_mark();
    printf("\nHeap usage: %u bytes(max)\n", (uint32_t)usage_mark);
    printf("            %u times allocating\n", (uint32_t)alloc_cnt);
    printf("            %u times free\n", (uint32_t)free_cnt);

    TEST_ASSERT_TRUE_MESSAGE(
        alloc_cnt == free_cnt, "alloc_cnt != free_cnt"
    );

    return UNITY_END();
}
