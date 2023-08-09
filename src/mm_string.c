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

static void* mstr_string_realloc(void*, mstr_bool_t, usize_t, usize_t);

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
    str->count = other->count;
    str->length = other->length;
    if (other->buff == other->stack_region) {
        str->buff = str->stack_region;
        str->cap_size = MSTR_STACK_REGION_SIZE;
        // 复制stack上的内容
        memcpy(str->buff, other->buff, other->count);
    }
    else {
        str->buff = other->buff;
        str->cap_size = other->cap_size;
    }
    other->buff = NULL;
    other->count = 0;
    other->length = 0;
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

//
// Allocator
//

MSTR_EXPORT_API(mstr_result_t)
mstr_reserve(MString* str, usize_t new_size)
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

MSTR_EXPORT_API(usize_t)
mstr_resize_tactic(usize_t old_sz, usize_t inc_len)
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

/**
 * @brief 重新分配内存区
 *
 * @param[in] old_ptr: 以前的内存区位置
 * @param[in] is_stack: 是否为stack上分配内存
 * @param[in] old_size: 原本的内存区大小
 * @param[in] new_size: 需要分配的大小
 *
 * @return void*: 新分配的地址, 失败返回NULL
 *
 * @attention 仅供内部使用
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

//
// Indexing operators
//

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

//
// Unicode
//

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

//
// Others
//

MSTR_EXPORT_API(const char*) mstr_c_str(MString* str)
{
    // 在length的地方补上0
    // 因为cap_size至少比length大1, 因此不需要担心内存问题
    str->buff[str->count] = 0;
    return str->buff;
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

MSTR_EXPORT_API(mstr_result_t)
mstr_strlen(
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
    if (len) {
        *len = len_val;
    }
    if (count) {
        *count = count_val;
    }
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
    if (len) {
        *len = len_val;
    }
    if (count) {
        *count = count_val;
    }
    return MStr_Ok;
#endif // _MSTR_USE_UTF_8
}
