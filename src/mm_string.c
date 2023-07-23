// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_string.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串
 * @version 1.0
 * @date    2023-04-22
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */

#define MSTR_IMP_SOURCES 1

#include "mm_string.h"
#include "mm_heap.h"
#include <stddef.h>
#include <string.h>

/**
 * @brief 进行扩展的阈值
 *
 */
#define MSTR_SIZE_EXPAND_MAX          1024

/**
 * @brief 超过X2阈值的情况下每次增大的大小
 *
 */
#define MSTR_SIZE_LARGE_CAP_SIZE_STEP 512

//
// private:
//

static mstr_bool_t mstr_compare_helper(
    const char*, const char*, usize_t
);
static void
    mstr_reverse_unicode_helper(mstr_char_t*, const mstr_char_t*);
static void* mstr_string_realloc(void*, mstr_bool_t, usize_t, usize_t);
static mstr_result_t mstr_expand_size(MString*, usize_t);
static usize_t mstr_resize_tactic(usize_t, usize_t);
static mstr_result_t
    mstr_strlen(usize_t*, usize_t*, const mstr_char_t*, const mstr_char_t*);
//
// public:
//

MSTR_EXPORT_API(mstr_result_t)
mstr_create(MString* str, const char* content)
{
    if (content == NULL || content[0] == '\0') {
        mstr_init(str);
        return MStr_Ok;
    }
    else {
        usize_t content_len, content_cnt;
        mstr_strlen(&content_len, &content_cnt, content, NULL);
        str->count = content_cnt;
        str->length = content_len;
        if (content_cnt == 0) {
            // str->buff =...;
            // str->cap_size = ...;
            return MStr_Ok;
        }
        else if (content_cnt < MSTR_STACK_REGION_SIZE) {
            str->buff = str->stack_region;
            str->cap_size = MSTR_STACK_REGION_SIZE;
            strcpy(str->buff, content);
            return MStr_Ok;
        }
        else {
            str->cap_size = content_cnt + MSTR_STACK_REGION_SIZE;
            str->buff = (char*)mstr_heap_alloc(str->cap_size);
            if (str->buff == NULL) {
                // 内存分配失败
                return MStr_Err_HeapTooSmall;
            }
            memcpy(str->buff, content, content_cnt);
            return MStr_Ok;
        }
    }
}

