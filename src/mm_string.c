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
 * @brief 每次分配的步进大小
 *
 */
#define MSTR_CAP_SIZE_STEP 16

//
// private:
//
static void* mstr_realloc(void*, bool_t, usize_t, usize_t);
static mstr_result_t mstr_expand_size(MString*, usize_t);
static mstr_result_t
    mstr_strlen(usize_t*, usize_t*, const mstr_char_t*, const mstr_char_t*);

//
// public:
//

MSTR_EXPORT_API(mstr_result_t)
mstr_create(MString* str, const char* content)
{
    usize_t content_len, content_cnt;
    if (content == NULL || content[0] == '\0') {
        content_len = 0;
        content_cnt = 0;
    }
    else {
        mstr_strlen(&content_len, &content_cnt, content, NULL);
    }
    if (content_cnt == 0) {
        str->buff = str->stack_region;
        str->count = 0;
        str->length = 0;
        str->cap_size = MSTR_STACK_REGION_SIZE;
        return MStr_Ok;
    }
    else if (content_cnt < MSTR_STACK_REGION_SIZE) {
        str->buff = str->stack_region;
        str->count = content_cnt;
        str->length = content_len;
        str->cap_size = MSTR_STACK_REGION_SIZE;
        strcpy(str->buff, content);
        return MStr_Ok;
    }
    else {
        str->cap_size = content_cnt + 1 + MSTR_CAP_SIZE_STEP;
        str->buff = (char*)mstr_heap_alloc(str->cap_size);
        if (str->buff == NULL) {
            // 内存分配失败
            return MStr_Err_HeapTooSmall;
        }
        str->count = content_cnt;
        str->length = content_len;
        memcpy(str->buff, content, content_cnt);
        return MStr_Ok;
    }
}

MSTR_EXPORT_API(void)
mstr_move_create(MString* str, MString* other)
{
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
mstr_copy_create(MString* str, const MString* other)
{
    mstr_result_t result = MStr_Ok;
    MSTR_AND_THEN(result, mstr_create(str, ""));
    MSTR_AND_THEN(result, mstr_concat(str, other));
    return result;
}

MSTR_EXPORT_API(mstr_codepoint_t)
mstr_char_at(const MString* str, usize_t idx)
{
#if _MSTR_USE_UTF_8
    usize_t cur_idx = 0;
    usize_t rem_bytes = 0;
    mstr_result_t res = MStr_Ok;
    mstr_codepoint_t out_code = 0;
    const char* it = str->buff;
    mstr_bounding_check(idx < str->length);
    while (cur_idx != idx) {
        usize_t cnt = mstr_char_length(*it);
        it += cnt;
        cur_idx += 1;
    }
    rem_bytes = str->count - (usize_t)(it - str->buff);
    res = mstr_codepoint_of(&out_code, it, rem_bytes);
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
                    str, str->cap_size + need_len + MSTR_CAP_SIZE_STEP
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
                str, str->cap_size + other->count + MSTR_CAP_SIZE_STEP
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
#if _MSTR_USE_UTF_8
    // UTF-8考虑的地方在于 <= 0x7f 是ASCII
    // 而其余情况都是UTF-8字符串
    // 那么就可以先反转每个bytes:
    // >> [char1: 110xxxxx 10xxxxxx], [char2: 110xxxxx 10xxxxxx]
    // => [char2: 10xxxxxx 110xxxxx], [char1: 10xxxxxx 110xxxxx]
    // 再去遍历一遍字符串, 跳过ascii, 并在发现lead character的时候
    // 把上次的位置到目前lead character的字符交换
    p1 = str->buff;
    p2 = str->buff;
    while (*p2) {
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
#endif // _MSTR_USE_UTF_8
}

MSTR_EXPORT_API(const char*) mstr_as_cstr(MString* str)
{
    // 在length的地方补上0
    // 因为cap_size至少比length大1, 因此不需要担心内存问题
    str->buff[str->count] = 0;
    return str->buff;
}

MSTR_EXPORT_API(bool_t) mstr_equal(const MString* a, const MString* b)
{
    if (a->count != b->count) {
        return False;
    }
    else {
        uint32_t bit = 0;
        usize_t len = a->count;
        for (usize_t i = 0; i < len; i += 1) {
            uint32_t ch_a = a->buff[i];
            uint32_t ch_b = b->buff[i];
            // equ
            bit |= ch_a - ch_b;
        }
        return bit == 0;
    }
}

MSTR_EXPORT_API(mstr_result_t)
mstr_remove(MString* str, mstr_codepoint_t* removed_ch, usize_t idx)
{
    return MStr_Err_NoImplemention;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_insert(MString* str, mstr_codepoint_t ch, usize_t idx)
{
    return MStr_Err_NoImplemention;
}

MSTR_EXPORT_API(void) mstr_iter(MStringIter* it, const MString* str)
{
    it->it = str->buff;
    it->it_end = str->buff + str->count;
    it->rem_length = str->length;
}

MSTR_EXPORT_API(void) mstr_iter_mut(MStringIterMut* it, MString* str)
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
            res = MStr_Err_UnicodeEncodingNotCompleted;
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
            res = MStr_Err_UnicodeEncodingNotCompleted;
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
            res = MStr_Err_UnicodeEncodingNotCompleted;
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
            res = MStr_Err_UnicodeEncodingNotCompleted;
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
mstr_as_utf8(mstr_codepoint_t, mstr_char_t*, usize_t*)
{
    return MStr_Err_NoImplemention;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_codepoint_of(mstr_codepoint_t*, const mstr_char_t*, usize_t)
{
    return MStr_Err_NoImplemention;
}
#endif // _MSTR_USE_UTF_8

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
        return MStr_Err_UnicodeEncodingNotCompleted;
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
 * @brief 扩展str的size
 *
 * @param[inout] str: 需要调整大小的str
 * @param[in] new_size: 调整到的新的大小
 * @return mstr_result_t: 调整结果
 */
static mstr_result_t mstr_expand_size(MString* str, usize_t new_size)
{
    if (new_size < MSTR_CAP_SIZE_STEP) {
        return MStr_Ok;
    }
    else if (new_size > str->cap_size) {
        char* new_ptr = (char*)mstr_realloc(
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
static void* mstr_realloc(
    void* old_ptr, bool_t is_stack, usize_t old_size, usize_t new_size
)
{
#if _MSTR_USE_MALLOC
    void* new_ptr;
    if (!is_stack) {
        new_ptr = realloc(old_ptr, new_size);
    }
    else {
        new_ptr = mstr_heap_alloc(new_size);
        if (new_ptr != NULL) {
            memcpy(new_ptr, old_ptr, old_size);
        }
    }
#else
    void* new_ptr = mstr_heap_alloc(new_size);
    if (new_ptr == NULL) {
        return NULL;
    }
    memcpy(new_ptr, old_ptr, old_size);
    if (!is_stack) {
        mstr_heap_free(old_ptr);
    }
#endif // _MSTR_USE_MALLOC
    return new_ptr;
}
