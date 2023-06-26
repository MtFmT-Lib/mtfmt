// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_match.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串查找操作
 * @version 1.0
 * @date    2023-06-22
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
#define BM_THRESHOLD_CNT  12

/**
 * @brief 单个char的最大值(unsigned)
 *
 */
#define BM_CHAR_INDEX_MAX 256

//
// private:
//

static mstr_result_t patt_match_naive(
    MStringMatchResult*,
    const mstr_char_t*,
    usize_t,
    const mstr_char_t*,
    usize_t
);
static mstr_result_t patt_match_large(
    MStringMatchResult*,
    const mstr_char_t*,
    usize_t,
    const mstr_char_t*,
    usize_t
);

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
    if (begin_pos > str->length) {
        // 超过主串的长度了, 肯定是找不到的
        f_res->is_matched = false;
        f_res->begin_pos = 0;
        f_res->begin_offset = 0;
        return MStr_Ok;
    }
    else if (pattern_cnt > str->count) {
        // 子串过长, 肯定是找不到的
        f_res->is_matched = false;
        f_res->begin_pos = 0;
        f_res->begin_offset = 0;
        return MStr_Ok;
    }
    else if (pattern_cnt == 0) {
        // 空的, 永远返回true
        f_res->begin_pos = 0;
        f_res->begin_offset = 0;
        f_res->is_matched = True;
        return MStr_Ok;
    }
    else if (begin_pos == 0 && pattern_cnt == str->count) {
        // 退化为比较
        f_res->is_matched = mstr_start_with(str, pattern, pattern_cnt);
        f_res->begin_pos = 0;
        f_res->begin_offset = 0;
        return MStr_Ok;
    }
    else {
        mstr_result_t res;
        MStringMatchResult find_res;
        usize_t offset = mstr_char_offset_at(str, begin_pos);
        if (pattern_cnt < BM_THRESHOLD_CNT) {
            res = patt_match_naive(
                &find_res,
                str->buff + offset,
                str->count - offset,
                pattern,
                pattern_cnt
            );
        }
        else {
            res = patt_match_large(
                &find_res,
                str->buff + offset,
                str->count - offset,
                pattern,
                pattern_cnt
            );
        }
        if (MSTR_SUCC(res)) {
            f_res->is_matched = find_res.is_matched;
            f_res->begin_pos =
                f_res->is_matched ? find_res.begin_pos : 0;
            f_res->begin_offset =
                f_res->is_matched ? find_res.begin_offset : 0;
        }
        return res;
    }
}

/**
 * @brief 暴力方式的子串匹配
 *
 * @param[out] res: 匹配结果
 * @param[in] mstr: 主串
 * @param[in] mstr_cnt: 主串长度
 * @param[in] patt: 模式串
 * @param[in] patt_len: 模式串长度
 */
static mstr_result_t patt_match_naive(
    MStringMatchResult* res,
    const mstr_char_t* mstr,
    usize_t mstr_cnt,
    const mstr_char_t* patt,
    usize_t patt_cnt
)
{
    usize_t offset = 0;
    usize_t match_cnt = patt_cnt - 1;
    const mstr_char_t* m_b = mstr;
    const mstr_char_t* m_p = mstr;
    const mstr_char_t* m_end = mstr + mstr_cnt;
    mstr_assert(patt_cnt > 0);
    // 默认为没有找到
    res->begin_pos = 0;
    res->begin_offset = 0;
    res->is_matched = False;
    // 匹配子串
    while (m_p < m_end - match_cnt) {
        isize_t i = (isize_t)match_cnt;
        // 倒着来匹配
        while (i >= 0 && patt[i] == m_p[i]) {
            i -= 1;
        }
        if (i < 0) {
            // 匹配成功
            res->begin_pos = offset;
            res->begin_offset = (usize_t)(m_p - m_b);
            res->is_matched = True;
            break;
        }
        else {
            // 匹配失败, 跳过1个字符的距离
            offset += 1;
            m_p += mstr_char_length(*m_p);
        }
    }
    return MStr_Ok;
}

/**
 * @brief 基于Boyer-Moore实现的字符串匹配
 *
 * @param[out] res: 匹配结果
 * @param[in] mstr: 主串
 * @param[in] mstr_len: 主串长度
 * @param[in] patt: 模式串
 * @param[in] patt_cnt: 模式串长度
 */
static mstr_result_t patt_match_large(
    MStringMatchResult* res,
    const mstr_char_t* mstr,
    usize_t mstr_cnt,
    const mstr_char_t* patt,
    usize_t patt_cnt
)
{
    uint8_t* bad_char_arr;
    usize_t match_cnt = patt_cnt - 1;
    const mstr_char_t* m_b = mstr;
    const mstr_char_t* m_p = mstr;
    const mstr_char_t* m_end = mstr + mstr_cnt;
    const usize_t BAD_CHAR_MAX_OFFSET = 255;
    mstr_assert(patt_cnt > 0);
    // 默认为没有找到
    res->begin_pos = 0;
    res->is_matched = False;
    // 分配bad char数组
    bad_char_arr = (uint8_t*)mstr_heap_alloc(
        sizeof(*bad_char_arr) * BM_CHAR_INDEX_MAX
    );
    if (bad_char_arr == NULL) {
        return MStr_Err_HeapTooSmall;
    }
    // 默认情况, 没有出现在sub string的字符移动substring.length长度
    for (usize_t i = 0; i < BM_CHAR_INDEX_MAX; i += 1) {
        usize_t offset = patt_cnt > BAD_CHAR_MAX_OFFSET ?
                             BAD_CHAR_MAX_OFFSET :
                             patt_cnt;
        bad_char_arr[i] = (uint8_t)offset;
    }
    // 不然, bad_char[ch]就是ch在sub string中最后一次出现的位置
    for (usize_t i = 0; i < patt_cnt - 1; i += 1) {
        usize_t offset = patt_cnt - i - 1;
        usize_t li_off =
            offset > BAD_CHAR_MAX_OFFSET ? BAD_CHAR_MAX_OFFSET : offset;
        usize_t ch_idx = (usize_t)(uint8_t)patt[i];
        mstr_assert(ch_idx >= 0 && ch_idx <= BM_CHAR_INDEX_MAX);
        bad_char_arr[ch_idx] = (uint8_t)li_off;
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
            usize_t ch_cnt = 0;
            res->is_matched = True;
            res->begin_offset = (usize_t)(m_p - m_b);
            // 计算跨过了多少utf-8的字符串
            while (m_b < m_p) {
                usize_t cnt = mstr_char_length(*m_b);
                ch_cnt += 1;
                m_b += cnt;
            }
            res->begin_pos = ch_cnt;
            break;
        }
        else {
            // substring的第i个字符匹配失败, m_p + i处失败
            // 移动next_pos到匹配失败的字符最后一次在sub
            // string中出现的位置处
            usize_t arr_idx = (usize_t)(uint8_t)m_p[i];
            mstr_assert(arr_idx >= 0 && arr_idx <= BM_CHAR_INDEX_MAX);
            m_p += (usize_t)bad_char_arr[arr_idx];
        }
    }
    mstr_heap_free(bad_char_arr);
    return MStr_Ok;
}
