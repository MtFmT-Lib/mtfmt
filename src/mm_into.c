// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_into.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   格式化的函数实现
 * @version 1.0
 * @date    2023-03-24
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "mm_fmt.h"
#include "mm_typedef.h"
// #include "runtime_system.h"

static void div_mod_10(u32_t, u32_t*, u32_t*);
static void div_mod_10_u64(u64_t, u64_t*, u32_t*);
static mstr_result_t bcdtoa(MString*, u32_t, u32_t, u32_t, bool_t);
static mstr_result_t uqtoa_impl(MString*, u32_t, u32_t);
static mstr_result_t uqtoa_helper_dpart(MString*, u32_t, u32_t);
static mstr_result_t utoa_impl_2base(MString*, u32_t, char, u32_t);
static mstr_result_t utoa_impl_10base(MString*, u32_t);

MSTR_EXPORT_API(mstr_result_t)
mstr_fmt_uqtoa(MString* res_str, u32_t value, u32_t quat)
{
    if (quat > 31) {
        // 最高支持q31
        return MStr_Err_UnsupportQuantBits;
    }
    else {
        return uqtoa_impl(res_str, value, quat);
    }
}

MSTR_EXPORT_API(mstr_result_t)
mstr_fmt_utoa(MString* res_str, u32_t value, MStrFmtIntIndex index)
{
    mstr_result_t result = MStr_Ok;
    switch (index) {
    case MStrFmtIntIndex_Bin:
        result = utoa_impl_2base(res_str, 2, 'a', value);
        break;
    case MStrFmtIntIndex_Oct:
        result = utoa_impl_2base(res_str, 8, 'a', value);
        break;
    case MStrFmtIntIndex_Dec:
        result = utoa_impl_10base(res_str, value);
        break;
    case MStrFmtIntIndex_Hex:
        result = utoa_impl_2base(res_str, 16, 'a', value);
        break;
    case MStrFmtIntIndex_Hex_UpperCase:
        result = utoa_impl_2base(res_str, 16, 'A', value);
        break;
    }
    return result;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_fmt_ttoa(
    MString* res_str,
    const sys_time_t* tm,
    const MStrFmtChronoFormatSpec* spec
)
{
    usize_t i = 0;
    mstr_result_t result = MStr_Ok;
    for (; i < spec->item_cnt && MSTR_SUCC(result); i += 1) {
        const MStrFmtChronoItemFormatSpec* item = &spec->items[i];
        // 取得日期和时间的值
        u32_t value = 0;
        u32_t max_len = 0;
        switch (item->value_type) {
        case MStrFmtChronoValueType_Day:
            value = tm->day;
            max_len = 2;
            break;
        case MStrFmtChronoValueType_Year:
            value = tm->year;
            max_len = 4;
            break;
        case MStrFmtChronoValueType_Month:
            value = tm->month;
            max_len = 2;
            break;
        case MStrFmtChronoValueType_Hour24:
            value = tm->hour;
            max_len = 2;
            break;
        case MStrFmtChronoValueType_Minute:
            value = tm->minute;
            max_len = 2;
            break;
        case MStrFmtChronoValueType_Second:
            value = tm->second;
            max_len = 2;
            break;
        case MStrFmtChronoValueType_Hour:
            value = tm->hour;
            max_len = 2;
            // TODO 转为12h制
            break;
        case MStrFmtChronoValueType_Week:
            value = tm->week;
            max_len = 1;
            // TODO 转换星期名字
            break;
        case MStrFmtChronoValueType_SubSecond:
            value = tm->sub_second;
            max_len = 4;
            break;
        default: system_unreachable(); break;
        }
        // 格式化单个的项
        result = MSTR_AND_THEN(
            result,
            bcdtoa(
                res_str,
                value,
                item->value_spec.format_length,
                max_len,
                item->value_spec.fixed_length
            )
        );
        // 放入分隔符
        if (item->split_beg != item->split_end) {
            result = MSTR_AND_THEN(
                result,
                mstr_concat_cstr_slice(
                    res_str, item->split_beg, item->split_end
                )
            );
        }
    }
    return result;
}

/**
 * @brief utoa在index等于10的时候的实现
 *
 */
static mstr_result_t utoa_impl_10base(MString* str, u32_t value)
{
    mstr_result_t result = MStr_Ok;
    if (value == 0) {
        // 值是0
        result = MSTR_AND_THEN(result, mstr_append(str, '0'));
    }
    else {
        // 转换
        while (value > 0) {
            u32_t digit, next_value;
            div_mod_10(value, &next_value, &digit);
            value = next_value;
            // 转换为字符
            char ch = (char)('0' + digit);
            // push char
            result = MSTR_AND_THEN(result, mstr_append(str, ch));
            if (MSTR_FAILED(result)) {
                break;
            }
        }
        // 翻转转换结果
        result = MSTR_AND_THEN(result, mstr_reverse_self(str));
    }
    return result;
}

/**
 * @brief utoa在index等于2, 8, 16的时候的实现
 *
 * @note 使用 hex_base 来指定是从 'a' 开始序列化还是 'A' 开始
 */
static mstr_result_t utoa_impl_2base(
    MString* str, u32_t index, char hex_base, u32_t value
)
{
    mstr_result_t result = MStr_Ok;
    if (value == 0) {
        // 值是0
        result = MSTR_AND_THEN(result, mstr_append(str, '0'));
    }
    else {
        // ilog2
        u32_t shift = index == 2 ? 1 : index == 8 ? 3 : 4;
        // 转换
        while (value > 0) {
            u32_t digit = value & (index - 1);
            value = value >> shift;
            // 转换为字符
            char ch;
            if (digit >= 10) {
                ch = (char)(hex_base + digit - 10);
            }
            else {
                ch = (char)('0' + digit);
            }
            // push char
            result = MSTR_AND_THEN(result, mstr_append(str, ch));
            if (MSTR_FAILED(result)) {
                break;
            }
        }
        // 翻转转换结果
        result = MSTR_AND_THEN(result, mstr_reverse_self(str));
    }
    return result;
}

/**
 * @brief 量化值的格式化实现
 *
 * @attention 最大支持的quat值是31, 外部需要保证参数正确
 */
static mstr_result_t uqtoa_impl(MString* str, u32_t value, u32_t quat)
{
    mstr_result_t result = MStr_Ok;
    if (value == 0) {
        // 值是0
        result = MSTR_AND_THEN(result, mstr_append(str, '0'));
    }
    else {
        u32_t ipart = value >> quat;
        u32_t dpart = value & ((1 << quat) - 1);
        // 转换整数部分
        result = MSTR_AND_THEN(result, utoa_impl_10base(str, ipart));
        // 转换小数部分
        if (dpart > 0) {
            // 小数点
            result = MSTR_AND_THEN(result, mstr_append(str, '.'));
            // 小数部分
            result = MSTR_AND_THEN(
                result, uqtoa_helper_dpart(str, dpart, quat)
            );
        }
    }
    return result;
}

/**
 * @brief 转换量化值的小数部分
 *
 */
static mstr_result_t uqtoa_helper_dpart(
    MString* str, u32_t value, u32_t quat
)
{
    MString buff;
    mstr_result_t result = MStr_Ok;
    result = mstr_create_empty(&buff);
    if (value == 0) {
        // 值是0
        result = MSTR_AND_THEN(result, mstr_append(&buff, '0'));
    }
    else {
        // 转换值
        // 因为2^-1 = 0.5, 2^-2 = 0.25 ...
        // 也就是5, 25, 125 ...
        // 即5的幂次, 因此这尝试用5的幂次来完成计算
        // 这样子的话, 就可以避免 / 4096 什么的啦
        u32_t p5 = 1;
        u64_t acc = 0;
        u32_t mask = 1 << (quat - 1);
        while (mask > 0) {
            p5 *= 5;
            acc *= 10;
            if ((value & mask) == mask) {
                acc += p5;
            }
            mask >>= 1;
        }
        // 把累加出来的值变成字符串
        usize_t cv_sz = 0;
        bool_t tail_0 = True;
        u64_t dpart_value = acc;
        while (dpart_value > 0) {
            u32_t digit;
            u64_t next_value;
            div_mod_10_u64(dpart_value, &next_value, &digit);
            // 转换为字符
            char ch = (char)('0' + digit);
            // 消除后缀多余的0, 然后push char
            if (digit != 0 || !tail_0) {
                // push char
                result = MSTR_AND_THEN(result, mstr_append(&buff, ch));
                if (MSTR_FAILED(result)) {
                    break;
                }
                // 标记其不是0后缀
                tail_0 = False;
            }
            cv_sz += 1;
            dpart_value = next_value;
        }
        // 补全前导0
        usize_t fill_len = quat - cv_sz;
        while (fill_len > 0) {
            result = MSTR_AND_THEN(result, mstr_append(&buff, '0'));
            fill_len -= 1;
            if (MSTR_FAILED(result)) {
                break;
            }
        }
        // 翻转结果
        result = MSTR_AND_THEN(result, mstr_reverse_self(&buff));
    }
    // copy到输出
    result = MSTR_AND_THEN(result, mstr_concat(str, &buff));
    return result;
}

/**
 * @brief BCD值转换为字符串
 *
 * @param[out] str: 字符串输出
 * @param[in] bcd: BCD码, 最多16位(4个十进制位)
 * @param[in] len: 期望转换的长度, 1 ~ 4
 * @param[in] max_len: 预期最大进行的转换长度, 1 ~ 4
 * @param[in] fixed: 是否补0
 */
static mstr_result_t bcdtoa(
    MString* str, u32_t bcd, u32_t len, u32_t max_len, bool_t fixed
)
{
    usize_t counter = max_len;
    mstr_result_t result = MStr_Ok;
    while (MSTR_SUCC(result) && counter > 0) {
        counter -= 1;
        usize_t cur_pos = counter;
        if (fixed && cur_pos < max_len - len) {
            // 到达限定的位数了
            break;
        }
        // 转换当前位
        char ch;
        u32_t dec_val = (bcd >> (4 * cur_pos)) & 0xf;
        if (dec_val > 0) {
            ch = (char)(dec_val + '0');
            result = MSTR_AND_THEN(result, mstr_append(str, ch));
        }
        else if (fixed || len > cur_pos) {
            result = MSTR_AND_THEN(result, mstr_append(str, '0'));
        }
    }
    return result;
}

/**
 * @brief 返回: div = x / 10; rem = x % 10
 *
 */
static void div_mod_10(u32_t x, u32_t* div, u32_t* rem)
{
#if _MSTR_USE_HARDWARE_DIV
    *div = x / 10;
    *rem = x % 10;
#else
    u32_t q = (x >> 1) + (x >> 2);
    q += (q >> 4);
    q += (q >> 8);
    q += (q >> 16);
    // q = x * 0.8, 现在计算q / 8, 得到x * 0.1
    q >>= 3;
    // 计算结果
    u32_t r = x - (q * 10);
    if (r > 9) {
        *div = q + 1;
        *rem = r - 10;
    }
    else {
        *div = q;
        *rem = r;
    }
#endif // _MSTR_USE_HARDWARE_DIV
}

/**
 * @brief 返回: div = x / 10; rem = x % 10
 *
 */
static void div_mod_10_u64(u64_t x, u64_t* div, u32_t* rem)
{
#if _MSTR_USE_HARDWARE_DIV
    *div = (u64_t)(x / 10);
    *rem = (u32_t)(x % 10);
#else
    u64_t q = (x >> 1) + (x >> 2);
    q += (q >> 4);
    q += (q >> 8);
    q += (q >> 16);
    q += (q >> 32);
    // q = x * 0.8, 现在计算q / 8, 得到x * 0.1
    q >>= 3;
    // 计算结果
    u64_t r = x - (q * 10);
    if (r > 9) {
        *div = q + 1;
        *rem = (u32_t)((r - 10) & 0xffffffff);
    }
    else {
        *div = q;
        *rem = (u32_t)(r & 0xffffffff);
    }
#endif // _MSTR_USE_HARDWARE_DIV
}
