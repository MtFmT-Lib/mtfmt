// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_monad_opers.cpp
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   Monadic result的cpp对象的那堆东东测试
 * @version 1.0
 * @date    2023-06-05
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "mtfmt.hpp"
#include "test_helper.h"
#include "test_main.h"
#include "unity.h"

/**
 * @brief 用于包装类型T, 让它变成一个non-trivial类型
 *
 */
template <typename T> class NonTrivialType final
{
    T value;

    uint32_t mask;

public:
    static constexpr uint32_t MASK_COPY_CTOR = 0x01;
    static constexpr uint32_t MASK_MOVE_CTOR = 0x02;
    static constexpr uint32_t MASK_COPY_ASSIGN = 0x04;
    static constexpr uint32_t MASK_MOVE_ASSIGN = 0x08;

    NonTrivialType(T val) noexcept : value(val), mask(0)
    {
    }

    ~NonTrivialType() = default;

    NonTrivialType(NonTrivialType&& r) noexcept
        : value(r.value), mask(r.mask | MASK_MOVE_CTOR)
    {
    }

    NonTrivialType(const NonTrivialType& r) noexcept
        : value(r.value), mask(r.mask | MASK_COPY_CTOR)
    {
    }

    NonTrivialType& operator=(NonTrivialType&& r) noexcept
    {
        value = r.value;
        mask = r.mask | MASK_MOVE_ASSIGN;
        return *this;
    }

    NonTrivialType& operator=(const NonTrivialType& r) noexcept
    {
        value = r.value;
        mask = r.mask | MASK_COPY_ASSIGN;
        return *this;
    }

    inline T get_value() const noexcept
    {
        return value;
    }

    inline uint32_t get_mask() const noexcept
    {
        return mask;
    }

    inline void clear_mask() noexcept
    {
        mask = 0;
    }
};

struct TrivialValue
{
    int value;
};

using NonTrivialValue = NonTrivialType<int>;

extern "C" void monadic_result_object_basic(void)
{
    NonTrivialValue val_origin = 0;
    ASSERT_ISSET_BIT(val_origin.get_mask(), 0);
    // copy ctor
    NonTrivialValue val_copy = val_origin;
    ASSERT_ISSET_BIT(
        val_copy.get_mask(), NonTrivialValue::MASK_COPY_CTOR
    );
    // move ctor
    NonTrivialValue val_move = std::move(NonTrivialValue{0});
    ASSERT_ISSET_BIT(
        val_move.get_mask(), NonTrivialValue::MASK_MOVE_CTOR
    );
    // copy assign
    NonTrivialValue val_copy_assign = 0;
    val_copy_assign = val_origin;
    ASSERT_ISSET_BIT(
        val_copy_assign.get_mask(), NonTrivialValue::MASK_COPY_ASSIGN
    );
    // move assign
    NonTrivialValue val_move_assign = 0;
    val_move_assign = std::move(NonTrivialValue{0});
    ASSERT_ISSET_BIT(
        val_move_assign.get_mask(), NonTrivialValue::MASK_MOVE_ASSIGN
    );
}

