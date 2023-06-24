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
// class decl
template <typename T, typename E> class result;

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
    using succ_t = T;
    using err_t = E;

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
 * @brief 判断T是不是一个result类型
 *
 */
template <typename T> struct is_result
{
    static constexpr bool value =
        details::is_instance_of<result, T>::value;
};

/**
 * @brief 继承该类型为result增加合适的拷贝, 移动constructor/assign
 *
 */
template <typename T, typename E>
struct result_add_ctrl : result_base<T, E>
{
    using base_t = details::result_base<T, E>;
    using base_t::base_t;

    // TODO
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
class result final : public details::result_add_ctrl<T, E>
{
public:
    using base_t = details::result_add_ctrl<T, E>;
    using succ_t = typename base_t::succ_t;
    using err_t = typename base_t::err_t;
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
     * @brief 交换succ和err的情况以及值
     *
     */
    result<E, T> conjugate() const noexcept
    {
        if (is_succ()) {
            return unsafe_get_succ_value();
        }
        else {
            return unsafe_get_err_value();
        }
    }

    /**
     * @brief 扁平化
     *
     * @note 将result<result<T, E>, E>变成result<T, E>,
     * 当内部为err的时候, 输出是err, 内部是succ的时候, 返回succ
     */
    template <typename T1 = T>
    details::enable_if_t<
        details::is_result<T1>::value,
        result<typename T1::succ_t, E>>
        flatten() const noexcept
    {
        static_assert(std::is_same<typename T1::err_t, E>::value);
        if (is_succ()) {
            const auto& ref_succ = base_t::t_val;
            if (ref_succ.is_succ()) {
                return ref_succ.unsafe_get_succ_value();
            }
            else {
                return ref_succ.unsafe_get_err_value();
            }
        }
        else {
            return base_t::e_val;
        }
    }

    /**
     * @brief 扁平化 (T不是result<T, E>的情况)
     *
     */
    template <typename T1 = T>
    details::enable_if_t<!details::is_result<T1>::value, result<T1, E>>
        flatten() const noexcept
    {
        return *this;
    }

    /**
     * @brief 在result是succ的时候执行map, 转换result
     *
     * @param[in] map_to: 映射函数, 拥有类型 T -> R
     *
     */
    template <
        typename F,
        typename R = details::function_return_type_t<F>>
    details::enable_if_t<
        details::holds_prototype<F, R, T>::value,
        result<R, E>>
        map(F map_to) const
    {
        if (is_succ()) {
            return map_to(base_t::t_val);
        }
        else {
            return base_t::e_val;
        }
    }

    /**
     * @brief 在result是err的时候执行map, 转换result
     *
     * @param[in] map_to: 映射函数, 拥有类型 E -> R
     */
    template <
        typename F,
        typename R = details::function_return_type_t<F>>
    details::enable_if_t<
        details::holds_prototype<F, R, E>::value,
        result<T, R>>
        map_err(F map_to) const
    {
        if (is_succ()) {
            return base_t::t_val;
        }
        else {
            return map_to(base_t::e_val);
        }
    }

    /**
     * @brief 在self是succ的时候执行then do, 否则返回err
     *
     * @param then_do: 接下来要做的事情, 拥有类型 T -> result<R, E>
     */
    template <
        typename F,
        typename R1 = details::function_return_type_t<F>,
        typename R = details::enable_if_t<
            details::is_result<R1>::value,
            typename R1::succ_t>>
    details::enable_if_t<
        details::holds_prototype<F, result<R, E>, T>::value,
        result<R, E>>
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
     * @brief 在self是err的时候执行else_do, 否则返回succ
     *
     * @param[in] else_do: 接下来要做的事情, 拥有类型 E -> result<R, E>
     */
    template <
        typename F,
        typename R1 = details::function_return_type_t<F>,
        typename R = details::enable_if_t<
            details::is_result<R1>::value,
            typename R1::err_t>>
    details::enable_if_t<
        details::holds_prototype<F, result<T, R>, E>::value,
        result<T, R>>
        or_else(F else_do) const
    {
        if (is_succ()) {
            return base_t::t_val;
        }
        else {
            return else_do(base_t::e_val);
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
     * @attention 抛出的异常必须继承自 std::exception, 如果未启用异常,
     * 会触发assert(false)
     */
    template <
        typename F,
        typename R1 = details::function_return_type_t<F>,
        typename R = details::
            enable_if_t<std::is_base_of<std::exception, R1>::value, R1>>
    details::enable_if_t<details::holds_prototype<F, R, E>::value, T>
        or_exception(F cont) const
    {
        if (is_succ()) {
            return base_t::t_val;
        }
        else {
#if _MSTR_USE_CPP_EXCEPTION
            throw cont(base_t::e_val);
#else
            (void)cont;
            mstr_cause_exception(base_t::e_val);
#endif // _MSTR_USE_CPP_EXCEPTION
        }
    }
};
} // namespace mtfmt
#endif // _INCLUDE_MM_RESULT_HPP_
