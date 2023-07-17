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

/**
 * @brief 表示使用msvc编译
 *
 */
#define MSTR_BUILD_CC_MSVC       0x1

/**
 * @brief 表示使用gnuc编译
 *
 */
#define MSTR_BUILD_CC_GNUC       0x2

/**
 * @brief 表示使用armcc编译
 *
 */
#define MSTR_BUILD_CC_ARMCC      0x3

/**
 * @brief 表示使用armcc (ac6)编译
 *
 */
#define MSTR_BUILD_CC_ARMCLANG   0x4

/**
 * @brief 表示使用WASM
 *
 */
#define MSTR_BUILD_CC_EMSCRIPTEN 0x5

/**
 * @brief 表示使用其它的编译器编译
 *
 */
#define MSTR_BUILD_CC_OTHER      0xf

#if defined(_MSC_VER)
#define MSTR_BUILD_CC MSTR_BUILD_CC_MSVC
#elif defined(__ARMCC_VERSION)
// 优先考虑armcc而不是gnuc
#if __ARMCC_VERSION >= 6000000
#define MSTR_BUILD_CC MSTR_BUILD_CC_ARMCLANG
#else
#define MSTR_BUILD_CC MSTR_BUILD_CC_ARMCC
#endif // __ARMCC_VERSION
#elif defined(__GNUC__)
#define MSTR_BUILD_CC MSTR_BUILD_CC_GNUC
#elif defined(__EMSCRIPTEN__)
#define MSTR_BUILD_CC MSTR_BUILD_CC_EMSCRIPTEN
#else
#define MSTR_BUILD_CC MSTR_BUILD_CC_OTHER
#endif // 编译器版本

#if defined(MSTR_IMP_SOURCES) && MSTR_BUILD_CC == MSTR_BUILD_CC_MSVC
// "该文件包含不能在当前代码页(XXXX)中表示的字符"
// unbengable (￣_￣|||)
#pragma warning(disable : 4819)
#endif // _MSC_VER

#if defined(MSTR_IMP_SOURCES) && MSTR_BUILD_CC == MSTR_BUILD_CC_ARMCLANG
// 我们不需要任何的对齐警告, 分支覆盖警告
// 因为parser在很多case不会处理所有情况(取而代之由default: 处理 break;)
// 所有的结构体不需要关注padding填充, 因为没有任何一个地方使用
// reinterpret_cast 指针转换均不需要关注对齐问题,
// 因为堆内存分配按照机器字长对齐
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wswitch-enum"
#elif defined(MSTR_IMP_SOURCES) && MSTR_BUILD_CC == MSTR_BUILD_CC_ARMCC
// TODO
#endif // _MSC_VER

#if !defined(_MSTR_USE_HARDWARE_DIV)
#if MSTR_BUILD_CC == MSTR_BUILD_CC_ARMCLANG || \
    MSTR_BUILD_CC == MSTR_BUILD_CC_ARMCC
#if defined(__TARGET_FEATURE_DIVIDE)
/**
 * @brief 指定是否使用硬件除法指令(跟随arm cc, 使用)
 *
 */
#define _MSTR_USE_HARDWARE_DIV 1
#else
/**
 * @brief 指定是否使用硬件除法指令(跟随arm cc, 关闭)
 *
 */
#define _MSTR_USE_HARDWARE_DIV 1
#endif // __TARGET_FEATURE_DIVIDE
#else
/**
 * @brief 指定是否使用硬件除法指令 (关闭)
 *
 */
#define _MSTR_USE_HARDWARE_DIV 0
#endif // 硬件除法, 默认关掉
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

#if !defined(_MSTR_MEM_ALLOC_FUNCTION)
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

#if !defined(_MSTR_MEM_REALLOC_FUNCTION)
#if !defined(_MSTR_MEM_ALLOC_FUNCTION) && \
    !defined(_MSTR_MEM_FREE_FUNCTION)
/**
 * @brief 内存重新分配函数, 使用c标准库
 *
 */
#define _MSTR_MEM_REALLOC_FUNCTION(old_p, s) realloc((old_p), (s))
/**
 * @brief 重新分配函数可用
 *
 */
#define MSTR_MEM_REALLOC_FUNCTION_NOT_AVAL   0
#else
/**
 * @brief 重新分配函数不可用
 *
 */
#define MSTR_MEM_REALLOC_FUNCTION_NOT_AVAL 1
#endif // _MSTR_MEM_XXX
#endif // _MSTR_MEM_REALLOC_FUNCTION
#else
/**
 * @brief 重新分配函数可用
 *
 */
