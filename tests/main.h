// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    main.h
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   所有可用的测试
 * @version 1.0
 * @date    2023-06-03
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#if !defined(_INCLUDE_MAIN_H_)
#define _INCLUDE_MAIN_H_ 1

#if __cplusplus
extern "C"
{
#endif
    void allocate_then_free(void);

    void monadic_result_object_basic(void);
    void monadic_result_copy_non_trivial_type(void);
    void monadic_result_copy_assign_non_trivial_type(void);

    void monadic_result_conjugate(void);
    void monadic_result_flatten(void);
    void monadic_result_map(void);
    void monadic_result_map_err(void);
    void monadic_result_and_then(void);
    void monadic_result_or_else(void);
    void monadic_result_or_value(void);
    void monadic_result_or_exception(void);

    void string_copy_create(void);
    void string_move_create(void);
    void string_length(void);
    void string_char_at(void);
    void string_insert(void);
    void string_remove(void);

    void string_equal(void);
    void string_not_equal(void);
    void string_end_with(void);
    void string_start_with(void);

    void string_concat_object(void);
    void string_concat_c_str(void);
    void string_concat_c_slice(void);

    void string_append(void);
    void string_repeat_append(void);

    void string_trans_clear(void);
    void string_trans_reverse(void);

    void string_index(void);
    void string_const_iterator(void);
    void string_reverse_const_iterator(void);

    void string_find(void);
    void string_contain(void);
    void string_find_large(void);
    void string_find_or_error(void);

    void string_retain_all(void);
    void string_retain_endwith(void);
    void string_retain_startwith(void);

    void itoa_int_index(void);
    void itoa_int_type(void);
    void itoa_int_sign(void);
    void itoa_int_from(void);
    void itoa_uint_index(void);
    void itoa_uint_type(void);
    void itoa_uint_from(void);

    void qtoa_signed(void);
    void qtoa_unsigned(void);
    void qtoa_signed_from(void);
    void qtoa_unsigned_from(void);

    void ftoa_basic(void);

    void fmt_align_left(void);
    void fmt_align_left_fill(void);
    void fmt_align_left_long(void);
    void fmt_align_right(void);
    void fmt_align_right_fill(void);
    void fmt_align_right_long(void);
    void fmt_align_middle(void);
    void fmt_align_middle_fill(void);
    void fmt_align_middle_long(void);

    void fmt_seq_arg_id(void);
    void fmt_seq_arg_id_err(void);

    void fmt_quat_value_sign(void);
    void fmt_quat_value_dualprec(void);
    void fmt_quat_value_singprec(void);

    void fmt_behav_signed_bin(void);
    void fmt_behav_signed_oct(void);
    void fmt_behav_signed_dec(void);
    void fmt_behav_signed_hex(void);

    void fmt_sign_add(void);
    void fmt_sign_sub(void);
    void fmt_sign_space(void);

    void fmt_sty_bin(void);
    void fmt_sty_oct(void);
    void fmt_sty_dec(void);
    void fmt_sty_hex(void);
    void fmt_sty_hex_prefix(void);

    void fmt_array(void);
    void fmt_array_element_style(void);
    void fmt_array_userdefind_split(void);

    void fmt_chrono_default_f(void);
    void fmt_chrono_default_g(void);
    void fmt_chrono_userdef_year(void);
    void fmt_chrono_userdef_month(void);
    void fmt_chrono_userdef_day(void);
    void fmt_chrono_userdef_hour(void);
    void fmt_chrono_userdef_minute(void);
    void fmt_chrono_userdef_second(void);
    void fmt_chrono_userdef_subsecond(void);
    void fmt_chrono_userdef_week(void);

    void fmt_escape_bracket(void);

    void sync_io_write(void);

    void cpp_wrap_fmt(void);
    void cpp_wrap_fmt_parser(void);
#if __cplusplus
}
#endif
#endif // _INCLUDE_MAIN_H_
