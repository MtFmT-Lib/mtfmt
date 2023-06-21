// SPDX-License-Identifier: LGPL-3.0
/**
 * @file mm_typedef.h
 * @author HalfSweet (HalfSweet@HalfSweet.cn), 向阳
 * (hinatahoshino@foxmail.com)
 * @brief 字符串格式化依赖的类型
 * @version 0.1
 * @date 2023-05-06
 *
 * @copyright Copyright (C) 2023 HalfSweet, 向阳, all rights reserved.
 *
 */

#if !defined(__MM_TYPEDEF_H__)
#define __MM_TYPEDEF_H__ 1

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief 字节(无符号)
 *
 */
typedef uint8_t byte_t;

/**
 * @brief 尺寸(无符号)
 *
 */
typedef size_t usize_t, uptr_t;

/**
 * @brief 尺寸(有符号, sizeof(isize_t) == sizeof(usize_t) ==
 * sizeof(iptr_t))
 *
 */
typedef intptr_t isize_t, iptr_t;

/**
 * @brief 布尔值
 *
 */
typedef bool bool_t;
enum
{
    True = true,
    False = false,
};

/**
 * @brief 字符
 *
 */
typedef char mstr_char_t;

/**
 * @brief unicode代码点
 *
 */
typedef uint32_t mstr_codepoint_t;

/**
 * @brief RTC时间
 *
 */
typedef struct tagMStrTime
{
    //! (BCD) 年份
    uint16_t year;

    //! (BCD) 月
    uint8_t month;

    //! (BCD) 日
    uint8_t day;

    //! (BCD) 时
    uint8_t hour;

    //! (BCD) 分
    uint8_t minute;

    //! (BCD) 秒
    uint8_t second;

    //! (BCD) 星期
    uint8_t week;

    //! (BCD) 亚秒值, 单位: x0.1ms
    uint32_t sub_second;
} MStrTime;
#endif
