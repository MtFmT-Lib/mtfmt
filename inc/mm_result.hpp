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
    using err_t = E;
    using base_t = details::result_base<T, E>;
    using base_t::base_t;

    /**
     * @brief 是否为succ?
     *
     */
    bool is_succ() const noexcept
    {
        return base_t::type_tag == base_t::TypeTag::SuccTag;
    }

    /**
     * @brief 是否为err?
     *
     */
    bool is_err() const noexcept
    {
        return !is_succ();
    }

    /**
     * @brief 取得succ的值
     *
     * @attention 只有在 is_succ() 返回 true 时才是有意义的
     */
    succ_t unsafe_get_succ_value() const noexcept
    {
        return base_t::t_val;
    }

    /**
     * @brief 取得err的值
     *
     * @attention 只有在 is_err() 返回 true 时才是有意义的
     */
    err_t unsafe_get_err_value() const noexcept
    {
        return base_t::e_val;
    }

    /**
     * @brief 在self是succ的时候执行then do, 否则返回err
     *
     * @param then_do: 接下来要做的事情, 拥有类型 T -> result<R, E>
     */
    template <
        typename F,
        typename R = typename std::enable_if<
            true &&
                // 返回值应该是result的实例
                details::is_instance_of<
                    result,
                    details::function_return_type_t<F>>::value,
            typename details::function_return_type_t<F>::succ_t>::type>
    typename std::enable_if<
        // 函数拥有类型 T -> result<R, E>
        details::holds_prototype<F, result<R, E>, T>::value,
        result<R, E>>::type
        and_then(F then_do) const
    {
        if (is_succ()) {
            return then_do(base_t::t_val);
        }
        else {
            return base_t::e_val;
        }
    }

    /**
     * @brief 如果为T则返回T, 不然返回or_val
     *
     */
    T or_value(T or_val) const noexcept
    {
        if (is_succ()) {
            return base_t::t_val;
        }
        else {
            return or_val;
        }
    }

    /**
     * @brief 如果为T则返回T, 不然抛出异常
     *
     * @attention 抛出的异常必须继承自 std::exception
     */
    template <
        typename F,
        typename R = typename std::enable_if<
            // 抛出的异常继承自 std::exception
            std::is_base_of<
                std::exception,
                details::function_return_type_t<F>>::value,
            details::function_return_type_t<F>>::type>
    typename std::enable_if<
        // 函数拥有类型 E -> Exception
        details::holds_prototype<F, R, E>::value,
        T>::type
        or_exception(F cont) const
    {
        if (is_succ()) {
            return base_t::t_val;
        }
        else {
            throw cont(base_t::e_val);
        }
    }
};
} // namespace mtfmt
#endif // _INCLUDE_MM_RESULT_HPP_
