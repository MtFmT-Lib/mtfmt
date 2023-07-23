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
#include "mm_type.h"

/**
 * @brief 栈上分配的空间大小
 *
 */
#define MSTR_STACK_REGION_SIZE 16

/**
 * @brief 字符串替换选项
 *
 */
typedef enum tagMStringReplaceOption
{
    /**
     * @brief 仅替换开始位置
     *
     */
    MStringReplaceOption_StartWith,

    /**
     * @brief 仅替换末尾位置
     *
     */
    MStringReplaceOption_EndWith,

    /**
     * @brief 全部替换
     *
     */
    MStringReplaceOption_All,
} MStringReplaceOption;

/**
 * @brief 字符串替换信息
 *
 */
typedef struct tagMStringReplaceTarget
{
    /**
     * @brief 替换选项
     *
     */
    MStringReplaceOption opt;

    /**
     * @brief 模式串
     *
     */
    const char* substr;

    /**
     * @brief 模式串长度
     *
     */
    usize_t substr_len;

    /**
     * @brief 替换为的目标字串
     *
     */
    const char* replace_to;

    /**
     * @brief 目标串长度
     *
     */
    usize_t replace_to_len;
} MStringReplaceTarget;

/**
 * @brief 字符串匹配信息
 *
 */
typedef struct tagMStringMatchResult
{
    /**
     * @brief 是否找到了目标
     *
     */
    usize_t is_matched;

    /**
     * @brief 目标位置的字符索引
     *
     */
    usize_t begin_pos;

    /**
     * @brief 目标位置的字符char的偏移
     *
     */
    usize_t begin_offset;
} MStringMatchResult;

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
 * @brief 初始化一个空的字符串
 *
 * @param[inout] pstr: 字符串
 */
#define mstr_init(pstr)                            \
    do {                                           \
        (pstr)->count = 0;                         \
        (pstr)->length = 0;                        \
        (pstr)->buff = (pstr)->stack_region;       \
        (pstr)->cap_size = MSTR_STACK_REGION_SIZE; \
    } while (0)

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

/**
 * @brief 创建空的总付出
 *
 */
#define mstr_create_empty(s) mstr_create(s, "")

/**
 * @brief 从other移动字符串到str
 *
 * @param[out] str: 目标字符串
 * @param[inout] other: 需要移动的字符串
 *
 * @attention 该函数转移other的所有权, other不需要再次释放
 */
MSTR_EXPORT_API(void)
mstr_move_from(MString* str, MString* other);

/**
 * @brief 从other复制字符串到str
 *
 * @param[out] str: 目标字符串
 * @param[in] other: 需要移动的字符串
 *
 * @attention 该函数会清空释放原有的字符串并创建一个新的,
 * other需要再次释放
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_copy_from(MString* str, const MString* other);

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
 */
MSTR_EXPORT_API(void) mstr_reverse_self(MString* str);

/**
 * @brief 翻转字符串, 不做任何其它处理
 *
 * @param[inout] str: 需要翻转的字符串
 *
 * @attention 通常情况下不需要使用此函数,
 * 此函数不会保证反转后的编码正确。它设计出来是为了给明确只有ASCII字符的字符串做翻转的,
 * 在UTF-8编码下, 该函数可降低很多操作
 */
MSTR_EXPORT_API(void) mstr_reverse_only(MString* str);

/**
 * @brief 转换为cstr
 *
 * @param str: 字符串
 * @return const char*: C字符串指针
 */
MSTR_EXPORT_API(const char*) mstr_c_str(MString* str);

/**
 * @brief 判断两个字符串是否相等
 *
 * @param[in] a: 字符串a
 * @param[in] b: 字符串b
 *
 * @return mstr_bool_t: 字符串相等情况
 */
MSTR_EXPORT_API(mstr_bool_t)
mstr_equal(const MString* a, const MString* b);

/**
 * @brief 判断字符串是否以某个字串开始
 *
 * @param[in] str: 字符串
 * @param[in] prefix: 需要确认其开始的内容
 * @param[in] prefix_cnt: prefix的char字符数
 */
MSTR_EXPORT_API(mstr_bool_t)
mstr_start_with(
    const MString* str, const char* prefix, usize_t prefix_cnt
);

/**
 * @brief 判断字符串是否以某个字串结束
 *
 * @param[in] str: 字符串
 * @param[in] suffix: 需要确认其结束的内容
 * @param[in] suffix_cnt: suffix的char字符数
 */
MSTR_EXPORT_API(mstr_bool_t)
mstr_end_with(
    const MString* str, const char* suffix, usize_t suffix_cnt
);

/**
 * @brief 判断字符串是否含有pattern
 *
 * @param[in] str: 字符串
 * @param[in] pattern: 需要包含的内容
 * @param[in] pattern_cnt: 需要包含的内容的字符数
 *
 * @attention 在发生意外的错误(如编码问题, 内存分配失败)时,
 * 该函数也会返回False
 */
