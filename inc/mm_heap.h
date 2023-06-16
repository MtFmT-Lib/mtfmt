// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_heap.h
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   堆内存管理
 * @version 1.0
 * @date    2023-04-22
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#if !defined(_INCLUDE_MM_HEAP_H_)
#define _INCLUDE_MM_HEAP_H_ 1
#include "mm_cfg.h"
#include "mm_typedef.h"

#if _MSTR_USE_MALLOC
#define mstr_heap_alloc(s) (malloc((s)))

#define mstr_heap_free(m) \
    do {                  \
        free(m);          \
        (m) = NULL;       \
    } while (0)

#define mstr_heap_init(mem, leng) ((void)mem, (void)leng)
#else
/**
 * @brief 初始化堆分配器
 *
 * @param[in] heap_memory: 堆内存区
 * @param[in] heap_size: 堆内存区的大小
 */
MSTR_EXPORT_API(void)
mstr_heap_init_sym(iptr_t heap_memory, usize_t heap_size);

/**
 * @brief 尝试从堆中分配size大小的内存
 *
 * @return void*: 分配结果, 如果分配失败返回NULL
 */
MSTR_EXPORT_API(void*)
mstr_heap_allocate_sym(usize_t size, usize_t align);

/**
 * @brief 释放由 mstr_heap_allocate 分配的内存
 *
 */
MSTR_EXPORT_API(void) mstr_heap_free_sym(void* memory);

/**
 * @brief 取得当前的空闲内存大小
 *
 */
MSTR_EXPORT_API(usize_t) mstr_heap_get_free_size(void);

/**
 * @brief 取得自运行以来空闲内存最小的值
 *
 */
MSTR_EXPORT_API(usize_t) mstr_heap_get_high_water_mark(void);

/**
 * @brief 取得分配器的统计数据
 *
 * @param[out] alloc_count: 分配次数
 * @param[out] free_count: 释放次数
 */
MSTR_EXPORT_API(void)
mstr_heap_get_allocate_count(usize_t* alloc_count, usize_t* free_count);

#define mstr_heap_init(mem, leng)                             \
    do {                                                      \
        mstr_heap_init_sym((intptr_t)(mem), (usize_t)(leng)); \
    } while (0)

#define mstr_heap_alloc(s) (mstr_heap_allocate_sym((s), 4))

#define mstr_heap_free(m)      \
    do {                       \
        mstr_heap_free_sym(m); \
        (m) = NULL;            \
    } while (0)

#endif // _MSTR_USE_MALLOC

#endif // _INCLUDE_MU_HEAP_H_
