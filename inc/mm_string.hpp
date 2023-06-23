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
#include <array>
#include <cstddef>
#include <iterator>
#include <string.h>
#include <string>
namespace mtfmt
{
/**
 * @brief unicode字符
 *
 */
using unicode_t = mstr_codepoint_t;

namespace details
{
#if _MSTR_USE_UTF_8
template <std::size_t N>
constexpr uint32_t utf8_meta(
    const mstr_char_t (&u8char)[N], std::size_t idx, uint32_t mask
)
{
    // 取得utf-8编码有效的部份: u8char[idx] & mask
    return static_cast<uint32_t>(
        static_cast<uint8_t>(u8char[idx]) & mask
    );
}

template <std::size_t N>
constexpr typename std::enable_if<N == 3, unicode_t>::type unicode_char(
    const mstr_char_t (&u8char)[N]
)
{
    return (utf8_meta(u8char, 1, 0x3f)) |
           (utf8_meta(u8char, 0, 0x1f) << 6);
}

template <std::size_t N>
constexpr typename std::enable_if<N == 4, unicode_t>::type unicode_char(
    const mstr_char_t (&u8char)[N]
)
{
    return (utf8_meta(u8char, 2, 0x3f)) |
           (utf8_meta(u8char, 1, 0x3f) << 6) |
           (utf8_meta(u8char, 0, 0x0f) << 12);
}

template <std::size_t N>
constexpr typename std::enable_if<N == 5, unicode_t>::type unicode_char(
    const mstr_char_t (&u8char)[N]
)
{
    return (utf8_meta(u8char, 3, 0x3f)) |
           (utf8_meta(u8char, 2, 0x3f) << 6) |
           (utf8_meta(u8char, 1, 0x3f) << 12) |
           (utf8_meta(u8char, 0, 0x07) << 18);
}
#endif // _MSTR_USE_UTF_8

/**
 * @brief 转为unicode char (<=1的情况)
 *
 */
template <std::size_t N>
constexpr typename std::enable_if<N <= 2, unicode_t>::type unicode_char(
    const mstr_char_t (&u8char)[N]
)
{
    return u8char[0];
}

/**
 * @brief 字符串迭代器
 *
 */
class string_iterator
{
    /**
     * @brief 位置
     *
     */
    const char* it;

    /**
     * @brief 剩余的长度
     *
     */
    usize_t rem_length;

public:
    using value_type = unicode_t;
    using pointer = const value_type*;
    using reference = const value_type&;
    using difference_type = iptr_t;
    using iterator_category = std::forward_iterator_tag;

    string_iterator(const mstr_char_t* buff, usize_t leng)
    {
        it = buff;
        rem_length = leng;
    }

    string_iterator(string_iterator&&) = default;
    string_iterator(const string_iterator&) = default;
    string_iterator& operator=(string_iterator&&) = default;
    string_iterator& operator=(const string_iterator&) = default;

    value_type operator*() const
    {
        usize_t len = mstr_char_length(*it);
        mstr_codepoint_t code;
        mstr_result_t res = mstr_codepoint_of(&code, it, len);
        if (MSTR_FAILED(res)) {
            throw mtfmt_error(MStr_Err_UnicodeEncodingError);
        }
        return code;
    }

    string_iterator& operator++()
    {
        usize_t len = mstr_char_length(*it);
        it += len;
        rem_length -= 1;
        return *this;
    }

    string_iterator operator++(int)
    {
        string_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    bool operator==(const string_iterator& rhs) const noexcept
    {
        return it == rhs.it;
    }

    bool operator!=(const string_iterator& rhs) const noexcept
    {
        return it != rhs.it;
    }
};

/**
 * @brief 字符串迭代器(mut)
 *
 */
class string_iterator_mut
{
    /**
     * @brief 位置
     *
     */
    char* it;

    /**
     * @brief 剩余的长度
     *
     */
    usize_t rem_length;

public:
    using value_type = unicode_t;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = iptr_t;
    using iterator_category = std::forward_iterator_tag;

