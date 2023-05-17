// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_fmt.h
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串格式化
 * @version 1.0
 * @date    2023-03-21
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#if !defined(_INCLUDE_MM_FMT_H_)
#define _INCLUDE_MM_FMT_H_
#include "mm_cfg.h"
#include "mm_result.h"
#include "mm_string.h"
#include "mm_typedef.h"

//! sizeof(MStrFmtParserState)
#define MFMT_PARSER_STATE_SIZE    32

//! 格式化最多支持的参数个数
#define MFMT_PLACE_MAX_NUM        16

//! 格式化最多支持的WIDTH
#define MFMT_PLACE_MAX_WIDTH      32

//! 日期和时间最多支持的placement数目
#define MFMT_CHRONO_PLACE_MAX_NUM 8

//
// 这个东东实现了一个超级小的支持类python的format语法的格式化器
// 并且提供了一些其它的格式化函数
//

/**
 * @brief 分析器状态
 *
 */
typedef struct tagMStrFmtParserState MStrFmtParserState;

/**
 * @brief 指定参数的分类
 *
 */
typedef enum tagMStrFmtArgClass
{
    //! 表示该位置是一个值
    MStrFmtArgClass_Value,

    //! 表示该位置是一个数组
    MStrFmtArgClass_Array,

    //! 表示该位置是一个转义字符
    MStrFmtArgClass_EscapeChar,
} MStrFmtArgClass;

/**
 * @brief 格式化参数的类型
 *
 */
typedef enum tagMStrFmtArgType
{
    //! 未知参数类型
    MStrFmtArgType_Unknown,

    //! 8位整数值
    MStrFmtArgType_Int8,

    //! 16位整数值
    MStrFmtArgType_Int16,

    //! 32位整数值
    MStrFmtArgType_Int32,

    //! 无符号8位整数值
    MStrFmtArgType_Uint8,

    //! 无符号16位整数值
    MStrFmtArgType_Uint16,

    //! 无符号32位整数值
    MStrFmtArgType_Uint32,

    //! C字符串指针
    MStrFmtArgType_CString,

    //! 时间
    MStrFmtArgType_Time,

    //! 量化值
    MStrFmtArgType_QuantizedValue,

    //! 无符号量化值
    MStrFmtArgType_QuantizedUnsignedValue,

    //! Array type, 要和上面的值顺序一致
    MStrFmtArgType_Array_Bit = 0x1000,

    //! 8位整数数组(`const i8_t*`)
    MStrFmtArgType_Array_Int8,

    //! 16位整数数组(`const i16_t*`)
    MStrFmtArgType_Array_Int16,

    //! 32位整数数组(`const i32_t*`)
    MStrFmtArgType_Array_Int32,

    //! 无符号8位整数数组(`const u8_t*`)
    MStrFmtArgType_Array_Uint8,

    //! 无符号16位整数数组(`const u16_t*`)
    MStrFmtArgType_Array_Uint16,

    //! 无符号32位整数数组(`const u32_t*`)
    MStrFmtArgType_Array_Uint32,

    //! C字符串指针组成的数组
    MStrFmtArgType_Array_CString,

    //! 时间结构体组成的数组
    MStrFmtArgType_Array_Time,

    //! 量化值数组
    MStrFmtArgType_Array_QuantizedValue,

    //! 无符号量化值数组
    MStrFmtArgType_Array_QuantizedUnsignedValue,
} MStrFmtArgType;

/**
 * @brief 格式化参数的对齐方式
 *
 */
typedef enum tagMStrFmtAlign
{
    //! 左对齐, 在右侧填充内容
    MStrFmtAlign_Left,

    //! 居中, 在两侧填充内容
    MStrFmtAlign_Center,

    //! 右对齐, 在左侧填充内容
    MStrFmtAlign_Right,
} MStrFmtAlign;

/**
 * @brief 格式化值时候的符号显示方式
 *
 */
typedef enum tagMStrFmtSignDisplay
{
    //! 总是显示符号
    MStrFmtSignDisplay_Always,

    //! 仅在值小于0时显示符号
    MStrFmtSignDisplay_NegOnly,

    //! 在小于0时显示负号, 大于0时显示为空格
    MStrFmtSignDisplay_Neg_Or_Space,
} MStrFmtSignDisplay;

/**
 * @brief 进行格式化的方式
 *
 */
