// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_stroper.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串操作
 * @version 1.0
 * @date    2023-04-22
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

static void
    mstr_reverse_unicode_helper(mstr_char_t*, const mstr_char_t*);
static mstr_bool_t mstr_compare_helper(
    const char*, const char*, usize_t
);
static mstr_result_t mstr_find_simple(
    isize_t*, const char*, usize_t, usize_t, const char*, usize_t
);
static mstr_result_t mstr_find_large(
    isize_t*, const mstr_char_t*, usize_t, const mstr_char_t*, usize_t
);
static mstr_result_t mstr_retain_all_impl(
    MString*, const char*, usize_t
);
static mstr_result_t mstr_retain_all_impl_helper(
    MString*, const MString*, const char*, usize_t, usize_t
);
static mstr_result_t mstr_replace_start_with_impl(
    MString*, const char*, usize_t, const char*, usize_t
);
static mstr_result_t mstr_replace_end_with_impl(
    MString*, const char*, usize_t, const char*, usize_t
);
static mstr_result_t mstr_replace_all_impl(
    MString*, const char*, usize_t, const char*, usize_t
);
static mstr_result_t mstr_replace_all_impl_helper(
    MString*, const MString*, const char*, usize_t, const char*, usize_t
);
static void make_jump_table(uint8_t*, const mstr_char_t*, usize_t);

//
// Transformation:
//

MSTR_EXPORT_API(void) mstr_clear(MString* str)
{
    str->count = 0;
    str->length = 0;
}

MSTR_EXPORT_API(void) mstr_reverse_self(MString* str)
{
    mstr_char_t* pe = str->buff + str->count;
    mstr_reverse_only(str);
    mstr_reverse_unicode_helper(str->buff, pe);
}

MSTR_EXPORT_API(void) mstr_reverse_only(MString* str)
{
    mstr_char_t* p2 = str->buff + str->count - 1;
    mstr_char_t* p1 = str->buff;
    while (p1 < p2) {
        char v = *p2;
        *p2 = *p1;
        *p1 = v;
        // ++, --
        p1 += 1;
        p2 -= 1;
    }
}

/**
 * @brief 翻转Unicode字符编码, 让其正确, 在UTF-8关闭的情况下,
 * 该函数不做任何操作
 *
 * @param[inout] buff: 操作数组
 * @param[in] pe: 数组的末尾位置
 */
static void mstr_reverse_unicode_helper(
    mstr_char_t* buff, const mstr_char_t* pe
)
{
#if _MSTR_USE_UTF_8
    mstr_char_t* p1 = buff;
    mstr_char_t* p2 = buff;
    // UTF-8考虑的地方在于 <= 0x7f 是ASCII
    // 而其余情况都是UTF-8字符串
    // 那么就可以先反转每个bytes:
    // >> [char1: 110xxxxx 10xxxxxx], [char2: 110xxxxx 10xxxxxx]
    // => [char2: 10xxxxxx 110xxxxx], [char1: 10xxxxxx 110xxxxx]
    // 再去遍历一遍字符串, 跳过ascii, 并在发现lead character的时候
    // 把上次的位置到目前lead character的字符交换
    while (p2 < pe) {
        mstr_char_t ch = *p2;
        p2 += 1;
        if ((ch & 0x80) == 0) {
            // ASCII
            p1 = p2;
        }
        else if ((ch & 0xc0) == 0xc0) {
            // 2~6bytes UTF-8的lead character
            mstr_char_t* t = p2;
            usize_t byte_len = (usize_t)(p2 - p1);
            // 因为一些编译器会抱怨case后无break
            // 因此这里用goto做
            switch (byte_len) {
            case 4: goto proc_4;
            case 3: goto proc_3;
            case 2: goto proc_2;
            default: mstr_unreachable(); goto end;
            }
        proc_4:
            //  3    2    1    0
            // /|\            /|\ 4个byte长度
            //  p1             p2
            // 交换p1和p2, 然后变成下面的样子
            //  0    2    1    3
            //      /|\  /|\ 2个byte长度
            //       p1   p2
            // 交给proc_2以一样的方式交换p1和p2即可
            ch = *p1;
            *p1++ = *--p2;
            *p2 = ch;
            goto proc_2;
        proc_3:
            // str[1]和str[1]不需要交换
            goto proc_2;
        proc_2:
            ch = *p1;
            *p1++ = *--p2;
            *p2 = ch;
            goto end;
        end:
            p1 = t;
            p2 = t;
        }
    }
#else
    (void)buff;
    (void)pe;
#endif // _MSTR_USE_UTF_8
}

