/**
 * @file mm_typedef.h
 * @author HalfSweet (HalfSweet@HalfSweet.cn)
 * @brief 字符串格式化依赖的类型
 * @version 0.1
 * @date 2023-05-06
 * 
 * @copyright Copyright (c) 2023 HalfSweet
 * 
 */

#ifndef __MM_TYPEDEF_H__
#define __MM_TYPEDEF_H__

#if 0 // TODO

#else
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief 8位有符号整数
 *
 */
typedef int8_t i8_t;

/**
 * @brief 8位无符号整数
 *
 */
typedef uint8_t u8_t;

/**
 * @brief 字节(无符号)
 * 
 */
typedef uint8_t byte_t;

/**
 * @brief 16位有符号整数
 *
 */
typedef int16_t i16_t;

/**
 * @brief 16位无符号整数
 *
 */
typedef uint16_t u16_t;

/**
 * @brief 32位有符号整数
 *
 */
typedef int32_t i32_t;

/**
 * @brief 32位无符号整数
 *
 */
typedef uint32_t u32_t;

/**
 * @brief 尺寸(无符号)
 *
 */
typedef u32_t usize_t;

/**
 * @brief 尺寸(有符号, sizeof(isize_t) == sizeof(usize_t) ==
 * sizeof(iptr_t))
 *
 */
typedef i32_t isize_t, iptr_t;

/**
 * @brief 64位有符号整数
 *
 */
typedef int64_t i64_t;

/**
 * @brief 64位无符号整数
 *
 */
typedef uint64_t u64_t;

/**
 * @brief 时间戳(毫秒)
 *
 */
typedef u64_t tick_t;

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
#endif

/**
 * @brief RTC时间
 *
 */
typedef struct tagRTCTime
{
    //! (BCD) 年份
    u8_t year;

    //! (BCD) 月
    u8_t month;

    //! (BCD) 日
    u8_t day;

    //! (BCD) 时
    u8_t hour;

    //! (BCD) 分
    u8_t minute;

    //! (BCD) 秒
    u8_t second;

    //! (BCD) 星期
    u8_t week;

    //! 对齐项
    u8_t reserved;

    //! (BCD) 亚秒值, 单位: x0.1ms
    u32_t sub_second;
} RTCTime, sys_time_t;

#endif