typedef enum tagMStrFmtFormatType
{
    //! 二进制值字符串
    MStrFmtFormatType_Binary,

    //! 八进制值字符串
    MStrFmtFormatType_Oct,

    //! 十进制值字符串
    MStrFmtFormatType_Deciaml,

    //! 小写十六进制值字符串
    MStrFmtFormatType_Hex,

    //! 大写十六进制值字符串
    MStrFmtFormatType_Hex_UpperCase,

    //! 小写十六进制值, 带"0x"前缀
    MStrFmtFormatType_Hex_WithPrefix,

    //! 大写十六进制值, 带"0X"前缀
    MStrFmtFormatType_Hex_UpperCase_WithPrefix,

    //! 未指定的转换方式
    MStrFmtFormatType_UnSpec,
} MStrFmtFormatType;

/**
 * @brief 转换整数时采用的进制
 *
 */
typedef enum tagMStrFmtIntIndex
{
    //! 转换为二进制字符串
    MStrFmtIntIndex_Bin,

    //! 转换为八进制字符串
    MStrFmtIntIndex_Oct,

    //! 转换为十进制字符串
    MStrFmtIntIndex_Dec,

    //! 转换为小写的十六进制值字符串
    MStrFmtIntIndex_Hex,

    //! 转换为大写的十六进制值字符串
    MStrFmtIntIndex_Hex_UpperCase,
} MStrFmtIntIndex;

/**
 * @brief 一般变量的格式化描述标记
 *
 */
typedef struct tagMStrFmtValueFormatSpec
{
    int reserved;
} MStrFmtValueFormatSpec;

/**
 * @brief 日期和时间的值的格式化描述
 *
 */
typedef struct tagMStrFmtChronoValueFormatSpec
{
    //! 是否以固定长度进行格式化
    bool_t fixed_length;

    //! 进行格式化的长度
    u8_t format_length;
} MStrFmtChronoValueFormatSpec;

/**
 * @brief 日期和时间的值类型
 *
 */
typedef enum tagMStrFmtChronoValueType
{
    //! 年
    MStrFmtChronoValueType_Year,

    //! 月
    MStrFmtChronoValueType_Month,

    //! 日
    MStrFmtChronoValueType_Day,

    //! 时 (24h制)
    MStrFmtChronoValueType_Hour24,

    //! 分
    MStrFmtChronoValueType_Minute,

    //! 秒
    MStrFmtChronoValueType_Second,

    //! 子秒
    MStrFmtChronoValueType_SubSecond,

    //! 时 (12h制)
    MStrFmtChronoValueType_Hour,

    //! 星期
    MStrFmtChronoValueType_Week,
} MStrFmtChronoValueType;

/**
 * @brief 时间的格式化描述标记的项
 *
 */
typedef struct tagMStrFmtChronoItemFormatSpec
{
    //! 值的格式化类型
    MStrFmtChronoValueType value_type;

    //! 值的格式化描述
    MStrFmtChronoValueFormatSpec value_spec;

    //! 该值后面的split chars, 可以为空, 引用自fmt, [beg,
    //! end)是需要的split char
    const char* split_beg;

    //! split chars结束位置
    const char* split_end;
} MStrFmtChronoItemFormatSpec;

/**
 * @brief 时间的格式化描述标记
 *
 */
typedef struct tagMStrFmtChronoFormatSpec
{
    //! 需要格式化的值(从左到右)
    MStrFmtChronoItemFormatSpec items[MFMT_CHRONO_PLACE_MAX_NUM];

    //! 项长度
    usize_t item_cnt;
} MStrFmtChronoFormatSpec;

/**
 * @brief 格式化标记
 *
 */
typedef struct tagMStrFmtFormatSpec
{
    //! 格式化的值类型
    MStrFmtFormatType fmt_type;

    union {
        //! [type: 非Time] 进行格式化的方式, 默认情况为未指定
        MStrFmtValueFormatSpec value_spec;
        //! [type: Time] 进行格式化的方式
        MStrFmtChronoFormatSpec chrono_spec;
    };
} MStrFmtFormatSpec;

/**
 * @brief 分析器的格式化标记
 *
 */
typedef struct tagMStrFmtFormatDescript
{
    //! 填充字符, 默认为空格
    char fill_char;

    //! 对齐方式
    MStrFmtAlign fmt_align;

    //! 符号的显示方式, 默认是Display Only Neg
    MStrFmtSignDisplay sign_display;

    //! 格式化宽度, -1表示使用默认值
    i32_t width;

    //! 格式化标记
    MStrFmtFormatSpec fmt_spec;
} MStrFmtFormatDescript;

/**
 * @brief 解析过程中的参数额外属性
 *
 * @note 对于量化值, a表示量化精度, 对于定点数, a表示整数部分精度,
 * b表示小数部分精度
 */
typedef struct tagMStrFmtArgProperty
{
    u32_t a, b;
} MStrFmtArgProperty;

