// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_result.hpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   monadic result
 * @version 1.0
 * @date    2023-06-15
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#if !defined(_INCLUDE_MM_RESULT_HPP_)
#define _INCLUDE_MM_RESULT_HPP_ 1
#include "mm_helper.hpp"
#include "mm_result.h"
namespace mtfmt
{
namespace details
{
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
} // namespace mtfmt
#endif // _INCLUDE_MM_RESULT_HPP_
