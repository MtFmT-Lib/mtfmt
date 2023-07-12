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
 * @brief 结果类型的stroager的types
 *
 */
template <typename... Targ> struct result_storager_type
{
    /**
     * @brief 存储的数据大小
     *
     */
    static constexpr std::size_t size =
        max_value<sizeof(Targ)...>::value;

    /**
     * @brief 存储的数据的内存对齐
     *
     */
    static constexpr std::size_t align =
        max_value<alignof(Targ)...>::value;

    /**
     * @brief 借助std storager实现数据存放
     *
     */
    using storager_t = typename std::aligned_storage<size, align>::type;
};

/**
 * @brief 结果类型的基类
 *
 */
template <typename T, typename E> class result_non_trivial_base
{
public:
    using value_type = T;
    using error_type = E;
    using const_reference_value_type = const T&;
    using const_reference_error_type = const E&;
    using storager_t = typename result_storager_type<T, E>::storager_t;

private:
    // TODO 暂时不提供 ...

    /**
     * @brief 从succ value构造
     *
     */
    result_non_trivial_base(T succ_val) : type_tag(TypeTag::SuccTag)
    {
    }

    /**
     * @brief 从err value构造
     *
     */
    result_non_trivial_base(E err_val) : type_tag(TypeTag::ErrorTag)
    {
    }

    ~result_non_trivial_base()
    {
    }

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

    void unsafe_set_succ_value(const T& value) noexcept
    {
        type_tag = TypeTag::SuccTag;
    }

    void unsafe_set_err_value(const E& value) noexcept
    {
        type_tag = TypeTag::ErrorTag;
    }

    const_reference_value_type unsafe_get_succ_value() const noexcept
    {
        mstr_assert(type_tag == TypeTag::SuccTag);
    }

    const_reference_error_type unsafe_get_err_value() const noexcept
    {
        mstr_assert(type_tag == TypeTag::ErrorTag);
    }

private:
    /**
     * @brief 数据存放在这里
     *
     * @note 这里用一个足够大的buffer实现union
     *
     */
    storager_t storager;
};

/**
 * @brief 结果类型的基类(trivial type)
 *
 * @note trivial type有copy ctor, copy assign等一系列的内容,
 * 所以不需要检查啦
 */
template <typename T, typename E> class result_trivial_base
{
    union {
        T t_val;
        E e_val;
    };

public:
    using value_type = T;
    using error_type = E;
    using const_reference_value_type = const T&;
    using const_reference_error_type = const E&;
    using storager_t = typename result_storager_type<T, E>::storager_t;

    /**
     * @brief 从succ value构造
     *
     */
    constexpr result_trivial_base(T succ_val)
        : t_val(succ_val), type_tag(TypeTag::SuccTag)
    {
    }

    /**
     * @brief 从err value构造
     *
     */
    constexpr result_trivial_base(E err_val)
        : e_val(err_val), type_tag(TypeTag::ErrorTag)
    {
    }

    // ctor为default, assign为default

    ~result_trivial_base()
    {
        if (type_tag == TypeTag::SuccTag) {
            t_val.~T();
        }
        else {
            e_val.~E();
        }
    }

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

    void unsafe_set_succ_value(const T& value) noexcept
    {
        t_val = value;
        type_tag = TypeTag::SuccTag;
    }

    void unsafe_set_err_value(const E& value) noexcept
    {
        e_val = value;
        type_tag = TypeTag::ErrorTag;
    }

    const_reference_value_type unsafe_get_succ_value() const noexcept
    {
        mstr_assert(type_tag == TypeTag::SuccTag);
        return t_val;
    }

    const_reference_error_type unsafe_get_err_value() const noexcept
    {
        mstr_assert(type_tag == TypeTag::ErrorTag);
        return e_val;
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
 * @brief 帮助判断T是trivial或者trivial result (case1)
 *
 */

template <typename T> struct is_trivial_or_trivial_result
{
    static constexpr bool value = std::is_trivial<T>::value;
};

/**
 * @brief 帮助判断T是trivial或者trivial result (case2)
 *
 */
template <typename... T>
struct is_trivial_or_trivial_result<result<T...>>
{
    static constexpr bool value =
        disjunction<is_trivial_or_trivial_result<T>::value...>::value;
};

/**
 * @brief 帮助判断result<T, E>是不是trivial
 *
 */
template <typename T, typename E> struct is_trivial_result
{
    static constexpr bool value =
        is_trivial_or_trivial_result<T>::value &&
        is_trivial_or_trivial_result<E>::value;
};

/**
 * @brief result的基类
 *
 */
template <typename T, typename E>
using result_base_t = typename std::conditional<
    is_trivial_result<T, E>::value,
    result_trivial_base<T, E>,
    result_non_trivial_base<T, E>>::type;

} // namespace details

/**
 * @brief 表示单位类型
 *
 */
using unit_t = details::unit_t;

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
class result final : public details::result_base_t<T, E>
{
public:
    using base_t = details::result_base_t<T, E>;
    using value_type = typename base_t::value_type;
    using error_type = typename base_t::error_type;
    using const_reference_value_type =
        typename base_t::const_reference_value_type;
    using const_reference_error_type =
        typename base_t::const_reference_error_type;
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
    const_reference_value_type unsafe_get_succ_value() const noexcept
    {
        return base_t::unsafe_get_succ_value();
    }

    /**
     * @brief 取得err的值
     *
     * @attention 只有在 is_err() 返回 true 时才是有意义的
     */
    const_reference_error_type unsafe_get_err_value() const noexcept
    {
        return base_t::unsafe_get_err_value();
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
        result<typename T1::value_type, E>>
        flatten() const noexcept
    {
        static_assert(std::is_same<typename T1::error_type, E>::value);
        if (is_succ()) {
            const auto& ref_succ = unsafe_get_succ_value();
            if (ref_succ.is_succ()) {
                return ref_succ.unsafe_get_succ_value();
            }
            else {
                return ref_succ.unsafe_get_err_value();
            }
        }
        else {
            return unsafe_get_err_value();
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
            return map_to(base_t::unsafe_get_succ_value());
        }
        else {
            return base_t::unsafe_get_err_value();
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
            return base_t::unsafe_get_succ_value();
        }
        else {
            return map_to(base_t::unsafe_get_err_value());
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
            typename R1::value_type>>
    details::enable_if_t<
        details::holds_prototype<F, result<R, E>, T>::value,
        result<R, E>>
        and_then(F then_do) const
    {
        if (is_succ()) {
            return then_do(base_t::unsafe_get_succ_value());
        }
        else {
            return base_t::unsafe_get_err_value();
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
            typename R1::error_type>>
    details::enable_if_t<
        details::holds_prototype<F, result<T, R>, E>::value,
        result<T, R>>
        or_else(F else_do) const
    {
        if (is_succ()) {
            return base_t::unsafe_get_succ_value();
        }
        else {
            return else_do(base_t::unsafe_get_err_value());
        }
    }

    /**
     * @brief 如果为T则返回T, 不然返回or_val
     *
     */
    T or_value(T or_val) const noexcept
    {
        if (is_succ()) {
            return base_t::unsafe_get_succ_value();
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
            return base_t::unsafe_get_succ_value();
        }
        else {
#if _MSTR_USE_CPP_EXCEPTION
            throw cont(base_t::unsafe_get_err_value());
#else
            (void)cont;
            mstr_cause_exception(base_t::unsafe_get_err_value());
#endif // _MSTR_USE_CPP_EXCEPTION
        }
    }
};
} // namespace mtfmt
#endif // _INCLUDE_MM_RESULT_HPP_