/**
 * @brief 解析器结果
 *
 */
typedef struct tagMStrFmtParseResult
{
    //! 参数分类
    MStrFmtArgClass arg_class;
    union {
        //! [MStrFmtArgClass: EscapeChar] 转义字符
        char escape_char;
        //! [MStrFmtArgClass: Value] 格式化信息
        struct
        {
            //! 参数位置
            u32_t arg_id;
            //! 参数类型
            MStrFmtArgType arg_type;
            //! 附带参数
            MStrFmtArgProperty arg_prop;
            //! 格式化描述
            MStrFmtFormatDescript format_spec;
        };
        //! [MStrFmtArgClass: Array] 格式化信息
        struct
        {
            //! 参数位置
            u32_t array_arg_id;
            //! 参数类型
            MStrFmtArgType array_ele_type;
            //! 附带参数
            MStrFmtArgProperty array_ele_prop;
            //! 格式化描述
            MStrFmtFormatDescript array_ele_format_spec;
            //! split chars, 引用自fmt, [beg, end)是需要的split char
            const char* array_split_beg;

            //! split chars结束位置
            const char* array_split_end;
        };
    };
} MStrFmtParseResult;

/**
 * @brief 格式化字符串
 *
 * @param[in] fmt: 格式化串
 * @param[out] res_str: 格式化结果输出
 * @param[in] buff_sz: buff的最大允许大小
 * @param[in] fmt_place: 预期fmt中使用的参数数目.
 * 最大不超过16(MFMT_PLACE_MAX_NUM)
 *
 * @return minfmt_result_t: 格式化结果
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_format(const char* fmt, MString* res_str, usize_t fmt_place, ...);

/**
 * @brief 格式化字符串
 *
 * @param[in] fmt: 格式化串
 * @param[out] buff: 格式化结果输出
 * @param[in] buff_sz: buff的最大允许大小
 * @param[in] fmt_place: 预期fmt中使用的参数数目.
 * 最大不超过16(MFMT_PLACE_MAX_NUM)
 * @param[in] ap_ptr: &ap ( 这样子是为了避免verify时的问题 )
 *
 * @return minfmt_result_t: 格式化结果
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_vformat(
    const char* fmt, MString* res_str, usize_t fmt_place, isize_t ap_ptr
);

/**
 * @brief 初始化Parser状态
 *
 * @param[in] mem: 内存, 大小需要至少比 sizeof(MStrFmtParserState) 大
 * @param[in] inp: 输入字符串
 * @param[out] ppstate: 解析状态
 *
 * @note 使用macro MFMT_PARSER_STATE_SIZE 来帮助解决 mem 的大小问题
 */
MSTR_EXPORT_API(void)
mstr_fmt_parser_init(
    byte_t* mem, const char* inp, MStrFmtParserState** ppstate
);

/**
 * @brief 解析 replacement field
 *
 * @param[inout] state: Parser状态
 * @param[out] result: Parser结果
 *
 * @return mstr_result_t: parser结果
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_fmt_parse_goal(
    MStrFmtParserState* state, MStrFmtParseResult* result
);

/**
 * @brief 计算parser停止的位置
 *
 * @param state: parser状态
 * @param pbeg: parser init时候传进去的inp指针
 *
 */
MSTR_EXPORT_API(usize_t)
mstr_fmt_parser_end_position(
    MStrFmtParserState* state, const char* pbeg
);

/**
 * @brief 将无符号量化值转换为字符串
 *
 * @param[out] res_str: 转换结果
 * @param[in] value: 需要转换的值
 * @param[in] quat: 量化精度
 *
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_fmt_uqtoa(MString* res_str, u32_t value, u32_t quat);

/**
 * @brief 将无符号整数转换为字符串
 *
 * @param[out] res_str: 转换结果
 * @param[in] value: 需要转换的值
 * @param[in] index: Index
 *
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_fmt_utoa(MString* res_str, u32_t value, MStrFmtIntIndex index);

/**
 * @brief 将日期时间值转换为字符串
 *
 *
 * @param[out] res_str: 转换结果
 * @param[in] tm: 日期时间值
 * @param[in] spec: 格式化信息
 */
MSTR_EXPORT_API(mstr_result_t)
mstr_fmt_ttoa(
    MString* res_str,
    const sys_time_t* tm,
    const MStrFmtChronoFormatSpec* spec
);

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
/**
 * @brief 标记不可达的分支
 *
 */
#define system_unreachable() assert_param(0)
#else
#define system_unreachable() ((void)0U)
#endif

#endif // !_INCLUDE_MM_FMT_H_
