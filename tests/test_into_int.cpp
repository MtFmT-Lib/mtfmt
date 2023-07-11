// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_into_int.cpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   整数格式化的测试
 * @version 1.0
 * @date    2023-06-30
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "helper.h"
#include "main.h"
#include "mtfmt.hpp"
#include "unity.h"
#include <stddef.h>
#include <stdio.h>

extern "C" void test_itoa_int_index(void)
{
    // @mstr_fmt_itoa
    // MStrFmtIntIndex_Bin
    mtfmt::string str_bin = u8"";
    str_bin.append_from(0b10100101, MStrFmtIntIndex_Bin);
    ASSERT_EQUAL_VALUE(str_bin, u8"10100101");
    // MStrFmtIntIndex_Oct
    mtfmt::string str_oct = u8"";
    str_oct.append_from(01234, MStrFmtIntIndex_Oct);
    ASSERT_EQUAL_VALUE(str_oct, u8"1234");
    // MStrFmtIntIndex_Dec
    mtfmt::string str_dec = u8"";
    str_dec.append_from(1234);
    ASSERT_EQUAL_VALUE(str_dec, u8"1234");
    // MStrFmtIntIndex_Hex
    mtfmt::string str_hex = u8"";
    str_hex.append_from(0xaa55, MStrFmtIntIndex_Hex);
    ASSERT_EQUAL_VALUE(str_hex, u8"aa55");
    // MStrFmtIntIndex_Hex_UpperCase
    mtfmt::string str_hex_upper = u8"";
    str_hex_upper.append_from(0xaa55, MStrFmtIntIndex_Hex_UpperCase);
    ASSERT_EQUAL_VALUE(str_hex_upper, u8"AA55");
    // MStrFmtIntIndex_Hex_WithPrefix
    mtfmt::string str_hex_prefix = u8"";
    str_hex_prefix.append_from(0xaa55, MStrFmtIntIndex_Hex_WithPrefix);
    ASSERT_EQUAL_VALUE(str_hex_prefix, u8"0xaa55");
    // MStrFmtIntIndex_Hex_UpperCase_WithPrefix
    mtfmt::string str_hex_prefix_upper = u8"";
    str_hex_prefix_upper.append_from(
        0xaa55, MStrFmtIntIndex_Hex_UpperCase_WithPrefix
    );
    ASSERT_EQUAL_VALUE(str_hex_prefix_upper, u8"0XAA55");
}

extern "C" void test_itoa_int_type(void)
{
    // @mstr_fmt_itoa
    // i8
    mtfmt::string str_8 = u8"";
    str_8.append_from(static_cast<int8_t>(123));
    str_8.append_from(static_cast<int8_t>(-123));
    ASSERT_EQUAL_VALUE(str_8, u8"123-123");
    // i16
    mtfmt::string str_16 = u8"";
    str_16.append_from(static_cast<int16_t>(123));
    str_16.append_from(static_cast<int16_t>(-123));
    ASSERT_EQUAL_VALUE(str_16, u8"123-123");
    // i32
    mtfmt::string str_32 = u8"";
    str_32.append_from(static_cast<int32_t>(123));
    str_32.append_from(static_cast<int32_t>(-123));
    ASSERT_EQUAL_VALUE(str_32, u8"123-123");
}

extern "C" void test_itoa_int_sign(void)
{
    // @mstr_fmt_itoa
    // MStrFmtSignDisplay_NegOnly
    mtfmt::string str_neg_only = u8"";
    str_neg_only.append_from(123);
    str_neg_only.append_from(-123);
    ASSERT_EQUAL_VALUE(str_neg_only, u8"123-123");
    // MStrFmtSignDisplay_Always
    mtfmt::string str_always = u8"";
    str_always.append_from(
        123, MStrFmtIntIndex_Dec, MStrFmtSignDisplay_Always
    );
    str_always.append_from(
        -123, MStrFmtIntIndex_Dec, MStrFmtSignDisplay_Always
    );
    ASSERT_EQUAL_VALUE(str_always, u8"+123-123");
    // MStrFmtSignDisplay_Neg_Or_Space
    mtfmt::string str_space = u8"";
    str_space.append_from(
        123, MStrFmtIntIndex_Dec, MStrFmtSignDisplay_Neg_Or_Space
    );
    str_space.append_from(
        -123, MStrFmtIntIndex_Dec, MStrFmtSignDisplay_Neg_Or_Space
    );
    ASSERT_EQUAL_VALUE(str_space, u8" 123-123");
}

extern "C" void test_itoa_uint_index(void)
{
    // @mstr_fmt_utoa
    // MStrFmtIntIndex_Bin
    mtfmt::string str_bin = u8"";
    str_bin.append_from(0b10100101u, MStrFmtIntIndex_Bin);
    ASSERT_EQUAL_VALUE(str_bin, u8"10100101");
    // MStrFmtIntIndex_Oct
    mtfmt::string str_oct = u8"";
    str_oct.append_from(01234u, MStrFmtIntIndex_Oct);
    ASSERT_EQUAL_VALUE(str_oct, u8"1234");
    // MStrFmtIntIndex_Dec
    mtfmt::string str_dec = u8"";
    str_dec.append_from(1234u);
    ASSERT_EQUAL_VALUE(str_dec, u8"1234");
    // MStrFmtIntIndex_Hex
    mtfmt::string str_hex = u8"";
    str_hex.append_from(0xaa55u, MStrFmtIntIndex_Hex);
    ASSERT_EQUAL_VALUE(str_hex, u8"aa55");
    // MStrFmtIntIndex_Hex_UpperCase
    mtfmt::string str_hex_upper = u8"";
    str_hex_upper.append_from(0xaa55u, MStrFmtIntIndex_Hex_UpperCase);
    ASSERT_EQUAL_VALUE(str_hex_upper, u8"AA55");
    // MStrFmtIntIndex_Hex_WithPrefix
    mtfmt::string str_hex_prefix = u8"";
    str_hex_prefix.append_from(0xaa55u, MStrFmtIntIndex_Hex_WithPrefix);
    ASSERT_EQUAL_VALUE(str_hex_prefix, u8"0xaa55");
    // MStrFmtIntIndex_Hex_UpperCase_WithPrefix
    mtfmt::string str_hex_prefix_upper = u8"";
    str_hex_prefix_upper.append_from(
        0xaa55u, MStrFmtIntIndex_Hex_UpperCase_WithPrefix
    );
    ASSERT_EQUAL_VALUE(str_hex_prefix_upper, u8"0XAA55");
}

extern "C" void test_itoa_uint_type(void)
{
    // @mstr_fmt_utoa
    // u8
    mtfmt::string str_8 = u8"";
    str_8.append_from(static_cast<uint8_t>(123));
    ASSERT_EQUAL_VALUE(str_8, u8"123");
    // u16
    mtfmt::string str_16 = u8"";
    str_16.append_from(static_cast<uint16_t>(123));
    ASSERT_EQUAL_VALUE(str_16, u8"123");
    // u32
    mtfmt::string str_32 = u8"";
    str_32.append_from(static_cast<uint32_t>(123));
    ASSERT_EQUAL_VALUE(str_32, u8"123");
}
