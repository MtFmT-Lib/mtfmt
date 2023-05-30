// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    helper.h
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   测试相关的helper
 * @version 1.0
 * @date    2023-05-28
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#if !defined(_INCLUDE_TEST_HELPER_H_)
#define _INCLUDE_TEST_HELPER_H_ 1
#include "mm_string.h"
#include "unity.h"

/**
 * @brief 判断字符串str和字面值target_str是否一致
 *
 */
#define IS_EQUAL_STRING(str, target_str)       \
    ({                                         \
        MString target;                        \
        mstr_create(&target, (target_str));    \
        bool_t res = mstr_equal(str, &target); \
        mstr_free(&target);                    \
        res;                                   \
    })

/**
 * @brief 确定2个字符串相等
 *
 */
#define ASSERT_EQUAL_STRING(str, target_str) \
    TEST_ASSERT_TRUE(IS_EQUAL_STRING(str, target_str))

/**
 * @brief 确定2个字符串不相等
 *
 */
#define ASSERT_NOT_EQUAL_STRING(str, target_str) \
    TEST_ASSERT_FALSE(IS_EQUAL_STRING(str, target_str))

/**
 * @brief 对某个表达式求值
 *
 */
#define EVAL(expr) TEST_ASSERT_TRUE(MSTR_SUCC(expr))

#endif // _INCLUDE_TEST_HELPER_H_
