// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_io.h
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   IO
 * @version 1.0
 * @date    2023-06-07
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#if !defined(_INCLUDE_MM_IO_H_)
#define _INCLUDE_MM_IO_H_ 1
#include "mm_cfg.h"
#include "mm_result.h"
#include "mm_type.h"

/**
 * @brief 写入数据
 *
 */
typedef mstr_result_t (*MStrIOWrite)(
    void* ctx, const byte_t* data, usize_t len
);

/**
 * @brief IO回调的接口
 *
 */
typedef struct tagMStrIOCallback
{
    /**
     * @brief 上下文
     *
     */
    void* capture;

    /**
     * @brief 写入数据callback
     *
     */
    MStrIOWrite io_write;
} MStrIOCallback;

/**
 * @brief 初始化IO的结构
 *
 * @param[in] context: 上下文
 * @param[inout] obj: IO结构对象
 * @param[in] cb_write: 写入数据callback, 不可留NULL
 *
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_io_init(void* context, MStrIOCallback* obj, MStrIOWrite cb_write);

/**
 * @brief 格式化字符串到指定io
 *
 * @param[inout] io: IO
 * @param[in] fmt: 格式化串
 * @param[in] fmt_place: 预期fmt中使用的参数数目.
 * 最大不超过16(MFMT_PLACE_MAX_NUM)
 *
 * @return minfmt_result_t: 格式化结果
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_ioformat(
    MStrIOCallback* io, const char* fmt, usize_t fmt_place, ...
);

/**
 * @brief 格式化字符串到指定io
 *
 * @param[inout] io: IO
 * @param[in] fmt: 格式化串
 * @param[in] fmt_place: 预期fmt中使用的参数数目.
 * 最大不超过16(MFMT_PLACE_MAX_NUM)
 * @param[in] ap_ptr: &ap
 *
 * @return minfmt_result_t: 格式化结果
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_iovformat(
    MStrIOCallback* io,
    const char* fmt,
    usize_t fmt_place,
    va_list* ap_ptr
);

/**
 * @brief 取得stdout的内部handler
 *
 */
MSTR_EXPORT_API(MStrIOCallback*) mstr_get_stdout(void);

#if _MSTR_USE_STD_IO
/**
 * @brief 写到 stdout
 *
 * @attention 宏 _MSTR_USE_STD_IO 为1时有效, 该宏假定格式化参数为最大值,
 *            可能会为坏坏刻意修改格式化串从而造成二进制安全问题
 */
#define mstr_print(fmt, ...)                                    \
    (mstr_ioformat(                                             \
        mstr_get_stdout(), fmt, MFMT_PLACE_MAX_NUM, __VA_ARGS__ \
    ))
#else
/**
 * @brief 写到 stdout (disable)
 *
 * @note 宏 _MSTR_USE_STD_IO 为1时启用
 *
 */
#define mstr_print(fmt, ...) ((void)(0))
#endif // _MSTR_USE_STD_IO
#endif // _INCLUDE_MM_IO_H_
