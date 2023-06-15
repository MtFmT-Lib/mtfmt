// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_helper.hpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   helper
 * @version 1.0
 * @date    2023-06-15
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#if !defined(_INCLUDE_MM_HELPER_HPP_)
#define _INCLUDE_MM_HELPER_HPP_ 1
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

/**
 * @brief 判断Ti是否为模板Tt的实例化
 *
 */
template <template <typename...> typename Tt, typename Ti>
struct is_instance_of
{
    static constexpr bool value = false;
};

// 判断Ti是否为模板Tt的实例化
template <template <typename...> typename Tt, typename... Ti>
struct is_instance_of<Tt, Tt<Ti...>>
{
    static constexpr bool value = true;
};

/**
 * @brief 表示检查函数参数有多少个
 *
 */
template <typename F, std::size_t N> struct function_has_n_args
{
    static constexpr bool value = function_arg_count<F>::N == N;
};

/**
 * @brief 表示第n个参数应该拥有类型T
 *
 */
template <typename F, std::size_t N, typename T>
struct function_args_hold_type
{
    static constexpr bool value = std::is_same<
        // 取得第n个参数
        typename std::tuple_element<N, function_arg_tuple_t<F>>::type,
        // 并检查其拥有的类型
        T>::value;
};

/**
 * @brief 检查函数F的类型为 ( T1, T2 ...) -> R
 *
 */
template <typename F, typename R, typename... T> struct holds_prototype
{
    using _Ret = details::function_return_type_t<F>;
    using _Args = details::function_arg_tuple_t<F>;
    using Targs = std::tuple<typename std::decay<T>::type...>;

    static constexpr bool value =
        function_has_n_args<F, sizeof...(T)>::value &&
        std::is_same<_Ret, R>::value &&
        std::is_same<_Args, Targs>::value;
};
} // namespace details
} // namespace mtfmt
#endif // _INCLUDE_MM_HELPER_HPP_
