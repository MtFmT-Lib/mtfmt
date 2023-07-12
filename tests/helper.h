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
#include "mm_cfg.h"
#include "mm_string.h"
#include "unity.h"

/**
 * @brief 确定两个值相等
 *
 */
#define ASSERT_EQUAL_VALUE(val1, val2)           \
    do {                                         \
        TEST_ASSERT_TRUE_MESSAGE(                \
            (val1) == (val2), #val1 " != " #val2 \
        );                                       \
    } while (0)

/**
 * @brief 确定一个值里面包括一个位
 *
 */
#define ASSERT_ISSET_BIT(val1, val2)                             \
    do {                                                         \
        TEST_ASSERT_TRUE_MESSAGE(                                \
            ((val1) & (val2)) == (val2), #val1 " not set " #val2 \
        );                                                       \
    } while (0)

/**
 * @brief 确定两个值不相等
 *
 */
#define ASSERT_NOTEQUAL_VALUE(val1, val2)        \
    do {                                         \
        TEST_ASSERT_TRUE_MESSAGE(                \
            (val1) != (val2), #val1 " == " #val2 \
        );                                       \
    } while (0)

/**
 * @brief 确定2个字符串相等
 *
 */
#define ASSERT_EQUAL_STRING(str, target_str)                  \
    do {                                                      \
        MString target;                                       \
        mstr_create(&target, (target_str));                   \
        mstr_bool_t res = mstr_equal(str, &target);           \
        mstr_free(&target);                                   \
        TEST_ASSERT_TRUE_MESSAGE(res, " str != " target_str); \
    } while (0)

/**
 * @brief 确定2个字符串不相等
 *
 */
#define ASSERT_NOT_EQUAL_STRING(str, target_str)    \
    do {                                            \
        MString target;                             \
        mstr_create(&target, (target_str));         \
        mstr_bool_t res = mstr_equal(str, &target); \
        mstr_free(&target);                         \
        TEST_ASSERT_FALSE(res);                     \
    } while (0)

/**
 * @brief 对某个表达式求值
 *
 */
#define EVAL(expr) TEST_ASSERT_TRUE(MSTR_SUCC(expr))

#endif // _INCLUDE_TEST_HELPER_H_
