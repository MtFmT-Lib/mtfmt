// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_fmt.h
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串格式化
 * @version 1.0
 * @date    2023-03-21
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#if !defined(_INCLUDE_MM_FMT_H_)
#define _INCLUDE_MM_FMT_H_
#include "mm_cfg.h"
#include "mm_parser.h"
#include "mm_result.h"
#include "mm_string.h"
#include "mm_typedef.h"

/**
 * @brief 格式化字符串
 *
 * @param[in] fmt: 格式化串
 * @param[out] res_str: 格式化结果输出
 * @param[in] buff_sz: buff的最大允许大小
 * @param[in] fmt_place: 预期fmt中使用的参数数目.
 * 最大不超过16(MFMT_PLACE_MAX_NUM)
 *
 * @return minfmt_result_t: 格式化结果
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_format(MString* res_str, const char* fmt, usize_t fmt_place, ...);

/**
 * @brief 格式化字符串
 *
 * @param[in] fmt: 格式化串
 * @param[out] buff: 格式化结果输出
 * @param[in] buff_sz: buff的最大允许大小
 * @param[in] fmt_place: 预期fmt中使用的参数数目.
 * 最大不超过16(MFMT_PLACE_MAX_NUM)
 * @param[in] ap_ptr: &ap ( 这样子是为了避免verify时的问题 )
 *
 * @return minfmt_result_t: 格式化结果
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_vformat(
    const char* fmt,
    MString* res_str,
    usize_t fmt_place,
    va_list* ap_ptr
);

/**
 * @brief 将无符号量化值转换为字符串
 *
 * @param[out] res_str: 转换结果
 * @param[in] value: 需要转换的值
 * @param[in] quat: 量化精度
 *
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_fmt_uqtoa(MString* res_str, uint32_t value, uint32_t quat);

/**
 * @brief 将无符号整数转换为字符串
 *
 * @param[out] res_str: 转换结果
 * @param[in] value: 需要转换的值
 * @param[in] index: Index
 *
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_fmt_utoa(MString* res_str, uint32_t value, MStrFmtIntIndex index);

/**
 * @brief 将日期时间值转换为字符串
 *
 *
 * @param[out] res_str: 转换结果
 * @param[in] tm: 日期时间值
 * @param[in] spec: 格式化信息
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_fmt_ttoa(
    MString* res_str,
    const MStrTime* tm,
    const MStrFmtChronoFormatSpec* spec
);
#endif // !_INCLUDE_MM_FMT_H_
