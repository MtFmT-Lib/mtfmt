// SPDX-License-Identifier: GPL-2.0
/**
 * @file    mm_string.h
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串
 * @version 1.0
 * @date    2023-04-22
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#if !defined(_INCLUDE_MM_STRING_H_)
#define _INCLUDE_MM_STRING_H_ 1
#include "mm_result.h"
#include "mu_typedef.h"

/**
 * @brief 栈上分配的空间大小
 *
 */
#define MSTR_STACK_REGION_SIZE 16

/**
 * @brief 字符串
 *
 */
typedef struct tagMString
{
    char* buff;

    /**
     * @brief 栈上分配的内存区
     *
     */
    char stack_region[MSTR_STACK_REGION_SIZE];

    /**
     * @brief 字符串长度
     *
     */
    usize_t length;

    /**
     * @brief 已经分配了的内存大小, cap_size >= length + 1
     *
     */
    usize_t cap_size;
} MString;

/**
 * @brief 创建字符串
 *
 * @param[out] str: 需要创建的字符串结构
 * @param[in] content: 需要丢到字符串里面的内容
 *
 * @return mstr_result_t: 创建结果
 */
extern mstr_result_t mstr_create(MString* str, const char* content);

// 创建空字符串
#define mstr_create_empty(s) (mstr_create((s), "\0"))

/**
 * @brief 清空字符串
 *
 * @param[inout] str: 需要清空的字符串
 */
extern void mstr_clear(MString* str);

/**
 * @brief 拼接字符串
 *
 * @param[out] str: 目标字符串
 * @param[in] other: 需要拼接的源字符串
 *
 * @return mstr_result_t: 拼接结果
 */
extern mstr_result_t mstr_concat(MString* str, const MString* other);

/**
 * @brief 拼接字符串(cstr)
 *
 * @param[out] str: 目标字符串
 * @param[in] other: 需要拼接的源字符串
 *
 * @return mstr_result_t: 拼接结果
 */
extern mstr_result_t mstr_concat_cstr(MString* str, const char* other);

/**
 * @brief 拼接字符串(cstr slice)
 *
 * @param[out] str: 目标字符串
 * @param[in] start: 需要拼接的源字符串起始
 * @param[in] end: 需要拼接的源字符串结束
 *
 * @return mstr_result_t: 拼接结果
 */
extern mstr_result_t mstr_concat_cstr_slice(
    MString* str, const char* start, const char* end
);

/**
 * @brief 向字符串尾部插入一个字符
 *
 * @param[inout] str: 字符串
 * @param[in] ch: 需要放入的字符
 * @return mstr_result_t: 结果
 */
extern mstr_result_t mstr_append(MString* str, char ch);

/**
 * @brief 向字符串尾部重复插入一个字符
 *
 * @param[inout] str: 字符串
 * @param[in] ch: 需要放入的字符
 * @param[in] cnt: 重复次数
 * @return mstr_result_t: 结果
 */
extern mstr_result_t mstr_repeat_append(
    MString* str, char ch, usize_t cnt
);

/**
 * @brief 翻转字符串
 *
 * @param[inout] str: 需要翻转的字符串
 *
 * @return mstr_result_t: 结果, 永远为MStr_Ok
 */
extern mstr_result_t mstr_reverse_self(MString* str);

/**
 * @brief 转换为cstr
 *
 * @param str: 字符串
 * @return const char*: C字符串指针
 */
extern const char* mstr_as_cstr(MString* str);

/**
 * @brief 释放一个字符串所占的内存
 *
 * @param str: 字符串结构
 */
extern void mstr_free(MString* str);

#endif // _INCLUDE_MM_STRING_H_
