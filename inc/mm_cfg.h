// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_cfg.h
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   配置选项
 * @version 1.0
 * @date    2023-05-17
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#if !defined(_INCLUDE_MM_CFG_H_)
#define _INCLUDE_MM_CFG_H_

#include <stdint.h>

#if !defined(_MSTR_USE_HARDWARE_DIV)
/**
 * @brief 指定是否使用硬件除法指令
 *
 */
#define _MSTR_USE_HARDWARE_DIV 0
#endif // _MSTR_USE_HARDWARE_DIV

#if !defined(_MSTR_USE_MALLOC)
/**
 * @brief 是否使用malloc替代自带的分配器
 *
 */
#define _MSTR_USE_MALLOC 0
#endif // _MSTR_USE_MALLOC

#if _MSTR_USE_MALLOC
#include <stdlib.h>
#include <string.h>

#if !defined(_MSTR_MALLOC_FUNCTION)
/**
 * @brief 内存分配函数
 *
 */
#define _MSTR_MEM_ALLOC_FUNCTION(s) malloc(s)
#endif // _MSTR_MALLOC_FUNCTION
#if !defined(_MSTR_MEM_FREE_FUNCTION)
/**
 * @brief 内存释放函数
 *
 */
#define _MSTR_MEM_FREE_FUNCTION(s) free(s)
#endif // _MSTR_MEM_FREE_FUNCTION
#endif // _MSTR_USE_MALLOCs

#if !defined(_MSTR_RUNTIME_HEAP_ALIGN)
/**
 * @brief 指定堆分配的对齐值
 *
 */
#define _MSTR_RUNTIME_HEAP_ALIGN 4
#endif // _MSTR_RUNTIME_HEAP_ALIGN

#if !defined(_MSTR_USE_STD_IO)
/**
 * @brief 指定是否使用stdout这些标准io操作
 *
 */
#define _MSTR_USE_STD_IO 0
#endif // _MSTR_USE_STD_IO

#if _MSTR_USE_STD_IO
#include <stdio.h>
#endif // _MSTR_USE_STD_IO

#if !defined(_MSTR_RUNTIME_CTRLFLOW_MARKER)
/**
 * @brief 指定是否使用unreachable等标记来标记控制流
 *
 */
#define _MSTR_RUNTIME_CTRLFLOW_MARKER 1
#endif // _MSTR_RUNTIME_CTRLFLOW_MARKER

#if _MSTR_RUNTIME_CTRLFLOW_MARKER
#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
/**
 * @brief 标记不可达的分支
 *
 */
#define mstr_unreachable() assert_param(0)
#elif defined(_MSTR_RUNTIME_ASSERT)
#include <assert.h>
#define mstr_unreachable() assert(0)
#else
#define mstr_unreachable() ((void)0U)
#endif
#else
#define mstr_unreachable() ((void)0U)
#endif // _MSTR_RUNTIME_CTRLFLOW_MARKER

//
// 导出函数修辞
//
#if __cplusplus
#define MSTR_EXPORT_MANGLE extern "C"
#else
#define MSTR_EXPORT_MANGLE extern
#endif // __cplusplus
//
// 定义导出定义
//
#if _MSTR_BUILD_DLL
#define MSTR_EXPORT_API(ret) \
    MSTR_EXPORT_MANGLE __declspec(dllexport) ret __stdcall
#elif _MSTR_IMPORT_FROM_DLL
#define MSTR_EXPORT_API(ret) \
    MSTR_EXPORT_MANGLE __declspec(dllimport) ret __stdcall
#elif _MSTR_BUILD_DYLIB
#define MSTR_EXPORT_API(ret) \
    MSTR_EXPORT_MANGLE __attribute__((visibility("default"))) ret
#elif __EMSCRIPTEN__
#include <emscripten.h>
#define MSTR_EXPORT_API(ret) MSTR_EXPORT_MANGLE ret EMSCRIPTEN_KEEPALIVE
#else
#define MSTR_EXPORT_API(ret) MSTR_EXPORT_MANGLE ret
#endif // EXPORT

/**
 * @brief 构建使用了 _MSTR_USE_MALLOC
 *
 */
#define MSTRCFG_USE_MALLOC_BIT     0x01

/**
 * @brief 构建使用了 _MSTR_BUILD_DLL
 *
 */
#define MSTRCFG_BUILD_DLL_BIT      0x02

/**
 * @brief 构建使用了 _MSTR_BUILD_DYLIB
 *
 */
#define MSTRCFG_BUILD_DYLIB_BIT    0x04

/**
 * @brief 构建使用了 WASM
 *
 */
#define MSTRCFG_USE_WASM_BIT       0x08

/**
 * @brief 标记是否使用了硬件除法
 *
 */
#define MSTRCFG_BUILD_HARDWARE_DIV 0x10

/**
 * @brief 标记是否使用了stdio
 *
 */
#define MSTRCFG_USE_STD_IO         0x20

/**
 * @brief 取得构建配置
 *
 * @return 构建配置
 */
MSTR_EXPORT_API(uint32_t) mstr_configure(void);
#endif // _INCLUDE_MM_CFG_H_