//
// Concatenation:
//

MSTR_EXPORT_API(mstr_result_t)
mstr_append(MString* str, mstr_codepoint_t ch)
{
    return mstr_repeat_append(str, ch, 1);
}

MSTR_EXPORT_API(mstr_result_t)
mstr_repeat_append(MString* str, mstr_codepoint_t ch, usize_t cnt)
{
    if (cnt == 0) {
        return MStr_Ok;
    }
    else {
        usize_t code_len;
        usize_t need_len;
        mstr_char_t buff[8];
        mstr_result_t result = MStr_Ok;
#if _MSTR_USE_UTF_8
        result = mstr_as_utf8(ch, buff, &code_len);
#else
        code_len = 1;
        buff[0] = (mstr_char_t)(ch & 0x7f);
#endif // _MSTR_USE_UTF_8
        need_len = code_len * cnt;
        if (str->count + need_len + 1 >= str->cap_size) {
            // 保证length < cap_size + 1
            // 且有足够的空间存放下一个字符
            MSTR_AND_THEN(
                result,
                mstr_reserve(
                    str, mstr_resize_tactic(str->cap_size, need_len)
                )
            );
        }
        if (MSTR_SUCC(result)) {
            usize_t i_cnt = 0;
            mstr_char_t* it = str->buff + str->count;
            while (i_cnt < cnt) {
                for (usize_t j = 0; j < code_len; j += 1) {
                    *it = buff[j];
                    it += 1;
                }
                i_cnt += 1;
            }
            str->count += code_len * cnt;
            str->length += cnt;
        }
        return result;
    }
}