#define MSTR_MEM_REALLOC_FUNCTION_NOT_AVAL 0
#endif // _MSTR_USE_MALLOCs

#if !defined(_MSTR_RUNTIME_HEAP_TRACING)
/**
 * @brief 指定heap tracing
 *
 * @note 该宏定义的函数拥有原型 (type: int, ptr: void*, size:
 * usize, old_size: usize_t) -> (), 其会在分配和释放时调用。type:
 * 0=alloc, 1=realloc, 2=free; size为操作大小, alloc时表示分配的大小,
 * realloc表示ptr新的大小, free时表示释放的大小,
 * old_size记录了realloc时曾经ptr的大小, 其余时候无效
 *
 * @attention 该宏仅在内建分配器上被使用
 *
 */
#define _MSTR_RUNTIME_HEAP_TRACING(type, ptr, size, old_size) ((void)0U)
#endif // _MSTR_RUNTIME_HEAP_TRACING

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

#if !defined(_MSTR_USE_FP_FLOAT32)
#if MSTR_BUILD_CC == MSTR_BUILD_CC_ARMCLANG || \
    MSTR_BUILD_CC == MSTR_BUILD_CC_ARMCC
#if defined(__TARGET_FPU_VFP)
/**
 * @brief 指定使用float32 (ARMCC 使用vfp)
 *
 */
#define _MSTR_USE_FP_FLOAT32 1
#else
/**
 * @brief 指定是否使用float32 (ARMCC未指定vfp, 不启用)
 *
 */
#define _MSTR_USE_FP_FLOAT32 0
#endif // ARMCC: __TARGET_FPU
#else
/**
 * @brief 指定是否使用float32
 *
 */
#define _MSTR_USE_FP_FLOAT32 0
#endif // MSTR_BUILD_CC
#endif // _MSTR_USE_FP_FLOAT32

#if !defined(_MSTR_USE_FP_FLOAT64)
#if MSTR_BUILD_CC == MSTR_BUILD_CC_ARMCLANG || \
    MSTR_BUILD_CC == MSTR_BUILD_CC_ARMCC
#if defined(__TARGET_FPU_SOFTVFP_VFPV3_D16) ||      \
    defined(__TARGET_FPU_SOFTVFP_VFPV3_D16_FP16) || \
    defined(__TARGET_FPU_SOFTVFP_VFPV4_D16) ||      \
    defined(__TARGET_FPU_VFPV3_D16) ||              \
    defined(__TARGET_FPU_VFPV3_D16_FP16) ||         \
    defined(__TARGET_FPU_VFPV4_D16)
/**
 * @brief 指定是否使用float64 (ARMCC指定d16或d32的vfp, 启用)
 *
 */
#define _MSTR_USE_FP_FLOAT64 1
#else
/**
 * @brief 指定是否使用float64 (ARMCC未指定d16或d32的vfp, 不启用)
 *
 */
#define _MSTR_USE_FP_FLOAT64 0
#endif // ARMCC: __TARGET_FPU
#else
/**
 * @brief 指定是否使用float64
 *
 */
#define _MSTR_USE_FP_FLOAT64 0
#endif // MSTR_BUILD_CC
#endif // _MSTR_USE_FP_FLOAT64

/**
 * @brief 是否有float的支持
 *
 */
#define _MSTR_USE_FP                                 \
    (_MSTR_USE_FP_FLOAT16 || _MSTR_USE_FP_FLOAT32 || \
     _MSTR_USE_FP_FLOAT64)

#if !defined(_MSTR_USE_CPP_EXCEPTION)
#if MSTR_BUILD_CC == MSTR_BUILD_CC_ARMCLANG || \
    MSTR_BUILD_CC == MSTR_BUILD_CC_ARMCC
#if defined(__EXCEPTIONS)
/**
 * @brief 指定是否使用异常(跟随arm cc, 使用)
 *
 */
#define _MSTR_USE_CPP_EXCEPTION 1
#else
/**
 * @brief 指定是否使用异常(跟随arm cc, 不使用)
 *
 */
#define _MSTR_USE_CPP_EXCEPTION 0
#endif // __EXCEPTIONS
#else
/**
 * @brief 指定是否使用异常 (默认打开)
 *
 */
#define _MSTR_USE_CPP_EXCEPTION 1
#endif // MSTR_BUILD_CC
#endif // _MSTR_USE_CPP_EXCEPTION

