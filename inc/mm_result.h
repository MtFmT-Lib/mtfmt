// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_result.h
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   结果类型
 * @version 1.0
 * @date    2023-04-22
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#if !defined(_INCLUDE_MM_RESULT_H_)
#define _INCLUDE_MM_RESULT_H_ 1

/**
 * @brief 结果类型
 *
 */
typedef enum tagMStrResult
{
    MStr_Ok = 0,
    // ERR: 内存错误: 内存分配失败
    MStr_Err_HeapTooSmall = -128,
    // ERR: buff太短
    MStr_Err_BufferTooSmall,
    // ERR: 格式化: 索引太多
    MStr_Err_IndexTooLarge,
    // ERR: 格式化: Parser错误: 未识别的token
    MStr_Err_UnrecognizedToken,
    // ERR: 格式化: Parser错误: Missing `{`
    MStr_Err_MissingLeftBrace,
    // ERR: 格式化: Parser错误: Missing `}`
    MStr_Err_MissingRightBrace,
    // ERR: 格式化: Parser错误: Missing Formatter
    MStr_Err_MissingReplacement,
    // ERR: 格式化: Parser错误: Missing ArgID
    MStr_Err_MissingArgumentID,
    // ERR: 格式化: Parser错误: Missing ArgType
    MStr_Err_MissingArgumentType,
    // ERR: 格式化: Parser错误: 不支持的fill char
    MStr_Err_UnsupportFillChar,
    // ERR: 格式化: Parser错误: fill char后面必须要align
    MStr_Err_MissingAlignAfterFillChar,
    // ERR: 格式化: Parser错误: 不支持的格式化方式
    MStr_Err_UnsupportFormatType,
    // ERR: 格式化: Parser错误: 请求宽度太大
    MStr_Err_WidthTooLarge,
    // ERR: 格式化: Parser错误: 请求的格式化项太多(日期时间)
    MStr_Err_TooMoreChronoItem,
    // ERR: 格式化: Parser错误: 错失格式化的token(日期时间)
    MStr_Err_MissingChronoItemType,
    // ERR: 格式化: Parser错误: 未指定
    MStr_Err_UndefinedParserError,
    // ERR: 格式化: 还未使用过的参数ID
    MStr_Err_UnusedArgumentID,
    // ERR: 格式化: 参数太大
    MStr_Err_InvaildArgumentID,
    // ERR: 格式化: 类型不正确
    MStr_Err_InvaildArgumentType,
    // ERR: 格式化: 内建buffer不够大
    MStr_Err_InternalBufferTooSmall,
    // ERR: 格式化: 不支持的类型
    MStr_Err_UnsupportType,
    // ERR: 格式化: 不支持的量化精度
    MStr_Err_UnsupportQuantBits,
    // ERR: 最后一个的flag
    MStr_Err_Flag_LastOne,
} mstr_result_t;

// 判断操作是否成功
#define MSTR_SUCC(s)   ((s) == MStr_Ok)

// 判断操作是否失败
#define MSTR_FAILED(s) ((s) != MStr_Ok)

// AND_THEN
#define MSTR_AND_THEN(e, then) \
    ({                         \
        mstr_result_t r = (e); \
        (MSTR_SUCC(r)) ? ({    \
            r = (then);        \
            r;                 \
        }) :                   \
                         (r);  \
    })

#endif // _INCLUDE_MM_RESULT_H_
