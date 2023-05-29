// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_heap.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   堆内存管理
 * @version 1.0
 * @date    2023-04-22
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "mm_heap.h"
#include <stddef.h>
#include <stdint.h>

#undef mstr_heap_init
#undef mstr_heap_free
#undef mstr_heap_alloc

/**
 * @brief 堆的size, 可以省个几bytes
 *
 */
typedef u16_t heap_size_t;

/**
 * @brief 空闲块记录
 *
 */
typedef struct tagFreeBlock
{
    /**
     * @brief 本空闲块的大小
     *
     */
    heap_size_t size;

    /**
     * @brief 对齐填充
     *
     */
    heap_size_t align_fill;

    /**
     * @brief 下一个空闲块的位置
     *
     */
    struct tagFreeBlock* next;
} FreeBlock;

/**
 * @brief 堆管理器
 *
 */
typedef struct tagHeap
{
    /**
     * @brief 空闲链表的header和末尾
     *
     */
    FreeBlock *head, *tail;

    /**
     * @brief 实际可用的内存区大小
     *
     */
    heap_size_t memory_size;

    /**
     * @brief 当前空闲内存区的总大小
     *
     */
    heap_size_t cur_free_size;

    /**
     * @brief 最差情况下的空闲内存大小
     *
     */
    heap_size_t free_highwatermark;

    /**
     * @brief 分配次数的统计
     *
     */
    u32_t alloc_count;

    /**
     * @brief 释放次数的统计
     *
     */
    u32_t free_count;
} Heap;

/**
 * @brief 全局的堆管理器
 *
 */
static Heap heap;

//
// private:
//

static iptr_t align_of(iptr_t, usize_t);
static void heap_free_impl(Heap*, void*);
static void heap_init_impl(Heap*, iptr_t, usize_t);
static void insert_free_block(const Heap*, FreeBlock*);
static void insert_block_helper(const Heap*, FreeBlock*, FreeBlock*);
static FreeBlock* split_free_block(FreeBlock*, heap_size_t);
static FreeBlock* allocate_tactic(const Heap*, heap_size_t);
static void* heap_allocate_impl(Heap*, heap_size_t, heap_size_t);

//
// public:
//

MSTR_EXPORT_API(void)
mstr_heap_init(intptr_t heap_memory, usize_t heap_size)
{
    heap_init_impl(&heap, heap_memory, heap_size);
}

MSTR_EXPORT_API(void*) mstr_heap_allocate(usize_t size, usize_t align)
{
    return heap_allocate_impl(
        &heap, (heap_size_t)size, (heap_size_t)align
    );
}

MSTR_EXPORT_API(void) mstr_heap_free(void* memory)
{
    heap_free_impl(&heap, memory);
}

MSTR_EXPORT_API(usize_t) mstr_heap_get_free_size(void)
{
    return heap.cur_free_size;
}

MSTR_EXPORT_API(usize_t) mstr_heap_get_high_water_mark(void)
{
    return heap.free_highwatermark;
}

MSTR_EXPORT_API(void)
mstr_heap_get_allocate_count(usize_t* alloc_count, usize_t* free_count)
{
    *alloc_count = heap.alloc_count;
    *free_count = heap.free_count;
}

/**
 * @brief 初始化堆
 *
 * @param[out] heap: heap结构
 * @param[in] memory: heap所占用的内存区
 * @param[in] memory_size: heap所占用的内存区大小
 */
static void heap_init_impl(
    Heap* heap, iptr_t memory, usize_t memory_size
)
{
    iptr_t head_addr = align_of(memory, _MSTR_RUNTIME_HEAP_ALIGN);
    usize_t total_size = memory_size - (usize_t)(head_addr - memory);
    // 起始位置的node
    FreeBlock* head_block = (FreeBlock*)head_addr;
    head_block->size = 0;
    head_block->align_fill = 0;
    // 结束位置的node
    iptr_t end_addr = align_of(
        head_addr + total_size - sizeof(FreeBlock),
        _MSTR_RUNTIME_HEAP_ALIGN
    );
    FreeBlock* end_block = (FreeBlock*)end_addr;
    end_block->size = 0;
    end_block->next = NULL;
    end_block->align_fill = 0;
    // 第一个块
    iptr_t first_addr = head_addr + sizeof(FreeBlock);
    FreeBlock* first_block = (FreeBlock*)first_addr;
    first_block->size =
        (heap_size_t)(end_addr - head_addr - sizeof(FreeBlock));
    first_block->align_fill = 0;
    // 和end连起来
    first_block->next = (FreeBlock*)end_addr;
    // 和head连起来
    head_block->next = (FreeBlock*)first_addr;
    // 初始化heap的结构
    heap->memory_size = (heap_size_t)memory_size;
    heap->head = head_block;
    heap->tail = end_block;
    heap->cur_free_size = first_block->size;
    heap->free_highwatermark = first_block->size;
    heap->alloc_count = 0;
    heap->free_count = 0;
}

