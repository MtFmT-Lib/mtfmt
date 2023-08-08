// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_string_trans.cpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   各个变换操作
 * @version 1.0
 * @date    2023-06-21
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "mm_heap.h"
#include "mm_string.h"
#include "test_helper.h"
#include "test_main.h"
#include "unity.h"
#include <stdio.h>

extern "C" void string_trans_clear(void)
{
    MString str;
    EVAL(mstr_create(&str, u8"Smile"));
    ASSERT_EQUAL_VALUE(str.count, 5);
    ASSERT_EQUAL_VALUE(str.length, 5);
    mstr_clear(&str);
    ASSERT_EQUAL_VALUE(str.count, 0);
    ASSERT_EQUAL_VALUE(str.length, 0);
    mstr_free(&str);
}

extern "C" void string_trans_reverse(void)
{
    // 偶数长度
    MString src1;
    EVAL(mstr_create(&src1, "1234567890"));
    ASSERT_EQUAL_STRING(&src1, "1234567890");
    mstr_reverse_self(&src1);
    ASSERT_EQUAL_STRING(&src1, "0987654321");
    mstr_free(&src1);
    // 奇数长度
    MString src2;
    EVAL(mstr_create(&src2, "123456789"));
    ASSERT_EQUAL_STRING(&src2, "123456789");
    mstr_reverse_self(&src2);
    ASSERT_EQUAL_STRING(&src2, "987654321");
    mstr_free(&src2);
    // Unicode
#if _MSTR_USE_UTF_8
    MString src_unicode;
    EVAL(mstr_create(&src_unicode, u8"😀😊"));
    ASSERT_EQUAL_STRING(&src_unicode, u8"😀😊");
    mstr_reverse_self(&src_unicode);
    ASSERT_EQUAL_STRING(&src_unicode, u8"😊😀");
    mstr_free(&src_unicode);
#endif
}
