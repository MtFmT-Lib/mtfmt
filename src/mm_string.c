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

//
// public:
//

MSTR_EXPORT_API(mstr_result_t)
mstr_create(MString* str, const char* content)
{
    usize_t content_len = (content == NULL || content[0] == '\0') ?
                              0 :
                              (usize_t)strlen(content);
    if (content_len == 0) {
        str->buff = str->stack_region;
        str->length = 0;
        str->cap_size = MSTR_STACK_REGION_SIZE;
        return MStr_Ok;
    }
    else if (content_len < MSTR_STACK_REGION_SIZE) {
        str->buff = str->stack_region;
        str->length = content_len;
        str->cap_size = MSTR_STACK_REGION_SIZE;
        strcpy(str->buff, content);
        return MStr_Ok;
    }
    else {
        str->cap_size = content_len + 1 + MSTR_CAP_SIZE_STEP;
        str->buff = (char*)mstr_heap_alloc(str->cap_size);
        if (str->buff == NULL) {
            // 内存分配失败
            return MStr_Err_HeapTooSmall;
        }
        str->length = content_len;
        strcpy(str->buff, content);
        return MStr_Ok;
    }
}

MSTR_EXPORT_API(void) mstr_clear(MString* str)
{
    str->length = 0;
}

MSTR_EXPORT_API(mstr_result_t) mstr_append(MString* str, char ch)
{
    return mstr_repeat_append(str, ch, 1);
}

MSTR_EXPORT_API(mstr_result_t)
mstr_repeat_append(MString* str, char ch, usize_t cnt)
{
    if (cnt == 0) {
        return MStr_Ok;
    }
    // else:
    mstr_result_t result = MStr_Ok;
    if (str->length + cnt + 1 >= str->cap_size) {
        // 保证length < cap_size + 1
        // 且有足够的空间存放下一个字符
        result = MSTR_AND_THEN(
            result,
            mstr_expand_size(str, str->cap_size + MSTR_CAP_SIZE_STEP)
        );
    }
    if (MSTR_SUCC(result)) {
        for (usize_t i = 0; i < cnt; i += 1) {
            str->buff[str->length + i] = ch;
        }
        str->length += cnt;
    }
    return result;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_concat(MString* str, const MString* other)
{
    mstr_result_t result = MStr_Ok;
    if (str->length + other->length >= str->cap_size) {
        // 且有足够的空间存放
        result = MSTR_AND_THEN(
            result,
            mstr_expand_size(
                str, str->cap_size + other->length + MSTR_CAP_SIZE_STEP
            )
        );
    }
    if (MSTR_SUCC(result)) {
        char* dst = str->buff + str->length;
        usize_t len = other->length;
        const char* src = other->buff;
        // 复制内容
        memcpy(dst, src, len);
        str->length += len;
    }
    return result;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_concat_cstr(MString* str, const char* other)
{
    MString lit;
    // const MString不会被修改, 所以可强转一下
    lit.buff = (char*)other;
    lit.length = (usize_t)strlen(other);
    lit.cap_size = 0;
    return mstr_concat(str, &lit);
}

MSTR_EXPORT_API(mstr_result_t)
mstr_concat_cstr_slice(MString* str, const char* start, const char* end)
{
    MString lit;
    // const MString不会被修改, 所以可强转一下
    lit.buff = (char*)start;
    lit.length = (usize_t)(end - start);
    lit.cap_size = 0;
    return mstr_concat(str, &lit);
}

MSTR_EXPORT_API(mstr_result_t) mstr_reverse_self(MString* str)
{
    char* pend = str->buff + str->length - 1;
    char* pbeg = str->buff;
    while (pbeg < pend) {
        char v = *pend;
        *pend = *pbeg;
        *pbeg = v;
        // ++, --
        pbeg += 1;
        pend -= 1;
    }
    return MStr_Ok;
}

MSTR_EXPORT_API(const char*) mstr_as_cstr(MString* str)
{
    // 在length的地方补上0
    // 因为cap_size至少比length大1, 因此不需要担心内存问题
    str->buff[str->length] = 0;
    return str->buff;
}

MSTR_EXPORT_API(bool_t) mstr_equal(const MString* a, const MString* b)
{
    if (a->length != b->length) {
        return False;
    }
    u32_t bit = 0;
    usize_t len = a->length;
    for (usize_t i = 0; i < len; i += 1) {
        u32_t ch_a = a->buff[i];
        u32_t ch_b = b->buff[i];
        // equ
        bit |= ch_a - ch_b;
    }
    return bit == 0;
}

MSTR_EXPORT_API(void) mstr_free(MString* str)
{
    if (str->buff != str->stack_region) {
        mstr_heap_free(str->buff);
    }
    // else: stack上分配的, 不用管它
    str->buff = NULL;
    str->length = 0;
    str->cap_size = 0;
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
            str->length,
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