MSTR_EXPORT_API(void)
mstr_move_from(MString* str, MString* other)
{
    if (str->buff != NULL) {
        mstr_free(str);
    }
    if (other->buff == other->stack_region) {
        str->buff = str->stack_region;
        str->count = other->count;
        str->cap_size = MSTR_STACK_REGION_SIZE;
        // 复制stack上的内容
        memcpy(str->buff, other->buff, other->count);
    }
    else {
        str->buff = other->buff;
        str->count = other->count;
        str->cap_size = other->cap_size;
    }
    other->buff = NULL;
    other->count = 0;
    other->cap_size = 0;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_copy_from(MString* str, const MString* other)
{
    mstr_result_t result = MStr_Ok;
    if (str->buff != NULL) {
        mstr_free(str);
    }
    MSTR_AND_THEN(result, mstr_create(str, ""));
    MSTR_AND_THEN(result, mstr_concat(str, other));
    return result;
}

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
                mstr_expand_size(
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
            mstr_expand_size(
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

MSTR_EXPORT_API(const char*) mstr_c_str(MString* str)
{
    // 在length的地方补上0
    // 因为cap_size至少比length大1, 因此不需要担心内存问题
    str->buff[str->count] = 0;
    return str->buff;
}

MSTR_EXPORT_API(mstr_bool_t)
mstr_equal(const MString* a, const MString* b)
{
    if (a->count != b->count) {
        return False;
    }
    else {
        usize_t len = a->count;
        return mstr_compare_helper(a->buff, b->buff, len);
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

MSTR_EXPORT_API(mstr_codepoint_t)
mstr_char_at(const MString* str, usize_t idx)
{
#if _MSTR_USE_UTF_8
    usize_t offset;
    mstr_result_t res = MStr_Ok;
    mstr_codepoint_t out_code = 0;
    mstr_bounding_check(idx < str->length);
    offset = mstr_char_offset_at(str, idx);
    res = mstr_codepoint_of(
        &out_code, str->buff + offset, str->count - offset
    );
    if (MSTR_SUCC(res)) {
        return out_code;
    }
    else {
        return 0;
    }
#else
    mstr_bounding_check(idx < str->length);
    return (mstr_codepoint_t)str->buff[idx];
#endif // _MSTR_USE_UTF_8
}

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
                mstr_expand_size(
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

MSTR_EXPORT_API(void) mstr_iter(MStringIter* it, const MString* str)
{
    it->it = str->buff;
    it->it_end = str->buff + str->count;
    it->rem_length = str->length;
}

MSTR_EXPORT_API(usize_t) mstr_char_length(char lead)
{
#if _MSTR_USE_UTF_8
    // 构造的小欧拉图会帮忙计算utf-8编码的长度
    const uint32_t map[8] = {1, 7, 2, 6, 0, 3, 4, 5};
    uint8_t uch = (uint8_t)lead;
    uint32_t v = (uint32_t)(uch ^ 0xff);
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v += 1;
    return map[((v * 232) >> 8) & 0x7];
#else
    (void)lead;
    return 1;
#endif // _MSTR_USE_UTF_8
}

MSTR_EXPORT_API(usize_t)
mstr_lead_char_offset(const mstr_char_t* buff, usize_t hist_len)
{
#if _MSTR_USE_UTF_8
    usize_t find_len = hist_len > 6 ? 6 : hist_len;
    const mstr_char_t* it = buff;
    while (find_len > 0) {
        mstr_char_t ch = *it;
        if ((ch & 0x80) == 0) {
            // ASCII
            it -= 1;
            break;
        }
        else if ((ch & 0xc0) == 0xc0) {
            // utf8前导字符
            it -= 1;
            break;
        }
        else {
            // utf8内容
            it -= 1;
            find_len -= 1;
        }
    }
    return (usize_t)(buff - it);
#else
    (void)buff;
    (void)hist_len;
    return 1;
#endif // _MSTR_USE_UTF_8
}

#if _MSTR_USE_UTF_8

MSTR_EXPORT_API(mstr_result_t)
mstr_as_utf8(
    mstr_codepoint_t code, mstr_char_t* result, usize_t* result_len
)
{
    if (code <= 0x7f) {
        // 0xxxxxxx, 1bytes
        result[0] = (mstr_char_t)(code & 0x7f);
        *result_len = 1;
        return MStr_Ok;
    }
    else if (code <= 0x7ff) {
        // 110xxxxx 10xxxxxx, 2bytes
        result[0] = (mstr_char_t)((code >> 6) & 0x1f) | 0xc0;
        result[1] = (mstr_char_t)(code & 0x3f) | 0x80;
        *result_len = 2;
        return MStr_Ok;
    }
    else if (code <= 0xffff) {
        // 1110xxxx 10xxxxxx 10xxxxxx, 3bytes
        result[0] = (mstr_char_t)((code >> 12) & 0xf) | 0xe0;
        result[1] = (mstr_char_t)((code >> 6) & 0x3f) | 0x80;
        result[2] = (mstr_char_t)(code & 0x3f) | 0x80;
        *result_len = 3;
        return MStr_Ok;
    }
    else if (code <= 0x1fffff) {
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx, 4bytes
        result[0] = (mstr_char_t)((code >> 18) & 0x7) | 0xf0;
        result[1] = (mstr_char_t)((code >> 12) & 0x3f) | 0x80;
        result[2] = (mstr_char_t)((code >> 6) & 0x3f) | 0x80;
        result[3] = (mstr_char_t)(code & 0x3f) | 0x80;
        *result_len = 4;
        return MStr_Ok;
    }
    else {
        *result_len = 0;
        return MStr_Err_UnicodeEncodingError;
    }
}

MSTR_EXPORT_API(mstr_result_t)
mstr_codepoint_of(
    mstr_codepoint_t* code, const mstr_char_t* ch, usize_t byte_count
)
{
    uint32_t val = 0;
    mstr_result_t res;
    uint8_t lead_char = (uint8_t)ch[0];
    if (lead_char <= 0x7f) {
        // 0xxxxxxx, 1bytes
        if (byte_count >= 1) {
            val |= (uint32_t)(ch[0] & 0x7f);
            res = MStr_Ok;
        }
        else {
            res = MStr_Err_EncodingNotCompleted;
        }
    }
    else if (lead_char <= 0xdf) {
        // 110xxxxx 10xxxxxx, 2bytes
        if (byte_count >= 2) {
            val |= (uint32_t)(ch[1]) & 0x3f;
            val |= ((uint32_t)(ch[0]) & 0x1f) << 6;
            res = MStr_Ok;
        }
        else {
            res = MStr_Err_EncodingNotCompleted;
        }
    }
    else if (lead_char <= 0xef) {
        // 1110xxxx 10xxxxxx 10xxxxxx, 3bytes
        if (byte_count >= 3) {
            val |= (uint32_t)(ch[2]) & 0x3f;
            val |= ((uint32_t)(ch[1]) & 0x3f) << 6;
            val |= ((uint32_t)(ch[0]) & 0x0f) << (6 + 6);
            res = MStr_Ok;
        }
        else {
            res = MStr_Err_EncodingNotCompleted;
        }
    }
    else if (lead_char <= 0xf7) {
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx, 4 bytes
        if (byte_count >= 4) {
            val |= (uint32_t)(ch[3]) & 0x3f;
            val |= ((uint32_t)(ch[2]) & 0x3f) << 6;
            val |= ((uint32_t)(ch[1]) & 0x3f) << (6 + 6);
            val |= ((uint32_t)(ch[0]) & 0x07) << (6 + 6 + 6);
            res = MStr_Ok;
        }
        else {
            res = MStr_Err_EncodingNotCompleted;
        }
    }
    else {
        // 再长的就不处理了 (￣▽￣)"
        res = MStr_Err_UnicodeEncodingError;
    }
    if (MSTR_SUCC(res)) {
        *code = val;
    }
    return res;
}
#else

MSTR_EXPORT_API(mstr_result_t)
mstr_as_utf8(
    mstr_codepoint_t code, mstr_char_t* result, usize_t* result_len
)
{
    (void)code;
    (void)result;
    (void)result_len;
    return MStr_Err_NoImplemention;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_codepoint_of(
    mstr_codepoint_t* code, const mstr_char_t* ch, usize_t byte_count
)
{
    (void)code;
    (void)ch;
    (void)byte_count;
    return MStr_Err_NoImplemention;
}
#endif // _MSTR_USE_UTF_8

MSTR_EXPORT_API(usize_t)
mstr_char_offset_at(const MString* str, usize_t idx)
{
#if _MSTR_USE_UTF_8
    usize_t cur_idx = 0;
    const char* it = str->buff;
    mstr_bounding_check(idx < str->length);
    while (cur_idx != idx) {
        usize_t cnt = mstr_char_length(*it);
        it += cnt;
        cur_idx += 1;
    }
    return (usize_t)(it - str->buff);
#else
    mstr_bounding_check(idx < str->length);
    return idx;
#endif // _MSTR_USE_UTF_8
}

MSTR_EXPORT_API(void) mstr_free(MString* str)
{
    if (str->buff != NULL && str->buff != str->stack_region) {
        mstr_heap_free(str->buff);
    }
    // else: stack上分配的, 不用管它
    str->buff = NULL;
    str->count = 0;
    str->cap_size = 0;
}

/**
 * @brief 计算字符串长度
 *
 * @param len: 字符长度
 * @param count: 字符串占用的字节数
 * @param str: 字符串
 * @param str_end: 字符串结束, 为NULL表示'\0'自己算
 */
static mstr_result_t mstr_strlen(
    usize_t* len,
    usize_t* count,
    const mstr_char_t* str,
    const mstr_char_t* str_end
)
{
    usize_t len_val = 0, count_val = 0;
    if (str_end == NULL) {
        // 给一个大大的值让str < str_end恒为true
        str_end = (const mstr_char_t*)(uptr_t)(-1);
    }
    if (str == NULL) {
        return 0;
    }
#if _MSTR_USE_UTF_8
    while (*str && str < str_end) {
        usize_t cnt = mstr_char_length(*str);
        count_val += cnt;
        len_val += 1;
        str += cnt;
    }
    *len = len_val;
    *count = count_val;
    if (str > str_end) {
        // 正常情况应该刚好相等
        // 不然说明原字符串不完整
        return MStr_Err_EncodingNotCompleted;
    }
    else {
        return MStr_Ok;
    }
#else
    while (*str && str < str_end) {
        count_val += 1;
        len_val += 1;
        str += 1;
    }
    *len = len_val;
    *count = count_val;
    return MStr_Ok;
#endif // _MSTR_USE_UTF_8
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

/**
 * @brief 扩展str的size
 *
 * @param[inout] str: 需要调整大小的str
 * @param[in] new_size: 调整到的新的大小
 * @return mstr_result_t: 调整结果
 */
static mstr_result_t mstr_expand_size(MString* str, usize_t new_size)
{
    if (new_size > str->cap_size) {
        char* new_ptr = (char*)mstr_string_realloc(
            str->buff,
            str->buff == str->stack_region,
            str->count,
            new_size
        );
        if (new_ptr == NULL) {
            // 分配失败
            return MStr_Err_HeapTooSmall;
        }
        str->buff = new_ptr;
        str->cap_size = new_size;
        return MStr_Ok;
    }
    else {
        return MStr_Ok;
    }
}

/**
 * @brief 重新分配内存区
 *
 * @param[in] old_ptr: 以前的内存区位置
 * @param[in] is_stack: 是否为stack上分配内存
 * @param[in] old_size: 原本的内存区大小
 * @param[in] new_size: 需要分配的大小
 * @return void*: 新分配的地址, 失败返回NULL
 */
static void* mstr_string_realloc(
    void* old_ptr,
    mstr_bool_t is_stack,
    usize_t old_size,
    usize_t new_size
)
{
    if (is_stack) {
        void* new_ptr = mstr_heap_alloc(new_size);
        if (new_ptr == NULL) {
            return NULL;
        }
        memcpy(new_ptr, old_ptr, old_size);
        return new_ptr;
    }
    else {
        return mstr_heap_realloc(old_ptr, new_size, old_size);
    }
}

/**
 * @brief 改变cap的策略
 *
 * @param[in] old_sz: 以前的大小
 * @param[in] inc_len: 至少需要增加的大小, 会在此基础上增加1
 */
static usize_t mstr_resize_tactic(usize_t old_sz, usize_t inc_len)
{
    usize_t min_sz = old_sz + inc_len + 1;
    if (old_sz < MSTR_SIZE_EXPAND_MAX) {
        usize_t exp_sz = old_sz * 2;
        return min_sz < exp_sz ? exp_sz : min_sz;
    }
    else {
        return old_sz + inc_len + MSTR_SIZE_LARGE_CAP_SIZE_STEP;
    }
}