#if !defined(_MSTR_USE_UTF_8)
/**
 * @brief 指定是否启用UTF-8支持
 *
 */
#define _MSTR_USE_UTF_8 1
#endif // _MSTR_USE_UTF_8

#if !defined(_MSTR_BUILD_DLL)
/**
 * @brief 指定是否使用动态库构建
 *
 */
#define _MSTR_BUILD_DLL 0
#endif // _MSTR_BUILD_DLL

#if _MSTR_RUNTIME_CTRLFLOW_MARKER
#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
/**
 * @brief 标记不可达的分支
 *
 */
#define mstr_unreachable()         assert_param(0)
/**
 * @brief 边界检查
 *
 */
#define mstr_bounding_check(expr)  assert_param(!!(expr))
/**
 * @brief 造成异常
 *
 */
#define mstr_cause_exception(code) assert_param(0)
/**
 * @brief assert
 *
 */
#define mstr_assert(e)             assert_param(e)
#elif defined(_MSTR_RUNTIME_ASSERT)
#include <assert.h>
#define mstr_unreachable()         assert(0)
#define mstr_bounding_check(expr)  assert(!!(expr))
#define mstr_cause_exception(code) assert(0)
#define mstr_assert(e)             assert(e)
#else
#define mstr_unreachable()         ((void)0U)
#define mstr_bounding_check(expr)  ((void)0U)
#define mstr_cause_exception(code) ((void)0U)
#define mstr_assert(e)             ((void)0U)
#endif
#else
#define mstr_unreachable()         ((void)0U)
#define mstr_bounding_check(expr)  ((void)0U)
#define mstr_cause_exception(code) ((void)0U)
#define mstr_assert(e)             ((void)0U)
#endif // _MSTR_RUNTIME_CTRLFLOW_MARKER

//
// 导出函数修辞
//
#if defined(__cplusplus)
#define MSTR_EXPORT_MANGLE extern "C"
#else
#define MSTR_EXPORT_MANGLE extern
#endif // __cplusplus
//
// 定义导出定义
//
#if defined(_MSTR_BUILD_DLL)
#if MSTR_BUILD_CC == MSTR_BUILD_CC_MSVC
#define MSTR_EXPORT_API(ret) \
    MSTR_EXPORT_MANGLE __declspec(dllexport) ret __stdcall
#else
#define MSTR_EXPORT_API(ret) \
    MSTR_EXPORT_MANGLE __attribute__((visibility("default"))) ret
#endif // _MSC_VER
#elif defined(_MSTR_IMPORT_FROM_DLL)
#define MSTR_EXPORT_API(ret) \
    MSTR_EXPORT_MANGLE __declspec(dllimport) ret __stdcall
#elif MSTR_BUILD_CC == MSTR_BUILD_CC_EMSCRIPTEN
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
 * @brief 标记是否使用了硬件除法
 *
 */
#define MSTRCFG_BUILD_HARDWARE_DIV 0x04

/**
 * @brief 标记是否使用了stdio
 *
 */
#define MSTRCFG_USE_STD_IO         0x08

/**
 * @brief 标记是否使用了UTF-8
 *
 */
#define MSTRCFG_USE_UTF_8          0x10

/**
 * @brief 标记是否使用了CXX异常
 *
 */
#define MSTRCFG_USE_CXX_EXCEPTION  0x20

/**
 * @brief 标记是否使用了float16支持
 *
 */
#define MSTRCFG_USE_FLOAT16        0x40

/**
 * @brief 标记是否使用了float支持
 *
 */
#define MSTRCFG_USE_FLOAT32        0x80

/**
 * @brief 标记是否使用了float64支持
 *
 */
#define MSTRCFG_USE_FLOAT64        0x100

/**
 * @brief 取得库版本信息
 *
 */
#define MSTR_CONFIGURE_VER_VAL(c)  (((c) >> 16) & 0xffff)

/**
 * @brief 取得使用的编译器版本
 *
 */
#define MSTR_CONFIGURE_CC_VAL(c)   (((c) >> 12) & 0xf)

/**
 * @brief 取得构建配置
 *
 */
#define MSTR_CONFIGURE_CFG_VAL(c)  ((c)&0xfff)

//@ <autogen:version>

//@ </autogen:version>

/**
 * @brief 取得构建配置
 *
 * @return 构建配置
 */
MSTR_EXPORT_API(uint32_t) mstr_configure(void);
#endif // _INCLUDE_MM_CFG_H_