MSTR_EXPORT_API(mstr_bool_t)
mstr_contains(
    const MString* str, const char* pattern, usize_t pattern_cnt
);

/**
 * @brief 取得第idx位置的字符ch
 *
 * @attention 边界检查失败会造成assert false, 函数失败会返回0
 * 而不是返回mstr_result
 */
MSTR_EXPORT_API(mstr_codepoint_t)
mstr_char_at(const MString* str, usize_t idx);

/**
 * @brief 从字符串中移除idx位置的字符
 *
 * @param[inout] str: 字符串
 * @param[out] remoed_ch: 被移除的字符, 可以为NULL
 * @param[in] idx: 需要移除的字符位置
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_remove(MString* str, mstr_codepoint_t* removed_ch, usize_t idx);

/**
 * @brief 从字符串中idx位置插入一个字符
 *
 * @param[inout] str: 字符串
 * @param[in] idx: 需要插入的字符位置
 * @param[in] ch: 需要插入的字符
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_insert(MString* str, usize_t idx, mstr_codepoint_t ch);

/**
 * @brief 查找子串第一次出现的位置
 *
 * @param[in] str: 字符串a
 * @param[out] f_res: 查找结果
 * @param[in] begin_pos: 开始查找的位置
 * @param[in] pattern: 需要查找的子串
 * @param[in] pattern_cnt: pattern的char个数
 *
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_find(
    const MString* str,
    MStringMatchResult* f_res,
    usize_t begin_pos,
    const char* pattern,
    usize_t pattern_cnt
);

/**
 * @brief 进行字符串替换
 *
 * @param[inout] str: 字符串
 * @param[in] target: 需要替换的目标
 * @param[in] target_cnt: 需要替换的目标的数组大小
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_replace_multi(
    MString* str, const MStringReplaceTarget* target, usize_t target_cnt
);

/**
 * @brief 设置替换的目标
 *
 * @param[out] rep: 替换目标的结构
 * @param[in] opt: 替换选项
 * @param[in] patt: 模式串
 * @param[in] target: 替换为的目标
 */
MSTR_EXPORT_API(void)
mstr_replace_set_target(
    MStringReplaceTarget* rep,
    MStringReplaceOption opt,
    const char* pattern,
    const char* target
);

/**
 * @brief 从字符串中移除所有匹配substr的字符
 *
 * @param[inout] str: 字符串
 * @param[in] opt: 替换模式
 * @param[in] pattern: 需要移除的pattern
 * @param[in] pattern_cnt: 需要移除的pattern的字符计数
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_retain(
    MString* str,
    MStringReplaceOption opt,
    const char* pattern,
    usize_t pattern_cnt
);

/**
 * @brief 进行字符串替换(单个目标)
 *
 * @param[inout] str: 字符串
 * @param[in] target: 需要替换的子串
 * @param[in] replace_to: 需要替换为的结果
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_replace(MString* str, const char* pattern, const char* replace_to);

/**
 * @brief 取得迭代器
 *
 * @param[out] it: 迭代器输出
 * @param[in] str: 原字符串
 *
 */
MSTR_EXPORT_API(void) mstr_iter(MStringIter* it, const MString* str);

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
 * @brief 判断buff的lead字符偏移量(相反数, 比如偏移量是-2但是会返回2)
 *
 * @note 在UTF-8功能启用的情况下, 其判断UTF-8编码的字符长度, 错误返回0,
 * 否则, 该函数永远返回1
 *
 * @param[in] buff: 字符数组
 * @param[in] hist_len: 字符数组允许往前查找的长度, 大于等于0
 * (不然就返回0啦), 如果大于6, 那么最大会被限制到6(1个utf-8编码的长度)
 *
 */
MSTR_EXPORT_API(usize_t)
mstr_lead_char_offset(const mstr_char_t* buff, usize_t hist_len);

/**
 * @brief 转换为UTF-8, 如果未启用UTF-8, 该函数返回未实现错误
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
 * 如果未启用UTF-8, 该函数返回未实现错误
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
 * @brief 找到第idx个字符的offset
 *
 * @param[in] str: 字符串
 * @param[in] idx: 字符索引
 *
 * @attention 该函数会进行边界检查
 *
 * @return usize_t: 相对于 buff 的偏移量, buff + 返回值 是该字符的 lead
 *
 */
MSTR_EXPORT_API(usize_t)
mstr_char_offset_at(const MString* str, usize_t idx);

/**
 * @brief 释放一个字符串所占的内存
 *
 * @param str: 字符串结构
 */
MSTR_EXPORT_API(void) mstr_free(MString* str);

#endif // _INCLUDE_MM_STRING_H_
