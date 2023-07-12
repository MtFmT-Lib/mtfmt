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
#include "helper.h"
#include "main.h"
#include "mtfmt.hpp"
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

    NonTrivialType(T val = 0) noexcept : value(val), mask(0)
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
};

using TrivialValue = int;
using NonTrivialValue = NonTrivialType<TrivialValue>;

extern "C" void monadic_result_object_basic(void)
{
    NonTrivialValue val_origin;
    ASSERT_ISSET_BIT(val_origin.get_mask(), 0);
    // copy ctor
    NonTrivialValue val_copy = val_origin;
    ASSERT_ISSET_BIT(
        val_copy.get_mask(), NonTrivialValue::MASK_COPY_CTOR
    );
    // move ctor
    NonTrivialValue val_move = std::move(NonTrivialValue{});
    ASSERT_ISSET_BIT(
        val_move.get_mask(), NonTrivialValue::MASK_MOVE_CTOR
    );
    // copy assign
    NonTrivialValue val_copy_assign;
    val_copy_assign = val_origin;
    ASSERT_ISSET_BIT(
        val_copy_assign.get_mask(), NonTrivialValue::MASK_COPY_ASSIGN
    );
    // move assign
    NonTrivialValue val_move_assign;
    val_move_assign = std::move(NonTrivialValue{});
    ASSERT_ISSET_BIT(
        val_move_assign.get_mask(), NonTrivialValue::MASK_MOVE_ASSIGN
    );
}

extern "C" void monadic_result_copy_trivial_type(void)
{
}

extern "C" void monadic_result_copy_non_trivial_type(void)
{
}

extern "C" void monadic_result_move_trivial_type(void)
{
}

extern "C" void monadic_result_move_non_trivial_type(void)
{
}

extern "C" void monadic_result_copy_assign_trivial_type(void)
{
}

extern "C" void monadic_result_copy_assign_non_trivial_type(void)
{
}

extern "C" void monadic_result_move_assign_trivial_type(void)
{
}

extern "C" void monadic_result_move_assign_non_trivial_type(void)
{
}
