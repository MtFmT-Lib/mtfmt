// SPDX-License-Identifier: GPL-2.0
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
#include "mm_typedef.h"

#if !defined(_MSTR_USE_MALLOC)
/**
 * @brief 是否使用malloc替代自带的分配器
 *
 */
#define _MSTR_USE_MALLOC 0
#endif // _MSTR_USE_MALLOC

/**
 * @brief 指定堆的大小
 *
 */
#define RUNTIME_HEAP_SIZE  2048

/**
 * @brief 指定堆分配的对齐值
 *
 */
#define RUNTIME_HEAP_ALIGN 4

/**
 * @brief 初始化堆分配器
 *
 */
extern void mheap_init(void);

/**
 * @brief 尝试从堆中分配size大小的内存
 *
 * @return void*: 分配结果, 如果分配失败返回NULL
 */
extern void* mheap_allocate(usize_t size, usize_t align);

/**
 * @brief 释放由 mheap_allocate 分配的内存
 *
 */
extern void mheap_free(void* memory);

/**
 * @brief 取得当前的空闲内存大小
 *
 */
extern usize_t mheap_get_free_size(void);

/**
 * @brief 取得自运行以来空闲内存最小的值
 *
 */
extern usize_t mheap_get_high_water_mark(void);

/**
 * @brief 取得分配器的统计数据
 *
 * @param[out] alloc_count: 分配次数
 * @param[out free_count: 释放次数
 */
extern void mheap_get_allocate_count(
    usize_t* alloc_count, usize_t* free_count
);

#if _MSTR_USE_MALLOC
#define mstr_heap_alloc(s) (malloc((s)))
#define mstr_heap_free(m) \
    do {                  \
        free(m);          \
        (m) = NULL;       \
    } while (0)
#else
#define mstr_heap_alloc(s) (mheap_allocate((s), 4))
#define mstr_heap_free(m) \
    do {                  \
        mheap_free(m);    \
        (m) = NULL;       \
    } while (0)
#endif // _MSTR_USE_MALLOC

#endif // _INCLUDE_MU_HEAP_H_
