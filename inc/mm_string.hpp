// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_string.hpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串 wrapper
 * @version 1.0
 * @date    2023-06-15
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#if !defined(_INCLUDE_MM_STRING_HPP_)
#define _INCLUDE_MM_STRING_HPP_ 1
#include "mm_cfg.h"
#include "mm_result.hpp"
#include "mm_string.h"
#include <cstddef>
#include <string>
namespace mtfmt
{
/**
 * @brief 字符串类
 *
 */
class string final
{
    /**
     * @brief c对象
     *
     */
    MString this_obj;

public:
    using char_t = char;
    using value_t = char_t;
    using pointer = value_t*;
    using const_pointer = const value_t*;
    using reference = value_t&;
    using const_reference = const value_t&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    /**
     * @brief 创建空的字符串
     *
     */
    string() noexcept
    {
        mstr_create_empty(&this_obj);
    }

    /**
     * @brief 从C字符串创建
     *
     * @param c_str: c字符串
     */
    explicit string(const char_t* c_str)
    {
        mstr_create(&this_obj, c_str);
    }

    /**
     * @brief 从std::string创建
     *
     * @param str: str
     */
    explicit string(const std::string& str)
    {
        mstr_create(&this_obj, str.c_str());
    }

    /**
     * @brief 从self创建
     *
     * @param str: str
     */
    string(const string& str) noexcept
    {
        mstr_copy_create(&this_obj, &str.this_obj);
    }

    ~string()
    {
        mstr_free(&this_obj);
    }

    /**
     * @brief copy
     */
    string& operator=(const string& str) noexcept
    {
        mstr_clear(&this_obj);
        mstr_concat(&this_obj, &str.this_obj);
        return *this;
    }

    /**
     * @brief 相等
     *
     */
    bool operator==(const string& str) const noexcept
    {
        return !!mstr_equal(&this_obj, &str.this_obj);
    }

    /**
     * @brief 相等(cstr)
     *
     */
    template <std::size_t N>
    bool operator==(const char_t (&str)[N]) const noexcept
    {
        MString obj;
        mstr_create(&obj, str);
        return !!mstr_equal(&this_obj, &obj);
    }

    /**
     * @brief 相等(cstr left)
     *
     */
    template <std::size_t N>
    friend bool operator==(
        const char_t (&str)[N], const string& pthis
    ) noexcept
    {
        return pthis == str;
    }

    /**
     * @brief 不等
     *
     */
    bool operator!=(const string& str) const noexcept
    {
        return !(*this == str);
    }

    /**
     * @brief 不等(cstr)
     *
     */
    template <std::size_t N>
    bool operator!=(const char_t (&str)[N]) const noexcept
    {
        return !(*this == str);
    }

    /**
     * @brief 不等(cstr left)
     *
     */
    template <std::size_t N>
    friend bool operator!=(
        const char_t (&str)[N], const string& pthis
    ) noexcept
    {
        return !(pthis == str);
    }

    /**
     * @brief 放入一个字符
     *
     */
    result<details::unit_t, mstr_result_t> push(char ch) noexcept
    {
        mstr_result_t code = mstr_append(&this_obj, ch);
        if (MSTR_SUCC(code)) {
            return details::unit_t();
        }
        else {
            return code;
        }
    }

    /**
     * @brief 重复放入一个字符
     *
     */
    result<details::unit_t, mstr_result_t> push(
        char ch, std::size_t repeat
    ) noexcept
    {
        mstr_result_t code = mstr_repeat_append(&this_obj, ch, repeat);
        if (MSTR_SUCC(code)) {
            return details::unit_t();
        }
        else {
            return code;
        }
    }

    /**
     * @brief 放入一个字符串
     *
     */
    result<details::unit_t, mstr_result_t> concat(const string& rhs
    ) noexcept
    {
        mstr_result_t code = mstr_concat(&this_obj, &rhs.this_obj);
        if (MSTR_SUCC(code)) {
            return details::unit_t();
        }
        else {
            return code;
        }
    }

    /**
     * @brief 放入一个字符串 (c_str)
     *
     */
    template <std::size_t N>
    result<details::unit_t, mstr_result_t> concat(const char_t (&rhs)[N]
    ) noexcept
    {
        mstr_result_t code = mstr_concat_cstr(&this_obj, rhs);
        if (MSTR_SUCC(code)) {
            return details::unit_t();
        }
        else {
            return code;
        }
    }

    /**
     * @brief 字符串拼接 (self)
     *
     * @attention 无法完成操作会抛出异常
     *
     */
    string& operator+=(const string& rhs)
    {
        concat(rhs).or_exception([](error_code_t e) {
            return mtfmt_error(e);
        });
        return *this;
    }

    /**
     * @brief 字符串拼接 (self, c_str)
     *
     * @attention 无法完成操作会抛出异常
     *
     */
    template <std::size_t N> string& operator+=(const char_t (&rhs)[N])
    {
        concat(rhs).or_exception([](error_code_t e) {
            return mtfmt_error(e);
        });
        return *this;
    }

    /**
     * @brief 取得C风格字符串
     */
    void clear() noexcept
    {
        mstr_clear(&this_obj);
    }

    /**
     * @brief 取得C风格字符串
     *
     * @return const element_t*: c风格字符串指针吗
     */
    const char_t* as_cstr() noexcept
    {
        return mstr_as_cstr(&this_obj);
    }

    /**
     * @brief 取得std::string
     *
     */
    std::string as_std_string()
    {
        return std::string(this_obj.buff, this_obj.count);
    }

    /**
     * @brief 返回raw object
     *
     */
    inline MString& raw_object() noexcept
    {
        return this_obj;
    }
};
} // namespace mtfmt
#endif // _INCLUDE_MM_STRING_HPP_