MSTR_EXPORT_API(mstr_result_t)
mstr_concat(MString* str, const MString* other)
{
    mstr_result_t result = MStr_Ok;
    if (str->count + other->count >= str->cap_size) {
        // 且有足够的空间存放
        MSTR_AND_THEN(
            result,
            mstr_reserve(
                str, mstr_resize_tactic(str->cap_size, other->count)
            )
        );
    }
    if (MSTR_SUCC(result)) {
        char* dst = str->buff + str->count;
        usize_t cnt = other->count;
        const char* src = other->buff;
        // 复制内容
        memcpy(dst, src, cnt);
        str->count += cnt;
        str->length += other->length;
    }
    return result;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_concat_cstr(MString* str, const char* other)
{
    MString lit;
    usize_t content_len, content_cnt;
    mstr_result_t res;
    res = mstr_strlen(&content_len, &content_cnt, other, NULL);
    if (MSTR_SUCC(res)) {
        // const MString不会被修改, 所以可强转一下
        lit.buff = (char*)(iptr_t)other;
        lit.count = content_cnt;
        lit.length = content_len;
        lit.cap_size = 0;
        res = mstr_concat(str, &lit);
    }
    return res;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_concat_cstr_slice(MString* str, const char* start, const char* end)
{
    if (start == end) {
        return MStr_Ok;
    }
    else {
        MString lit;
        usize_t content_len, content_cnt;
        mstr_result_t res;
        res = mstr_strlen(&content_len, &content_cnt, start, end);
        if (MSTR_SUCC(res)) {
            // const MString不会被修改, 所以可强转一下
            lit.buff = (char*)(iptr_t)start;
            lit.count = content_cnt;
            lit.length = content_len;
            lit.cap_size = 0;
            res = mstr_concat(str, &lit);
        }
        return res;
    }
}

//
// Comparison operator
//

MSTR_EXPORT_API(mstr_bool_t)
mstr_equal(const MString* a, const MString* b)
{
    return mstr_equal_cstr(a, b->buff, b->count);
}

MSTR_EXPORT_API(mstr_bool_t)
mstr_equal_cstr(const MString* a, const mstr_char_t* b, usize_t b_cnt)
{
    if (a->count != b_cnt) {
        return False;
    }
    else {
        return mstr_compare_helper(a->buff, b, b_cnt);
    }
}

MSTR_EXPORT_API(mstr_bool_t)
mstr_start_with(
    const MString* str, const char* prefix, usize_t prefix_cnt
)
{
    usize_t len = prefix_cnt;
    if (str->count < len) {
        return False;
    }
    else {
        return mstr_compare_helper(str->buff, prefix, len);
    }
}

MSTR_EXPORT_API(mstr_bool_t)
mstr_end_with(
    const MString* str, const char* suffix, usize_t suffix_cnt
)
{
    usize_t len = suffix_cnt;
    if (str->count < len) {
        return False;
    }
    else {
        usize_t offset = str->count - len;
        return mstr_compare_helper(str->buff + offset, suffix, len);
    }
}

MSTR_EXPORT_API(mstr_bool_t)
mstr_contains(
    const MString* str, const char* pattern, usize_t pattern_cnt
)
{
    mstr_result_t match_ret;
    MStringMatchResult match_res;
    match_ret = mstr_find(str, &match_res, 0, pattern, pattern_cnt);
    if (MSTR_FAILED(match_ret)) {
        return False;
    }
    else {
        return match_res.is_matched;
    }
}

/**
 * @brief 比较相同长度len的字串a和b是否一致
 *
 * @param a: 字符串a
 * @param b: 字符串b
 * @param len: 字符串a和b的长度
 *
 * @return mstr_bool_t: 比较结果
 */
static mstr_bool_t mstr_compare_helper(
    const char* a, const char* b, usize_t len
)
{
    uint32_t bit = 0;
    for (usize_t i = 0; i < len; i += 1) {
        uint32_t ch_a = a[i];
        uint32_t ch_b = b[i];
        // equ
        bit |= ch_a ^ ch_b;
    }
    return bit == 0;
}

//
// Insert & remove
//

MSTR_EXPORT_API(mstr_result_t)
mstr_remove(MString* str, mstr_codepoint_t* removed_ch, usize_t idx)
{
    mstr_result_t res = MStr_Ok;
    // 找到需要移除的位置
    usize_t offset = mstr_char_offset_at(str, idx);
    // 记录返回的字符
    if (removed_ch != NULL) {
#if _MSTR_USE_UTF_8
        mstr_codepoint_t out_code = 0;
        res = mstr_codepoint_of(
            &out_code, str->buff + offset, str->count - offset
        );
        if (MSTR_SUCC(res)) {
            *removed_ch = out_code;
        }
        else {
            *removed_ch = 0;
        }
#else
        *removed_ch = str->buff[offset];
#endif // _MSTR_USE_UTF_8
    }
    // 移除掉该字符
    if (MSTR_SUCC(res)) {
        usize_t ccnt = mstr_char_length(*(str->buff + offset));
        usize_t rems = offset + ccnt;
        memmove(
            str->buff + offset, str->buff + rems, str->count - rems
        );
        str->count -= ccnt;
        str->length -= 1;
    }
    return res;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_insert(MString* str, usize_t idx, mstr_codepoint_t ch)
{
    if (idx == str->length) {
        // 等效为append
        // 数组末尾插入的时间复杂度是O(1)的, 因此单独分开一个case
        return mstr_append(str, ch);
    }
    else {
        mstr_result_t res = MStr_Ok;
        // 找到需要插入的位置
        usize_t offset = mstr_char_offset_at(str, idx);
        // 对需要插入的字符进行转码
        char insert_data[8];
        usize_t insert_data_len;
#if _MSTR_USE_UTF_8
        res = mstr_as_utf8(ch, insert_data, &insert_data_len);
#else
        insert_data[0] = (char)ch;
        insert_data_len = 1;
#endif // _MSTR_USE_UTF_8
       // 保证空间足够
        if (str->count + insert_data_len + 1 >= str->cap_size) {
            // 保证length < cap_size + 1
            // 且有足够的空间存放下一个字符
            MSTR_AND_THEN(
                res,
                mstr_reserve(
                    str,
                    mstr_resize_tactic(str->cap_size, insert_data_len)
                )
            );
        }
        // 插入该字符
        if (MSTR_SUCC(res)) {
            // 留出空间
            memmove(
                str->buff + offset + insert_data_len,
                str->buff + offset,
                str->count - offset
            );
            // 然后复制内容过去
            memcpy(str->buff + offset, insert_data, insert_data_len);
            str->count += insert_data_len;
            str->length += 1;
        }
        return res;
    }
}

//
// Substring matcher & replace
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
    const char* patt,
    usize_t patt_cnt,
    const char* replace_to,
    usize_t replace_to_cnt
)
{
    mstr_result_t res = MStr_Ok;
    if (patt_cnt == 0 || patt_cnt > str->count) {
        res = MStr_Ok;
    }
    else if (opt == MStringReplaceOption_StartWith) {
        res = mstr_replace_start_with_impl(
            str, patt, patt_cnt, replace_to, replace_to_cnt
        );
    }
    else if (opt == MStringReplaceOption_EndWith) {
        res = mstr_replace_end_with_impl(
            str, patt, patt_cnt, replace_to, replace_to_cnt
        );
    }
    else if (opt == MStringReplaceOption_All) {
        res = mstr_replace_all_impl(
            str, patt, patt_cnt, replace_to, replace_to_cnt
        );
    }
    else {
        mstr_unreachable();
        res = MStr_Err_NoImplemention;
    }
    return res;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_retain(
    MString* str,
    MStringReplaceOption opt,
    const char* patt,
    usize_t patt_cnt
)
{
    mstr_result_t res = MStr_Ok;
    if (patt_cnt == 0 || patt_cnt > str->count) {
        res = MStr_Ok;
    }
    else if (opt == MStringReplaceOption_StartWith) {
        res = mstr_replace_start_with_impl(str, patt, patt_cnt, "", 0);
    }
    else if (opt == MStringReplaceOption_EndWith) {
        res = mstr_replace_end_with_impl(str, patt, patt_cnt, "", 0);
    }
    else if (opt == MStringReplaceOption_All) {
        res = mstr_retain_all_impl(str, patt, patt_cnt);
    }
    else {
        mstr_unreachable();
        res = MStr_Err_NoImplemention;
    }
    return res;
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
    ret_code = mstr_find_large(
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
static mstr_result_t mstr_find_large(
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
    MSTR_AND_THEN(
        res, mstr_strlen(&patt_len, NULL, patt, patt + patt_cnt)
    );
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
 * @brief 替换(start with)的实现
 *
 * @param[inout] str: 字符串
 * @param[in] patt: 匹配模式
 * @param[in] patt_cnt: 匹配模式的字符串的字符计数
 * @param[in] rep_to: 替换到的字符串
 * @param[in] rep_to_cnt: 替换到的字符串的字符计数
 *
 */
static mstr_result_t mstr_replace_start_with_impl(
    MString* str,
    const char* patt,
    usize_t patt_cnt,
    const char* rep_to,
    usize_t rep_to_cnt
)
{
    usize_t patt_len = 0;
    usize_t replace_len = 0;
    mstr_result_t res = MStr_Ok;
    mstr_assert(patt_cnt > 0);
    mstr_assert(patt_cnt <= str->count);
    // 计算unicode长度
    MSTR_AND_THEN(
        res, mstr_strlen(&patt_len, NULL, patt, patt + patt_cnt)
    );
    MSTR_AND_THEN(
        res,
        mstr_strlen(&replace_len, NULL, rep_to, rep_to + rep_to_cnt)
    );
    // 如果开头是合适的就替换掉
    if (!mstr_start_with(str, patt, patt_cnt) || MSTR_FAILED(res)) {
        return res;
    }
    // 按照 rep_to_cnt 做个特判
    if (rep_to_cnt == 0) {
        // 把数据挪到前面覆盖掉
        memmove(str->buff, str->buff + patt_cnt, str->count - patt_cnt);
        // 减去长度
        str->count -= patt_cnt;
        str->length -= patt_len;
    }
    else {
        usize_t need_cnt = str->count + rep_to_cnt;
        // 判断够不够长, 不够就把空间弄起来
        if (need_cnt >= str->cap_size + patt_cnt) {
            res = mstr_reserve(
                str, mstr_resize_tactic(str->cap_size, need_cnt + 1)
            );
        }
        // 挪出一个空间
        memmove(
            str->buff + rep_to_cnt,
            str->buff + patt_cnt,
            str->count - patt_cnt
        );
        // 把替换结果挪进去
        memmove(str->buff, rep_to, rep_to_cnt);
        // 减去长度
        str->count = str->count - patt_cnt + rep_to_cnt;
        str->length = str->length - patt_len + replace_len;
    }
    return res;
}

/**
 * @brief 替换(end with)的实现
 *
 * @param[inout] str: 字符串
 * @param[in] patt: 匹配模式
 * @param[in] patt_cnt: 匹配模式的字符串的字符计数
 * @param[in] rep_to: 替换到的字符串
 * @param[in] rep_to_cnt: 替换到的字符串的字符计数
 *
 */
static mstr_result_t mstr_replace_end_with_impl(
    MString* str,
    const char* patt,
    usize_t patt_cnt,
    const char* rep_to,
    usize_t rep_to_cnt
)
{
    usize_t patt_len = 0;
    usize_t replace_len = 0;
    mstr_result_t res = MStr_Ok;
    mstr_assert(patt_cnt > 0);
    mstr_assert(patt_cnt <= str->count);
    // 计算unicode长度
    MSTR_AND_THEN(
        res, mstr_strlen(&patt_len, NULL, patt, patt + patt_cnt)
    );
    MSTR_AND_THEN(
        res,
        mstr_strlen(&replace_len, NULL, rep_to, rep_to + rep_to_cnt)
    );
    // 如果结尾是合适的就替换掉
    if (!mstr_end_with(str, patt, patt_cnt) || MSTR_FAILED(res)) {
        return res;
    }
    // 按照 rep_to_cnt 做个特判
    if (rep_to_cnt == 0) {
        // 减去长度, 把数据截断
        str->count -= patt_cnt;
        str->length -= patt_len;
    }
    else {
        // 不然等效为append
        usize_t need_cnt = str->count + rep_to_cnt;
        // 判断够不够长, 不够就把空间弄起来
        if (need_cnt >= str->cap_size + patt_cnt) {
            res = mstr_reserve(
                str, mstr_resize_tactic(str->cap_size, need_cnt + 1)
            );
        }
        // 把替换结果挪进去
        memmove(str->buff + str->count - patt_cnt, rep_to, rep_to_cnt);
        // 减去长度
        str->count = str->count - patt_cnt + rep_to_cnt;
        str->length = str->length - patt_len + replace_len;
    }
    return res;
}

/**
 * @brief 替换(all)的实现
 *
 * @param[inout] str: 字符串
 * @param[in] patt: 匹配模式
 * @param[in] patt_cnt: 匹配模式的字符串的字符计数
 * @param[in] rep_to: 替换到的字符串
 * @param[in] rep_to_cnt: 替换到的字符串的字符计数
 *
 */
static mstr_result_t mstr_replace_all_impl(
    MString* str,
    const char* patt,
    usize_t patt_cnt,
    const char* rep_to,
    usize_t rep_to_cnt
)
{
    MString tmp_str;
    mstr_result_t res = MStr_Ok;
    mstr_init(&tmp_str);
    mstr_assert(patt_cnt > 0);
    // 进行处理
    MSTR_AND_THEN(
        res,
        mstr_replace_all_impl_helper(
            &tmp_str, str, patt, patt_cnt, rep_to, rep_to_cnt
        )
    );
    // 把tmp_str移动到str
    if (MSTR_SUCC(res)) {
        mstr_move_from(str, &tmp_str);
    }
    return res;
}

/**
 * @brief 替换(all)的实现(helper)
 *
 * @param[out] out_str: 替换结果
 * @param[in] src_str: 源字符串
 * @param[in] patt: 匹配模式
 * @param[in] patt_cnt: 匹配模式的字符串的字符计数
 * @param[in] rep_to: 替换到的字符串
 * @param[in] rep_to_cnt: 替换到的字符串的字符计数
 *
 */
static mstr_result_t mstr_replace_all_impl_helper(
    MString* out_str,
    const MString* src_str,
    const char* patt,
    usize_t patt_cnt,
    const char* rep_to,
    usize_t rep_to_cnt
)
{
    usize_t patt_len = 0;
    usize_t replace_len = 0;
    mstr_result_t res = MStr_Ok;
    mstr_assert(patt_cnt > 0);
    mstr_assert(patt_cnt <= src_str->count);
    // 计算unicode长度
    MSTR_AND_THEN(
        res, mstr_strlen(&patt_len, NULL, patt, patt + patt_cnt)
    );
    MSTR_AND_THEN(
        res,
        mstr_strlen(&replace_len, NULL, rep_to, rep_to + rep_to_cnt)
    );
    return MStr_Err_NoImplemention;
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
