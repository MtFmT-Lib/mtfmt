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
#include <exception>
#include <tuple>
#include <type_traits>
#include <utility>
namespace mtfmt
{
namespace details
{
/**
 * @brief 单位类型
 *
 */
struct unit_t
{
};

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
 * @brief 用于存放函数类型信息的东东
 */
template <typename Ret, typename... Args> struct function_traits_base
{
    //! 返回值类型
    using return_type_t = Ret;
    //! 参数数目
    static constexpr size_t NArgs = sizeof...(Args);
    //! 参数(元组)
    using arg_tuple_t = std::tuple<Args...>;
};

template <typename F> struct function_trait_impl;

template <typename F>
struct function_trait_impl<std::reference_wrapper<F>>
    : public function_trait_impl<F>
{
};

// for: function pointer
template <typename Ret, typename... Args>
struct function_trait_impl<Ret (*)(Args...)>
    : public function_traits_base<Ret, Args...>
{
};

// for: lambda函数
template <typename Ret, typename Cl, typename... Args>
struct function_trait_impl<Ret (Cl::*)(Args...)>
    : public function_traits_base<Ret, Args...>
{
};

// for: lambda函数(不可变capture)
template <typename Ret, typename Cl, typename... Args>
struct function_trait_impl<Ret (Cl::*)(Args...) const>
    : public function_traits_base<Ret, Args...>
{
};

// 除去如上三种情况的case
template <typename F>
struct function_trait_impl
    : public function_trait_impl<decltype(&F::operator())>
{
};

/**
 * @brief 取得函数的各种细节
 *
 */
template <typename F>
struct function_trait
    : public function_trait_impl<typename std::decay<F>::type>
{
};

/**
 * @brief 取得函数的返回值类型
 *
 */
template <typename F>
using function_return_type_t =
    typename function_trait<F>::return_type_t;

/**
 * @brief 取得函数的参数类型
 *
 */
template <typename F>
using function_arg_tuple_t = typename function_trait<F>::arg_tuple_t;

/**
 * @brief 取得函数的参数的个数
 *
 */
template <typename F> struct function_arg_count
{
    static constexpr std::size_t N = function_trait<F>::NArgs;
};

} // namespace details

/**
 * @brief 返回值的错误结果
 *
 */
using error_code_t = mstr_result_t;

/**
 * @brief 表示存在错误
 */
class mtfmt_error : std::exception
{
public:
    explicit mtfmt_error(error_code_t) : std::exception()
    {
    }
};

/**
 * @brief 结果类
 *
 * @tparam T: succ时的结果
 * @tparam E: 失败时的结果
 */
template <typename T, typename E>
class result final : public details::result_base<T, E>
{
public:
    using succ_t = T;
    using error_t = E;
    using base_t = details::result_base<T, E>;
    using base_t::base_t;

    /**
     * @brief 如果为T则返回T, 不然返回or_val
     *
     */
    T or_value(T or_val) const noexcept
    {
        if (base_t::type_tag == base_t::TypeTag::SuccTag) {
            return base_t::t_val;
        }
        else {
            return or_val;
        }
    }

    /**
     * @brief 如果为T则返回T, 不然抛出异常
     *
     */
    template <typename F>
    typename std::enable_if<
        // 参数个数必须是1个
        details::function_arg_count<F>::N == 1 &&
            // 第一个参数的类型应当和 类型E 一致
            std::is_same<
                typename std::tuple_element<
                    0,
                    details::function_arg_tuple_t<F>>::type,
                E>::value &&
            // 异常应该继承自std::exception
            std::is_base_of<
                std::exception,
                details::function_return_type_t<F>>::value,
        T>::type
        or_exception(F cont) const
    {
        if (base_t::type_tag == base_t::TypeTag::SuccTag) {
            return base_t::t_val;
        }
        else {
            throw cont(base_t::e_val);
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