    string_iterator_mut(mstr_char_t* buff, usize_t leng)
    {
        it = buff;
        rem_length = leng;
    }

    string_iterator_mut(string_iterator_mut&&) = default;
    string_iterator_mut(const string_iterator_mut&) = default;
    string_iterator_mut& operator=(string_iterator_mut&&) = default;
    string_iterator_mut& operator=(const string_iterator_mut&) =
        default;

    value_type operator*() const
    {
        usize_t len = mstr_char_length(*it);
        mstr_codepoint_t code;
        mstr_result_t res = mstr_codepoint_of(&code, it, len);
        if (MSTR_FAILED(res)) {
            throw mtfmt_error(MStr_Err_UnicodeEncodingError);
        }
        return code;
    }

    string_iterator_mut& operator++()
    {
        usize_t len = mstr_char_length(*it);
        it += len;
        rem_length -= 1;
        return *this;
    }

    string_iterator_mut operator++(int)
    {
        string_iterator_mut tmp = *this;
        ++*this;
        return tmp;
    }

    bool operator==(const string_iterator_mut& rhs) const noexcept
    {
        return it == rhs.it;
    }

    bool operator!=(const string_iterator_mut& rhs) const noexcept
    {
        return it != rhs.it;
    }
};

} // namespace details

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
    using value_t = mstr_char_t;
    using pointer = value_t*;
    using const_pointer = const value_t*;
    using reference = value_t&;
    using const_reference = const value_t&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using iterator = details::string_iterator_mut;
    using const_iterator = details::string_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator =
        std::reverse_iterator<const_iterator>;

    using repeat_char_t = std::tuple<unicode_t, std::size_t>;

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
    string(const value_t* c_str)
    {
        mstr_create(&this_obj, c_str);
    }

    /**
     * @brief 从std::string创建
     *
     * @param str: str
     */
    string(const std::string& str)
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
     * @brief 取得字符串长度
     *
     */
    usize_t length() const noexcept
    {
        return this_obj.length;
    }