/**
 * @brief 在堆中分配内存
 *
 * @param heap: 堆
 * @param need_size: 需要分配的大小
 * @param align: 要求的字节对齐
 * @return void*: 内存区, 分配失败返回NULL
 */
static void* heap_allocate_impl(
    Heap* heap, heap_size_t need_size, heap_size_t align
)
{
    heap_size_t alloc_size =
        (heap_size_t)align_of(need_size + sizeof(FreeBlock), align);
    // 找到合适的空闲块
    FreeBlock* alloc_block = allocate_tactic(heap, alloc_size);
    if (alloc_block == NULL) {
        return NULL;
    }
    // 计算对齐后的地址
    iptr_t head_addr = (iptr_t)alloc_block;
    iptr_t align_addr = align_of(head_addr, align);
    iptr_t align_offset = align_addr - head_addr;
    // 把FreeBlock描述放到align_addr位置, 并记录分配大小和对齐偏移量
    FreeBlock* mem_block = (FreeBlock*)align_addr;
    mem_block->next = NULL;
    mem_block->size = (heap_size_t)alloc_size;
    mem_block->align_fill = (heap_size_t)align_offset;
    // 内存区
    iptr_t res_mem = align_addr + sizeof(FreeBlock);
    // 减去使用的大小
    heap->cur_free_size -= alloc_size;
    // 判断high water mark
    if (heap->free_highwatermark > heap->cur_free_size) {
        heap->free_highwatermark = heap->cur_free_size;
    }
    // 统计分配次数
    heap->alloc_count += 1;
    // ret
    return (void*)res_mem;
}

/**
 * @brief 释放由堆分配器分配的内存
 *
 * @param heap
 * @param mem
 */
static void heap_free_impl(Heap* heap, void* mem)
{
    FreeBlock* block = (FreeBlock*)((iptr_t)(mem) - sizeof(FreeBlock));
    // 本内存块大小
    heap_size_t block_sz = block->size + block->align_fill;
    // 空闲块的真正起始位置
    iptr_t freeblock_head =
        (iptr_t)(mem) - sizeof(FreeBlock) - block->align_fill;
    // 构造正确的node
    FreeBlock* free_block = (FreeBlock*)freeblock_head;
    free_block->next = NULL;
    free_block->size = block_sz;
    free_block->align_fill = 0;
    // 插入到freelist
    insert_free_block(heap, free_block);
    // 增加freesize
    heap->cur_free_size += block_sz;
    // 统计释放次数
    heap->free_count += 1;
}

/**
 * @brief 分配策略, 尝试找到 need_size 大小的 free block
 *
 * @param heap: 堆
 * @param need_size: 需要的大小
 * @return FreeBlock*: 找到的空闲块, 如果没有找到返回NULL
 */
static FreeBlock* allocate_tactic(
    const Heap* heap, heap_size_t need_size
)
{
    FreeBlock* block_it = heap->head->next;
    FreeBlock* prev_it = heap->head;
    FreeBlock* result = NULL;
    while (block_it != heap->tail) {
        FreeBlock* block = block_it;
        if (block->next != NULL && block->size <= need_size) {
            // 尺寸不合适, 移动到下一个块
            prev_it = block_it;
            block_it = block->next;
        }
        else {
            // 尺寸合适
            // 移走block这个node
            prev_it->next = block_it->next;
            if (block->size > sizeof(FreeBlock) * 2) {
                // 该空闲块可以split成两部分
                FreeBlock* new_block =
                    split_free_block(block_it, need_size);
                // 插入新的空闲块
                insert_free_block(heap, new_block);
            }
            // return
            result = block_it;
            break;
        }
    }
    return result;
}

/**
 * @brief 分割空闲块
 *
 * @note 从 block 前段划分出alloc_size大小的空间, 并返回划分后的新free
 * block
 *
 * @param block: 需要划分的块
 * @param alloc_size: 分配大小
 * @return FreeBlock*: 划分后的结果
 */
static FreeBlock* split_free_block(
    FreeBlock* block, heap_size_t alloc_size
)
{
    // 考虑如下的free block
    //
    // ```txt
    // | freeblock |
    // ```
    //
    // 按照分配的大小`alloc_sz`变成
    //
    // ```txt
    // | cur_block |  | new_block |
    //   /|\            /|\       |
    //   删掉            返回
    // ```
    iptr_t last_addr = (iptr_t)block;
    iptr_t next_addr = last_addr + alloc_size;
    FreeBlock* cur_block = block;
    FreeBlock* new_block = (FreeBlock*)next_addr;
    new_block->align_fill = 0;
    new_block->size = cur_block->size - alloc_size;
    new_block->next = cur_block->next;
    // ret
    return (FreeBlock*)next_addr;
}

