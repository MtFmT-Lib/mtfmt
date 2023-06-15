// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mtfmt.hpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   格式化 (C++ Header)
 * @version 1.0
 * @date    2023-06-03
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#if !defined(_INCLUDE_MTFMT_HPP_)
#define _INCLUDE_MTFMT_HPP_
#include "mm_helper.hpp"
#include "mm_parser.hpp"
#include "mm_result.hpp"
#include "mm_string.hpp"
#include "mtfmt.h"
namespace mtfmt
{
/**
 * @brief 进行格式化
 *
 * @tparam Args: 参数类型
 *
 * @param fmt_str: 格式化串
 * @param args: 格式化参数
 *
 * @return result<string, error_code_t>: 结果
 */
template <typename... Args>
inline result<string, error_code_t> format(
    const string::char_t* fmt_str, Args&&... args
)
{
    string str;
    error_code_t code = mstr_format(
        &str.raw_object(),
        fmt_str,
        sizeof...(args),
        std::forward<Args&&>(args)...
    );
    if (MSTR_SUCC(code)) {
        return str;
    }
    else {
        return code;
    }
}
} // namespace mtfmt
#endif // _INCLUDE_MTFMT_H_