    /**
     * @brief 取得字符串占用的字节数
     *
     */
    usize_t byte_count() const noexcept
    {
        return this_obj.count;
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
    bool operator==(const value_t (&str)[N]) const noexcept
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
        const value_t (&str)[N], const string& pthis
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
    bool operator!=(const value_t (&str)[N]) const noexcept
    {
        return !(*this == str);
    }

    /**
     * @brief 不等(cstr left)
     *
     */
    template <std::size_t N>
    friend bool operator!=(
        const value_t (&str)[N], const string& pthis
    ) noexcept
    {
        return !(pthis == str);
    }

    /**
     * @brief 判断字符串是否以另一个字串开始(c_str buffer)
     *
     */
    template <std::size_t N>
    bool start_with(const value_t (&prefix)[N]) const noexcept
    {
        return !!mstr_start_with(&this_obj, prefix, N);
    }

    /**
     * @brief 判断字符串是否以另一个字串开始(c_str)
     *
     */
    bool start_with(const char* prefix) const noexcept
    {
        return !!mstr_start_with(&this_obj, prefix, strlen(prefix));
    }

    /**
     * @brief 判断字符串是否以另一个字串开始(MString)
     *
     */
    bool start_with(const string* prefix) const noexcept
    {
        const char* buff = prefix->this_obj.buff;
        usize_t buff_len = prefix->this_obj.count;
        return !!mstr_start_with(&this_obj, buff, buff_len);
    }

    /**
     * @brief 判断字符串是否以另一个字串结束(c_str buffer)
     *
     */
    template <std::size_t N>
    bool end_with(const value_t (&suffix)[N]) const noexcept
    {
        return !!mstr_end_with(&this_obj, suffix, N);
    }

    /**
     * @brief 判断字符串是否以另一个字串结束(c_str)
     *
     */
    bool end_with(const char* suffix) const noexcept
    {
        return !!mstr_end_with(&this_obj, suffix, strlen(suffix));
    }

    /**
     * @brief 判断字符串是否以另一个字串结束(MString)
     *
     */
    bool end_with(const string* suffix) const noexcept
    {
        const char* buff = suffix->this_obj.buff;
        usize_t buff_len = suffix->this_obj.count;
        return !!mstr_end_with(&this_obj, buff, buff_len);
    }

    /**
     * @brief 放入一个字符
     *
     */
    result<details::unit_t, mstr_result_t> push(
        mstr_codepoint_t uni_char
    ) noexcept
    {
        mstr_result_t code = mstr_append(&this_obj, uni_char);
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
        mstr_codepoint_t ch, std::size_t repeat
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
    result<details::unit_t, mstr_result_t> concat(const value_t (&rhs
    )[N]) noexcept
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
    template <std::size_t N> string& operator+=(const value_t (&rhs)[N])
    {
        concat(rhs).or_exception([](error_code_t e) {
            return mtfmt_error(e);
        });
        return *this;
    }

    /**
     * @brief 字符串拼接 (字符)
     *
     * @attention 无法完成操作会抛出异常
     *
     */
    string& operator+=(unicode_t rhs)
    {
        push(rhs).or_exception([](error_code_t e) {
            return mtfmt_error(e);
        });
        return *this;
    }

    /**
     * @brief 字符串拼接 (重复n个)
     *
     * @attention 无法完成操作会抛出异常
     *
     */
    string& operator+=(const repeat_char_t& rhs)
    {
        auto ch = std::get<0>(rhs);
        auto cnt = std::get<1>(rhs);
        push(ch, cnt).or_exception([](error_code_t e) {
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
     * @brief 翻转字符串
     *
     */
    void reverse() noexcept
    {
        mstr_reverse_self(&this_obj);
    }

    /**
     * @brief 取得C风格字符串
     *
     * @return const element_t*: c风格字符串指针吗
     */
    const value_t* as_cstr() noexcept
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
     * @brief 取得迭代器起始
     *
     */
    iterator begin() noexcept
    {
        return iterator(this_obj.buff, this_obj.length);
    }

    /**
     * @brief 取得迭代器起始 (const)
     *
     */
    const_iterator begin() const noexcept
    {
        return const_iterator(this_obj.buff, this_obj.length);
    }

    /**
     * @brief 取得迭代器结束
     *
     */
    iterator end() noexcept
    {
        return iterator(this_obj.buff + this_obj.count, 0);
    }

    /**
     * @brief 取得迭代器结束 (const)
     *
     */
    const_iterator end() const noexcept
    {
        return const_iterator(this_obj.buff + this_obj.count, 0);
    }

    /**
     * @brief 取得第n个unicode字符
     *
     * @attention 该重载直接返回Unicode代码点
     *            因utf-8编码是变长的, 不支持修改
     */
    unicode_t operator[](std::size_t i) const
    {
        unicode_t code = mstr_char_at(&this_obj, i);
        if (code == 0) {
            throw mtfmt_error(MStr_Err_IndexOutOfBound);
        }
        return code;
    }

    /**
     * @brief 取得Unicode代码点的字符
     *
     * @param u8char: utf-8字符串
     *
     */
    template <std::size_t N>
    constexpr static unicode_t unicode_char(const value_t (&u8char)[N])
    {
        return details::unicode_char(u8char);
    }

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
    static result<string, error_code_t> format(
        const string::value_t* fmt_str, Args&&... args
    )
    {
        string str;
        error_code_t code = mstr_format(
            &str.raw_object_mut(),
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

protected:
    /**
     * @brief 返回raw object
     *
     */
    const MString& raw_object() const noexcept
    {
        return this_obj;
    }

    /**
     * @brief 返回raw object(可变的)
     *
     */
    MString& raw_object_mut() noexcept
    {
        return this_obj;
    }
};

namespace literals
{
/**
 * @brief 从c字符串指针构造mtfmt::string
 *
 */
inline string operator""_ms(const char* str)
{
    return string(str);
}
} // namespace literals

} // namespace mtfmt
#endif // _INCLUDE_MM_STRING_HPP_