/**
 * @brief 插入空闲块
 *
 * @note 该函数和 `insert_block_helper` 配合使用
 *
 * @param[in] heap: heap
 * @param[inout] block: 需要插入的空闲块
 */
static void insert_free_block(const Heap* heap, FreeBlock* block)
{
    FreeBlock* it = heap->head;
    // 找到一个位置, 使得it->next的地址大于node
    while (it->next < block) {
        it = it->next;
    }
    // 插入并合并空闲块
    FreeBlock* insert_pos = it;
    insert_block_helper(heap, insert_pos, block);
}

/**
 * @brief 插入并尝试合并一个块
 *
 */
static void insert_block_helper(
    const Heap* heap, FreeBlock* insert_pos, FreeBlock* insert_block
)
{
    // # 插入并合并空闲块
    //
    // 空闲块合并旨在合并相邻的内存区域, 以减少内存碎片。
    //
    // 该函数基于空闲块链表的所有node的地址是单调递增的假设而设计
    //
    // 对于该策略的情况有二:
    //
    // a. 插入的块`insert_block`在其前一个块之后
    //
    // ```txt
    //  ... -> | insert_pos | -> | insert_pos.next | -> ...
    //                       /|\ 地址是这个
    // ```
    //
    // 此时进行合并:
    //
    // ```txt
    //  ... -> | insert_pos        | -> | insert_pos.next | -> ...
    //                       /|\ 空闲块扩大
    // ```
    //
    // 然后把空闲块作为下一步需要插入的块`then_need_insert`,
    // 不然下一步需要插入的块还是`insert_block`
    //
    // b. 插入的块`insert_block`后面紧跟着一个块
    //
    // ```txt
    //  ... -> | insert_pos | -> | insert_pos.next | -> ...
    //                         /|\ 地址是这个, 后面紧跟着insert_pos.next
    // ```
    //
    // 此时依然要进行合并, 分两种情况处理:
    //
    //  * insert_pos.next是最后一个块, 此时不合并,
    //    因为最后一个块是不放数据的
    //  * insert_pos.next不是最后一个块, 进行合并
    //
    // 如果需要合并, 完成后变成这样:
    //
    // ```txt
    //  ... -> | insert_pos | -> |        insert_pos.next | -> ...
    //                            /|\ 空闲块扩大
    // ```
    FreeBlock* first_insert_pos = insert_pos;
    FreeBlock* first_insert_block = insert_block;
    // 检查insert_pos后面是不是刚好是node
    FreeBlock* then_insert_block;
    iptr_t insert_beg = (iptr_t)insert_pos;
    iptr_t need_node_addr = (iptr_t)first_insert_block;
    if (insert_beg + first_insert_pos->size == need_node_addr) {
        // insert_pos后面是insert_block, 合并它们
        first_insert_pos->size += first_insert_block->size;
        // 然后尝试插入合并后的块
        then_insert_block = first_insert_pos;
    }
    else {
        // 不然就还是尝试插入原始块
        then_insert_block = first_insert_block;
    }
    // 检查insert_pos.next前面是不是刚好是insert_block
    iptr_t then_insert_addr = (iptr_t)then_insert_block;
    FreeBlock* next_node = first_insert_pos->next;
    iptr_t next_node_addr = (iptr_t)next_node;
    if (then_insert_addr + then_insert_block->size == next_node_addr) {
        if (next_node != heap->tail) {
            // 后面不是freelist尾, 可以合并
            then_insert_block->size += next_node->size;
            then_insert_block->next = next_node->next;
        }
        else {
            // 不能合并, 把它和freelist尾连起来
            then_insert_block->next = heap->tail;
        }
    }
    else {
        // 把insert_block和下一个node连起来
        then_insert_block->next = first_insert_pos->next;
    }
    // 如果情况a没处理,
    // 那么要把first_insert_pos和first_insert_block连起来
    if (then_insert_block != first_insert_pos) {
        first_insert_pos->next = first_insert_block;
    }
}

/**
 * @brief 计算beg对齐到align字节的对齐地址
 *
 * @param[in] beg: 起始地址
 * @param[in] align: 需要对齐的字节数, 必须是2的整数幂
 *
 *  @return heap_size_t: 对齐后的地址
 */
static iptr_t align_of(iptr_t beg, usize_t align)
{
    return (beg + align - 1) & ~(align - 1);
}