extern "C" void monadic_result_copy_non_trivial_type(void)
{
    using Result1 = mtfmt::result<NonTrivialValue, int>;
    using Result2 = mtfmt::result<int, NonTrivialValue>;
    // succ的情况
    NonTrivialValue val_origin_succ = 0;
    ASSERT_ISSET_BIT(val_origin_succ.get_mask(), 0);
    // 构造result
    Result1 res_succ_1 = val_origin_succ;
    ASSERT_EQUAL_VALUE(res_succ_1.is_succ(), true);
    ASSERT_EQUAL_VALUE(
        res_succ_1.unsafe_get_succ_value().get_value(), 0
    );
    // copy ctor
    res_succ_1.unsafe_get_succ_value_mut().clear_mask();
    Result1 res_succ_2 = res_succ_1;
    ASSERT_EQUAL_VALUE(res_succ_2.is_succ(), true);
    ASSERT_ISSET_BIT(
        res_succ_2.unsafe_get_succ_value().get_mask(),
        NonTrivialValue::MASK_COPY_CTOR
    );
    // fail case
    Result1 res_fail_1 = 1;
    ASSERT_EQUAL_VALUE(res_fail_1.is_err(), true);
    ASSERT_EQUAL_VALUE(res_fail_1.unsafe_get_err_value(), 1);
    // copy ctor
    Result1 res_fail_2 = res_fail_1;
    ASSERT_EQUAL_VALUE(res_fail_2.is_err(), true);
    // 构造result (fail)
    NonTrivialValue val_origin_err = 0;
    ASSERT_ISSET_BIT(val_origin_err.get_mask(), 0);
    // 构造result
    Result2 res_err_1 = val_origin_err;
    ASSERT_EQUAL_VALUE(res_err_1.is_err(), true);
    ASSERT_EQUAL_VALUE(res_err_1.unsafe_get_err_value().get_value(), 0);
    // copy ctor
    res_err_1.unsafe_get_err_value_mut().clear_mask();
    Result2 res_err_2 = res_err_1;
    ASSERT_EQUAL_VALUE(res_err_2.is_err(), true);
    ASSERT_ISSET_BIT(
        res_err_2.unsafe_get_err_value().get_mask(),
        NonTrivialValue::MASK_COPY_CTOR
    );
}

extern "C" void monadic_result_copy_assign_non_trivial_type(void)
{
    using Result1 = mtfmt::result<NonTrivialValue, int>;
    using Result2 = mtfmt::result<int, NonTrivialValue>;
    // succ的情况
    NonTrivialValue val_origin_succ = 0;
    ASSERT_ISSET_BIT(val_origin_succ.get_mask(), 0);
    // 构造result
    Result1 res_succ_1 = val_origin_succ;
    ASSERT_EQUAL_VALUE(res_succ_1.is_succ(), true);
    ASSERT_EQUAL_VALUE(
        res_succ_1.unsafe_get_succ_value().get_value(), 0
    );
    res_succ_1.unsafe_get_succ_value_mut().clear_mask();
    // result 2
    Result1 res_succ_2 = 1;
    ASSERT_EQUAL_VALUE(res_succ_2.is_succ(), false);
    // copy result_1
    res_succ_2 = res_succ_1;
    ASSERT_EQUAL_VALUE(res_succ_2.is_succ(), true);
    ASSERT_EQUAL_VALUE(
        res_succ_2.unsafe_get_succ_value().get_value(), 0
    );
    // copy 的实现其实是靠 copy ctor 做的, 不是 copy assign
    ASSERT_ISSET_BIT(
        res_succ_2.unsafe_get_succ_value().get_mask(),
        NonTrivialValue::MASK_COPY_CTOR
    );
    // error 的情况
    NonTrivialValue val_origin_err = 0;
    ASSERT_ISSET_BIT(val_origin_err.get_mask(), 0);
    // 构造result
    Result2 res_err_1 = val_origin_err;
    ASSERT_EQUAL_VALUE(res_err_1.is_err(), true);
    ASSERT_EQUAL_VALUE(res_err_1.unsafe_get_err_value().get_value(), 0);
    res_err_1.unsafe_get_err_value_mut().clear_mask();
    // result 2
    Result2 res_err_2 = 1;
    ASSERT_EQUAL_VALUE(res_err_2.is_err(), false);
    // copy result_1
    res_err_2 = res_err_1;
    ASSERT_EQUAL_VALUE(res_err_2.is_err(), true);
    ASSERT_EQUAL_VALUE(res_err_2.unsafe_get_err_value().get_value(), 0);
    // copy 的实现其实是靠 copy ctor 做的, 不是 copy assign
    ASSERT_ISSET_BIT(
        res_err_2.unsafe_get_err_value().get_mask(),
        NonTrivialValue::MASK_COPY_CTOR
    );
}
