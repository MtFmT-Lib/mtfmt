// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_pattern.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   模式串匹配和替换
 * @version 1.0
 * @date    2023-07-23
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */

#define MSTR_IMP_SOURCES 1

#include "mm_heap.h"
#include "mm_string.h"
#include <stddef.h>
#include <string.h>

/**
 * @brief 指定使用BM算法的阈值
 *
 */
#define BM_THRESHOLD_CNT  6

/**
 * @brief 单个char的最大值(unsigned)
 *
 */
#define BM_CHAR_INDEX_MAX 256

//
// private:
//

static mstr_result_t mstr_find_simple(
    isize_t*, const char*, usize_t, usize_t, const char*, usize_t
);
static mstr_result_t patt_match_impl(
    isize_t*, const mstr_char_t*, usize_t, const mstr_char_t*, usize_t
);
static mstr_result_t mstr_retain_start_with_impl(
    MString*, const char*, usize_t
);
static mstr_result_t mstr_retain_end_with_impl(
    MString*, const char*, usize_t
);
static mstr_result_t mstr_retain_all_impl(
    MString*, const char*, usize_t
);
static mstr_result_t mstr_retain_all_impl_helper(
    MString*, const MString*, const char*, usize_t, usize_t
);
static void make_jump_table(uint8_t*, const mstr_char_t*, usize_t);
static mstr_result_t unicode_length_of(usize_t*, const char*, usize_t);
//
// public:
//
MSTR_EXPORT_API(mstr_result_t)
mstr_find(
    const MString* str,
    MStringMatchResult* f_res,
    usize_t begin_pos,
    const char* pattern,
    usize_t pattern_cnt
)
{
    usize_t offset;
    isize_t find_res;
    mstr_result_t ret_code;
    offset = mstr_char_offset_at(str, begin_pos);
    ret_code = mstr_find_simple(
        &find_res, str->buff, str->count, offset, pattern, pattern_cnt
    );
    if (MSTR_SUCC(ret_code)) {
        if (find_res == -1) {
            f_res->is_matched = False;
            f_res->begin_pos = 0;
            f_res->begin_offset = 0;
        }
        else {
            usize_t cur_cnt = 0;
            usize_t cur_len = 0;
            const char* it = str->buff + offset;
            while (cur_cnt != (usize_t)find_res) {
                usize_t cnt = mstr_char_length(*it);
                it += cnt;
                cur_len += 1;
                cur_cnt += cnt;
            }
            f_res->is_matched = True;
            f_res->begin_pos = cur_len;
            f_res->begin_offset = (usize_t)find_res;
        }
    }
    return ret_code;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_replace(
    MString* str,
    MStringReplaceOption opt,
    const char* pattern,
    usize_t pattern_cnt,
    const char* replace_to,
    usize_t replace_to_cnt
)
{

    return MStr_Err_NoImplemention;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_retain(
    MString* str,
    MStringReplaceOption opt,
    const char* patt,
    usize_t patt_cnt
)
{
    if (patt_cnt == 0) {
        return MStr_Ok;
    }
    else if (opt == MStringReplaceOption_StartWith) {
        return mstr_retain_start_with_impl(str, patt, patt_cnt);
    }
    else if (opt == MStringReplaceOption_EndWith) {
        return mstr_retain_end_with_impl(str, patt, patt_cnt);
    }
    else if (opt == MStringReplaceOption_All) {
        return mstr_retain_all_impl(str, patt, patt_cnt);
    }
    else {
        mstr_unreachable();
        return MStr_Err_NoImplemention;
    }
}

/**
 * @brief 字符串查找(仅返回字符的count)
 *
 * @param[out] result: 结果, -1表示没有找到
 * @param[in] main_str: 主串
 * @param[in] main_str_cnt: 主串的字符计数
 * @param[in] begin_pos: 开始查找的位置的字符计数偏移量
 * @param[in] pattern: 模式串
 * @param[in] pattern_cnt: 模式串的长度计数
 *
 */
static mstr_result_t mstr_find_simple(
    isize_t* result,
    const char* main_str,
    usize_t main_str_cnt,
    usize_t begin_pos,
    const char* pattern,
    usize_t pattern_cnt
)
{
    isize_t find_res = -1;
    mstr_result_t ret_code;
    if (begin_pos > main_str_cnt || pattern_cnt > main_str_cnt) {
        // 超过主串的长度了, 肯定是找不到的
        // 子串过长, 肯定是找不到的
        *result = -1;
        return MStr_Ok;
    }
    if (pattern_cnt == 0) {
        // 空的, 永远返回true
        *result = 0;
        return MStr_Ok;
    }
    ret_code = patt_match_impl(
        &find_res,
        main_str + begin_pos,
        main_str_cnt - begin_pos,
        pattern,
        pattern_cnt
    );
    if (MSTR_SUCC(ret_code)) {
        *result = find_res;
    }
    return ret_code;
}

/**
 * @brief 子串匹配
 *
 * @param[out] res: 匹配结果
 * @param[in] mstr: 主串
 * @param[in] mstr_cnt: 主串长度
 * @param[in] patt: 模式串
 * @param[in] patt_len: 模式串长度
 */
static mstr_result_t patt_match_impl(
    isize_t* res,
    const mstr_char_t* mstr,
    usize_t mstr_cnt,
    const mstr_char_t* patt,
    usize_t patt_cnt
)
{
    uint8_t* bad_char_arr = NULL;
    usize_t match_cnt = patt_cnt - 1;
    const mstr_char_t* m_b = mstr;
    const mstr_char_t* m_p = mstr;
    const mstr_char_t* m_end = mstr + mstr_cnt;
    mstr_assert(patt_cnt > 0);
    // 默认为没有找到
    *res = -1;
    // 分配bad char数组, 并构造跳转表
    if (patt_cnt >= BM_THRESHOLD_CNT) {
        bad_char_arr = (uint8_t*)mstr_heap_alloc(
            sizeof(*bad_char_arr) * BM_CHAR_INDEX_MAX
        );
        if (bad_char_arr == NULL) {
            return MStr_Err_HeapTooSmall;
        }
        make_jump_table(bad_char_arr, patt, patt_cnt);
    }
    // 匹配子串
    while (m_p < m_end - match_cnt) {
        isize_t i = (isize_t)match_cnt;
        // 倒着来匹配
        while (i >= 0 && patt[i] == m_p[i]) {
            i -= 1;
        }
        if (i < 0) {
            // 匹配成功
            *res = (isize_t)(m_p - m_b);
            break;
        }
        else {
            // substring的第i个字符匹配失败, m_p + i处失败
            // 移动next_pos到匹配失败的字符最后一次在sub
            // string中出现的位置处
            usize_t arr_idx = (usize_t)(uint8_t)m_p[i];
            mstr_assert(arr_idx >= 0 && arr_idx <= BM_CHAR_INDEX_MAX);
            m_p += bad_char_arr ? (usize_t)bad_char_arr[arr_idx] : 1;
        }
    }
    if (bad_char_arr != NULL) {
        mstr_heap_free(bad_char_arr);
    }
    return MStr_Ok;
}

/**
 * @brief mstr_retain (start_with) 的实现
 *
 * @param[inout] str: 需要进行剔除的字符串
 * @param[in] patt: 模式串
 * @param[in] patt_cnt: 模式串的字符计数
 */
static mstr_result_t mstr_retain_start_with_impl(
    MString* str, const char* patt, usize_t patt_cnt
)
{
    usize_t patt_len = 0;
    mstr_result_t res = MStr_Ok;
    // 计算unicode长度
    MSTR_AND_THEN(res, unicode_length_of(&patt_len, patt, patt_cnt));
    // 把数据挪到前面覆盖掉
    if (MSTR_SUCC(res) && mstr_start_with(str, patt, patt_cnt)) {
        memmove(str->buff, str->buff + patt_cnt, str->count - patt_cnt);
        // 减去长度
        str->count -= patt_cnt;
        str->length -= patt_len;
    }
    return res;
}

/**
 * @brief mstr_retain (end_with) 的实现
 *
 * @param[inout] str: 需要进行剔除的字符串
 * @param[in] patt: 模式串
 * @param[in] patt_cnt: 模式串的字符计数
 */
static mstr_result_t mstr_retain_end_with_impl(
    MString* str, const char* patt, usize_t patt_cnt
)
{
    usize_t patt_len = 0;
    mstr_result_t res = MStr_Ok;
    // 计算unicode长度
    MSTR_AND_THEN(res, unicode_length_of(&patt_len, patt, patt_cnt));
    // 减去长度, 把数据截断
    if (MSTR_SUCC(res) && mstr_end_with(str, patt, patt_cnt)) {
        str->count -= patt_cnt;
        str->length -= patt_len;
    }
    return res;
}

/**
 * @brief mstr_retain (all) 的实现
 *
 * @param[inout] str: 需要进行剔除的字符串
 * @param[in] patt: 模式串
 * @param[in] patt_cnt: 模式串的字符计数
 */
static mstr_result_t mstr_retain_all_impl(
    MString* str, const char* patt, usize_t patt_cnt
)
{
    MString tmp_str;
    usize_t patt_len = 0;
    mstr_result_t res = MStr_Ok;
    mstr_init(&tmp_str);
    mstr_assert(patt_cnt > 0);
    // 计算unicode长度
    MSTR_AND_THEN(res, unicode_length_of(&patt_len, patt, patt_cnt));
    // 保留足够的长度, retain 结果一定是 小于等于 源字符串 的
    MSTR_AND_THEN(res, mstr_reserve(&tmp_str, str->count));
    // 进行处理
    MSTR_AND_THEN(
        res,
        mstr_retain_all_impl_helper(
            &tmp_str, str, patt, patt_cnt, patt_len
        )
    );
    // 把tmp_str移动到str
    if (MSTR_SUCC(res)) {
        mstr_move_from(str, &tmp_str);
    }
    return res;
}

/**
 * @brief mstr_retain (all) 的实现 (helper)
 *
 * @param[in] out_str: 剔除结果, 外部已经init, 并保留了足够的内存区
 * @param[in] src_str: 需要进行剔除的字符串
 * @param[in] patt: 模式串
 * @param[in] patt_cnt: 模式串的字符计数
 * @param[in] patt_len: 模式串的长度
 */
static mstr_result_t mstr_retain_all_impl_helper(
    MString* out_str,
    const MString* src_str,
    const char* patt,
    usize_t patt_cnt,
    usize_t patt_len
)
{
    uint8_t* jmp_table = NULL;
    usize_t match_cnt = patt_cnt - 1;
    usize_t result_cnt = src_str->count;
    usize_t result_len = src_str->length;
    mstr_char_t* m_r = out_str->buff;
    const mstr_char_t* m_p = src_str->buff;
    const mstr_char_t* m_end = src_str->buff + src_str->count;
    mstr_assert(patt_cnt > 0);
    mstr_assert(patt_len > 0);
    // 构造表
    if (patt_cnt >= BM_THRESHOLD_CNT) {
        jmp_table = (uint8_t*)mstr_heap_alloc(
            sizeof(*jmp_table) * BM_CHAR_INDEX_MAX
        );
        if (jmp_table == NULL) {
            return MStr_Err_HeapTooSmall;
        }
        make_jump_table(jmp_table, patt, patt_cnt);
    }
    // 进行剔除
    while (m_p < m_end - match_cnt) {
        isize_t i = (isize_t)match_cnt;
        // 倒着来匹配
        while (i >= 0 && patt[i] == m_p[i]) {
            i -= 1;
        }
        if (i < 0) {
            // 匹配成功, 剔除掉这个东东
            m_p += patt_cnt;
            result_cnt -= patt_cnt;
            result_len -= patt_len;
        }
        else {
            // substring的第i个字符匹配失败, m_p + i处失败
            // 移动next_pos到匹配失败的字符最后一次在sub
            // string中出现的位置处
            usize_t arr_idx = (usize_t)(uint8_t)m_p[i];
            usize_t arr_add_len = 0;
            mstr_assert(arr_idx >= 0 && arr_idx <= BM_CHAR_INDEX_MAX);
            arr_add_len = jmp_table ? (usize_t)jmp_table[arr_idx] : 1;
            // 添加本内容
            mstr_assert(
                (usize_t)(m_r - out_str->buff) < out_str->cap_size
            );
            memcpy(m_r, m_p, arr_add_len);
            m_r += arr_add_len;
            m_p += arr_add_len;
        }
    }
    // 拼接剩下的部分
    mstr_assert(
        (usize_t)((m_r - out_str->buff) + (m_end - m_p)) <
        out_str->cap_size
    );
    memcpy(m_r, m_p, (usize_t)(m_end - m_p));
    // 填充结果
    out_str->count = result_cnt;
    out_str->length = result_len;
    // 释放跳转表
    if (jmp_table != NULL) {
        mstr_heap_free(jmp_table);
    }
    return MStr_Ok;
}

/**
 * @brief 按照模式串 patt 构造跳转表
 *
 * @param[out] table: 跳转表
 * @param[in] patt: 模式串
 * @param[in] patt_cnt: 模式串的字符数
 */
static void make_jump_table(
    uint8_t* table, const mstr_char_t* patt, usize_t patt_cnt
)
{
    const usize_t BAD_CHAR_MAX_OFFSET = 255;
    mstr_assert(patt_cnt > 0);
    // 默认情况, 没有出现在sub string的字符移动substring.length长度
    for (usize_t i = 0; i < BM_CHAR_INDEX_MAX; i += 1) {
        usize_t offset = patt_cnt > BAD_CHAR_MAX_OFFSET ?
                             BAD_CHAR_MAX_OFFSET :
                             patt_cnt;
        table[i] = (uint8_t)offset;
    }
    // 不然, bad_char[ch]就是ch在sub string中最后一次出现的位置
    for (usize_t i = 0; i < patt_cnt - 1; i += 1) {
        usize_t offset = patt_cnt - i - 1;
        usize_t li_off =
            offset > BAD_CHAR_MAX_OFFSET ? BAD_CHAR_MAX_OFFSET : offset;
        usize_t ch_idx = (usize_t)(uint8_t)patt[i];
        mstr_assert(ch_idx >= 0 && ch_idx <= BM_CHAR_INDEX_MAX);
        table[ch_idx] = (uint8_t)li_off;
    }
}

/**
 * @brief 计算unicode字符串的字符长度
 *
 */
static mstr_result_t unicode_length_of(
    usize_t* len, const char* str, usize_t cnt
)
{
    usize_t str_len = 0;
    const char* str_beg = str;
    while (*str && (usize_t)(str - str_beg) < cnt) {
        usize_t cur_cnt = mstr_char_length(*str);
        if (cur_cnt == 0) {
            break;
        }
        str += cur_cnt;
        str_len += 1;
    }
    if ((usize_t)(str - str_beg) != cnt) {
        *len = 0;
        return MStr_Err_UnicodeEncodingError;
    }
    else {
        *len = str_len;
        return MStr_Ok;
    }
}
