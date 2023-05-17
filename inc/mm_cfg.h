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

#if !defined(_MSTR_USE_MALLOC)
/**
 * @brief 是否使用malloc替代自带的分配器
 *
 */
#define _MSTR_USE_MALLOC 0
#endif // _MSTR_USE_MALLOC

#if !defined(_MSTR_RUNTIME_HEAP_SIZE)
/**
 * @brief 指定堆的大小
 *
 */
#define _MSTR_RUNTIME_HEAP_SIZE 2048
#endif // _MSTR_RUNTIME_HEAP_SIZE

#if !defined(_MSTR_RUNTIME_HEAP_ALIGN)
/**
 * @brief 指定堆分配的对齐值
 *
 */
#define _MSTR_RUNTIME_HEAP_ALIGN 4
#endif // _MSTR_RUNTIME_HEAP_ALIGN

#if __cplusplus
/**
 * @brief 导出函数修辞
 *
 */
#define MSTR_EXPORT_MANGLE extern "C"
#else
#define MSTR_EXPORT_MANGLE
#endif // __cplusplus
#if __EMSCRIPTEN__
/**
 * @brief 定义导出定义
 *
 */
#define MSTR_EXPORT_API(ret) MSTR_EXPORT_MANGLE ret EMSCRIPTEN_KEEPALIVE
#else
#define MSTR_EXPORT_API(ret) MSTR_EXPORT_MANGLE extern ret
#endif // EXPORT

#endif // _INCLUDE_MM_CFG_H_
