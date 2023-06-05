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
#include "mtfmt.h"
#include <cstddef>
#include <type_traits>
#include <utility>
namespace mtfmt
{
/**
 * @brief 返回值的错误结果
 *
 */
using error_code_t = mstr_result_t;

/**
 * @brief 结果类型的基类
 *
 */
template <typename T, typename E> class result_base
{
protected:
    /**
     * @brief 类型标签
     *
     */
    enum class TypeTag
    {
        SuccTag = 0,
        ErrorTag = 1
    };

    /**
     * @brief 类型标签
     *
     */
    TypeTag type_tag;

    union {
        T t_val;
        E e_val;
    };

public:
    constexpr result_base(T succ_val)
        : type_tag(TypeTag::SuccTag), t_val(succ_val)
    {
    }

    constexpr result_base(E err_val)
        : type_tag(TypeTag::ErrorTag), e_val(err_val)
    {
    }

    ~result_base()
    {
        if (type_tag == TypeTag::SuccTag) {
            t_val.~T();
        }
        else {
            e_val.~E();
        }
    }
};

/**
 * @brief 结果类
 *
 * @tparam T: succ时的结果
 * @tparam E: 失败时的结果
 */
template <typename T, typename E>
class result final : public result_base<T, E>
{
public:
    using base_t = result_base<T, E>;
    using base_t::base_t;

    /**
     * @brief 如果为T则返回T, 不然返回or_val
     *
     */
    inline T or_value(T or_val) const noexcept
    {
        if (base_t::type_tag == base_t::TypeTag::SuccTag) {
            return base_t::t_val;
        }
        else {
            return or_val;
        }
    }
};

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
    inline bool operator==(const string& str) const noexcept
    {
        return !!mstr_equal(&this_obj, &str.this_obj);
    }

    /**
     * @brief 相等(cstr)
     *
     */
    template <std::size_t N>
    inline bool operator==(const char_t (&str)[N]) const noexcept
    {
        MString obj;
        mstr_create(&obj, str);
        return !!mstr_equal(&this_obj, &obj);
    }

    /**
     * @brief 取得C风格字符串
     *
     * @return const element_t*: c风格字符串指针吗
     */
    inline const char_t* as_cstr() noexcept
    {
        return mstr_as_cstr(&this_obj);
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
        fmt_str,
        &str.raw_object(),
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
