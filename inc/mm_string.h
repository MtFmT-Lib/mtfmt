// SPDX-License-Identifier: LGPL-3.0
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
#include "mm_cfg.h"
#include "mm_result.h"
#include "mm_typedef.h"

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
     * @brief 字符串的字节长度
     *
     */
    usize_t count;

    /**
     * @brief 字符串长度
     *
     */
    usize_t length;

    /**
     * @brief 已经分配了的内存大小, cap_size >= count + 1
     *
     */
    usize_t cap_size;
} MString;

/**
 * @brief 字符串迭代器
 *
 */
typedef struct tagMStringIter
{
    /**
     * @brief 位置
     *
     */
    const char* it;

    /**
     * @brief 结束位置
     *
     */
    const char* it_end;

    /**
     * @brief 剩余的长度
     *
     */
    usize_t rem_length;
} MStringIter;

/**
 * @brief 字符串迭代器 (可变的)
 *
 */
typedef struct tagMStringIterMut
{
    /**
     * @brief 位置
     *
     */
    char* it;

    /**
     * @brief 结束位置
     *
     */
    const char* it_end;

    /**
     * @brief 剩余的长度
     *
     */
    usize_t rem_length;
} MStringIterMut;

/**
 * @brief 创建字符串
 *
 * @param[out] str: 需要创建的字符串结构
 * @param[in] content: 需要丢到字符串里面的内容
 *
 * @return mstr_result_t: 创建结果
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_create(MString* str, const char* content);

//! 创建空字符串
#define mstr_create_empty(s) (mstr_create((s), "\0"))

/**
 * @brief 移动字符串来创建
 *
 * @param[out] str: 需要创建的字符串结构
 * @param[inout] other: 需要移动的字符串
 */
MSTR_EXPORT_API(void)
mstr_move_create(MString* str, MString* other);

/**
 * @brief 复制字符串来创建
 *
 * @param[out] str: 需要创建的字符串结构
 * @param[in] other: 需要移动的字符串
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_copy_create(MString* str, const MString* other);

/**
 * @brief 取得第idx位置的字符ch
 *
 * @attention 边界检查失败会造成assert false, 函数失败会返回0
 * 而不是返回mstr_result
 */
MSTR_EXPORT_API(mstr_codepoint_t)
mstr_char_at(const MString* str, usize_t idx);

/**
 * @brief 拼接字符串
 *
 * @param[out] str: 目标字符串
 * @param[in] other: 需要拼接的源字符串
 *
 * @return mstr_result_t: 拼接结果
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_concat(MString* str, const MString* other);

/**
 * @brief 拼接字符串(cstr)
 *
 * @param[out] str: 目标字符串
 * @param[in] other: 需要拼接的源字符串
 *
 * @return mstr_result_t: 拼接结果
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_concat_cstr(MString* str, const char* other);

/**
 * @brief 拼接字符串(cstr slice)
 *
 * @param[out] str: 目标字符串
 * @param[in] start: 需要拼接的源字符串起始
 * @param[in] end: 需要拼接的源字符串结束
 *
 * @return mstr_result_t: 拼接结果
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_concat_cstr_slice(
    MString* str, const char* start, const char* end
);

/**
 * @brief 向字符串尾部插入一个字符
 *
 * @param[inout] str: 字符串
 * @param[in] ch: 需要放入的字符(unicode point)
 * @return mstr_result_t: 结果
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_append(MString* str, mstr_codepoint_t ch);

/**
 * @brief 向字符串尾部重复插入一个字符
 *
 * @param[inout] str: 字符串
 * @param[in] ch: 需要放入的字符(unicode point)
 * @param[in] cnt: 重复次数
 * @return mstr_result_t: 结果
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_repeat_append(MString* str, mstr_codepoint_t ch, usize_t cnt);

/**
 * @brief 清空字符串
 *
 * @param[inout] str: 需要清空的字符串
 */
MSTR_EXPORT_API(void) mstr_clear(MString* str);

/**
 * @brief 翻转字符串
 *
 * @param[inout] str: 需要翻转的字符串
 *
 * @return mstr_result_t: 结果, 永远为MStr_Ok
 */
MSTR_EXPORT_API(void) mstr_reverse_self(MString* str);

/**
 * @brief 转换为cstr
 *
 * @param str: 字符串
 * @return const char*: C字符串指针
 */
MSTR_EXPORT_API(const char*) mstr_as_cstr(MString* str);

/**
 * @brief 判断两个字符串是否相等
 *
 * @param[in] a: 字符串a
 * @param[in] b: 字符串b
 *
 * @return bool_t: 字符串相等情况
 */
MSTR_EXPORT_API(bool_t) mstr_equal(const MString* a, const MString* b);

/**
 * @brief 取得迭代器
 *
 * @param[out] it: 迭代器输出
 * @param[in] str: 原字符串
 *
 */
MSTR_EXPORT_API(void) mstr_iter(MStringIter* it, const MString* str);

/**
 * @brief 取得可变迭代器
 *
 * @param[out] it: 迭代器输出
 * @param[in] str: 原字符串
 */
MSTR_EXPORT_API(void) mstr_iter_mut(MStringIterMut* it, MString* str);

/**
 * @brief 移动到下一个位置
 *
 */
#define mstr_iter_move_next(it)                      \
    do {                                             \
        usize_t step = mstr_char_length(*((it).it)); \
        (it).it += step;                             \
        (it).rem_leng -= 1;                          \
    } while (0)

/**
 * @brief 判断迭代器是否已到末尾
 *
 */
#define mstr_iter_is_end(it) ((it).it == (it).it_end)

/**
 * @brief 判断前导字符lead后面的字符长度, 前导字符包括在内
 *
 * @note 在UTF-8功能启用的情况下, 其判断UTF-8编码的字符长度, 错误返回0,
 * 否则, 该函数永远返回1
 *
 */
MSTR_EXPORT_API(usize_t) mstr_char_length(char lead);

/**
 * @brief 转换为UTF-8
 *
 * @param code: 字符代码点
 * @param result: 转换输出, 至少要有6个bytes
 * @param result_len: 转换输出的有效长度
 *
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_as_utf8(
    mstr_codepoint_t code, mstr_char_t* result, usize_t* result_len
);

/**
 * @brief 取得lead字符ch[0]所跟着的内容的unicode代码点值
 *
 * @param[out] code: 解码结果
 * @param[in] ch: 字符串
 * @param[in] byte_count: 字符的字节数
 *
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_codepoint_of(
    mstr_codepoint_t* code, const mstr_char_t* ch, usize_t byte_count
);

/**
 * @brief 释放一个字符串所占的内存
 *
 * @param str: 字符串结构
 */
MSTR_EXPORT_API(void) mstr_free(MString* str);

#endif // _INCLUDE_MM_STRING_H_
