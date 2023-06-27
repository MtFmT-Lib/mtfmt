// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_parser.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   formatter parser实现
 * @version 1.0
 * @date    2023-03-21
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */

#define MSTR_IMP_SOURCES 1

#include "mm_parser.h"

//! peek一个字符
#define LEX_PEEK_CHAR(pstr) (*(pstr))

//! move到下一个字符的位置
#define LEX_MOVE_TO_NEXT(pstr) \
    do {                       \
        (pstr) += 1;           \
    } while (0)

//! ACCEPT一个token, 注意这个macro不会跳转
#define LEX_ACCEPT_TOKEN(tk, typ, at_pstr)            \
    do {                                              \
        (tk)->type = (typ);                           \
        (tk)->len = (usize_t)((at_pstr) - (tk)->beg); \
    } while (0)

//! 判断是否为一个number
#define IS_NUMBER(ch)         ((ch) >= '0' && (ch) <= '9')

//! 取得当前Token
#define LEX_CURRENT_TOKEN(ps) ((ps)->token)

//! 表示一个stage开始
#define PARSER_STAGE_BEGIN(s, new_s) \
    ((ParserStage)((uint32_t)(s) | (uint32_t)(new_s)))

//! 表示一个stage结束
#define PARSER_STAGE_END(s, new_s) \
    ((ParserStage)((uint32_t)(s) & ~(uint32_t)(new_s)))

//! 判断是否为Chrono格式化需要的token
#define IS_CHRONO_TOKEN_TYPE(type)                              \
    ((uint32_t)(type) > (uint32_t)TokenType_Flag_ChronoBegin && \
     (uint32_t)(type) < (uint32_t)TokenType_Flag_ChronoEnd)

//! 判断是否为Chrono格式化需要的用户自定义token
#define IS_CHRONO_USERDEF_TOKEN_TYPE(type)                             \
    ((uint32_t)(type) > (uint32_t)TokenType_Flag_ChronoUserdefBegin && \
     (uint32_t)(type) < (uint32_t)TokenType_Flag_ChronoEnd)

/**
 * @brief Token的类型
 *
 */
typedef enum tagTokenType
{
    //! '\0'
    TokenType_EOF,

    //! '>'
    TokenType_Gt,

    //! '<'
    TokenType_Lt,

    //! '='
    TokenType_Equ,

    //! ':'
    TokenType_Colon,

    //! '|:'
    TokenType_VlineColon,

    //! ':i8'
    TokenType_Type_Int8,

    //! ':i16'
    TokenType_Type_Int16,

    //! ':i32'
    TokenType_Type_Int32,

    //! ':u8'
    TokenType_Type_Uint8,

    //! ':u16'
    TokenType_Type_Uint16,

    //! ':u32'
    TokenType_Type_Uint32,

    //! 'FXX.XXX'
    TokenType_Type_IFixedNumber,

    //! 'FXX.XXXu'
    TokenType_Type_UFixedNumber,

    //! 'qXX'
    TokenType_Type_IQuant,

    //! 'qXXu'
    TokenType_Type_UQuant,

    //! f16
    TokenType_Type_Floating16,

    //! f32
    TokenType_Type_Floating32,

    //! f64
    TokenType_Type_Floating64,

    //! ':s'
    TokenType_Type_CString,

    //! ':t'
    TokenType_Type_SysTime,

    //! '{'
    TokenType_LeftBrace,

    //! '}'
    TokenType_RightBrace,

    //! '{['
    TokenType_LeftBraceBracket,

    //! ']}'
    TokenType_RightBracketBrace,

    //! '{{'
    TokenType_LeftBraceBrace,

    //! '}}'
    TokenType_RightBraceBrace,

    //! [0-9] +
    TokenType_Digits,

    //! 开始: Chrono相关的token
    TokenType_Flag_ChronoBegin,

    //! Chrono: f
    TokenType_ChronoPredef_f,

    //! Chrono: g
    TokenType_ChronoPredef_g,

    //! 开始: Chrono相关的userdef token
    TokenType_Flag_ChronoUserdefBegin,

    //! Chrono: 年份, 1 ~ 2位
    TokenType_Chrono_Year_1,

    //! Chrono: 年份, 2位, 不足补0
    TokenType_Chrono_Year_Fixed2,

    //! Chrono: 年份, 3位或者4位, 不足补0
    TokenType_Chrono_Year_3,

    //! Chrono: 年份, 4位, 不足补0
    TokenType_Chrono_Year_Fixed4,

    //! Chrono: 月份, 1 ~ 2位
    TokenType_Chrono_Month_1,

    //! Chrono: 月份, 2位, 不足补0
    TokenType_Chrono_Month_Fixed2,

    //! Chrono: 天数, 1 ~ 2位
    TokenType_Chrono_Day_1,

    //! Chrono: 天数, 2位, 不足补0
    TokenType_Chrono_Day_Fixed2,

    //! Chrono: 小时, 1 ~ 2位
    TokenType_Chrono_Hour_1,

    //! Chrono: 小时, 2位, 不足补0
    TokenType_Chrono_Hour_Fixed2,

    //! Chrono: 小时(24 hour), 1 ~ 2位
    TokenType_Chrono_Hour24_1,

    //! Chrono: 小时(24 hour), 2位, 不足补0
    TokenType_Chrono_Hour24_Fixed2,

    //! Chrono: 分钟, 1 ~ 2位
    TokenType_Chrono_Minute_1,

    //! Chrono: 分钟, 2位, 不足补0
    TokenType_Chrono_Minute_Fixed2,

    //! Chrono: 秒, 1 ~ 2位
    TokenType_Chrono_Second_1,

    //! Chrono: 秒, 2位, 不足补0
    TokenType_Chrono_Second_Fixed2,

    //! Chrono: 子秒, 1位
    TokenType_Chrono_SubSecond_Fixed1,

    //! Chrono: 子秒, 2位
    TokenType_Chrono_SubSecond_Fixed2,

    //! Chrono: 子秒, 3位
    TokenType_Chrono_SubSecond_Fixed3,

    //! Chrono: 子秒, 4位
    TokenType_Chrono_SubSecond_Fixed4,

    //! Chrono: 星期 (full name)
    TokenType_Chrono_WeekName,

    //! 结束: Chrono相关的token
    TokenType_Flag_ChronoEnd,

    //! 其它没有被定义的字符
    TokenType_OtherChar,
} TokenType;

/**
 * @brief TOKEN结构
 *
 */
typedef struct tagToken
{
    //! token长度
    usize_t len;
    //! token类型
    TokenType type;
    //! token的起始位置
    const char* beg;
} Token;

/**
 * @brief 表示parser的各个stage
 *
 * @note 该enum指定了parser stage, 好影响lex识别内容的倾向 (
 * 屑死了 o(*￣▽￣*)o
 * )
 */
typedef enum tagParserStage
{
    /**
     * @brief 当前parser在normal状态
     *
     */
    ParserStage_Normal = 0x00,

    /**
     * @brief 表示parser期望一个End的结束符号( } 或者 ]} )
     *
     */
    ParserStage_NeedEnd = 0x01,

    /**
     * @brief 表示parser期望对chrono的格式化内容进行解析
     *
     */
    ParserStage_MatchChronoToken = 0x02
} ParserStage;

/**
 * @brief 分析器状态
 *
 */
struct tagMStrFmtParserState
{
    /**
     * @brief 当前Token
     *
     */
    Token token;

    /**
     * @brief 当前读取位置
     *
     */
    const char* current;

    /**
     * @brief parser进行的stage, 指示lex应该怎么做
     *
     */
    ParserStage stage;
};

//
// parser:
//

static mstr_result_t
    parse_replacement_field(MStrFmtParserState*, MStrFmtParseResult*);
static mstr_result_t
    parse_array_field(MStrFmtParserState*, MStrFmtParseResult*);
static mstr_result_t
    parse_opt_split_chars(MStrFmtParserState*, const char**, const char**);
static mstr_result_t
    parse_split_chars(MStrFmtParserState*, const char**, const char**);
static mstr_result_t
    parse_simple_field(MStrFmtParserState*, MStrFmtParseResult*);
static mstr_result_t
    parse_opt_formatfield_spec(MStrFmtParserState*, MStrFmtArgType, MStrFmtFormatDescript*);
static mstr_result_t
    parse_opt_format_chrono_spec(MStrFmtParserState*, MStrFmtFormatSpec*);
static mstr_result_t
    parse_format_spec(MStrFmtParserState*, MStrFmtFormatSpec*);
static mstr_result_t
    parse_chrono_spec(MStrFmtParserState*, MStrFmtFormatSpec*);
static mstr_result_t
    parse_chrono_spec_item(MStrFmtParserState*, MStrFmtChronoItemFormatSpec*);
static mstr_result_t parse_opt_width(MStrFmtParserState*, int32_t*);
static mstr_result_t
    parse_opt_sign(MStrFmtParserState*, MStrFmtSignDisplay*);
static mstr_result_t
    parse_opt_items(MStrFmtParserState*, char*, MStrFmtAlign*, MStrFmtFormatSpec*, MStrFmtSignDisplay*);
static mstr_result_t parse_align(MStrFmtParserState*, MStrFmtAlign*);
static mstr_result_t parse_arg_id(MStrFmtParserState*, uint32_t*);
static mstr_result_t
    parse_arg_type(MStrFmtParserState*, MStrFmtArgType*, MStrFmtArgProperty*);
static void
    parse_arg_get_fixed_props(const Token*, MStrFmtArgProperty*);
static mstr_result_t parse_one_token(
    MStrFmtParserState*, TokenType, mstr_result_t
);
static mstr_result_t parse_end_token(
    MStrFmtParserState*, TokenType, mstr_result_t
);

//
// lexer:
//

static mstr_result_t parser_next_token(MStrFmtParserState*);
static mstr_result_t parser_peek_token(Token*, MStrFmtParserState*);
static mstr_result_t lex_next_token(
    Token*, const char*, usize_t, ParserStage
);
static uint32_t lex_atou(const char*, usize_t);

/**
 * @brief 默认的array split
 *
 */
static const char* const MFMT_DEFAULT_SPLIT = ", ";
static const usize_t MFMT_DEFAULT_SPLIT_LENGTH = 2;

/**
 * @brief 默认的年月日之间的分割
 *
 */
static const char* const MFMT_DEFAULT_CHRONO_DATE_SPLIT = "-";
static const usize_t MFMT_DEFAULT_CHRONO_DATE_SPLIT_LENGTH = 1;

/**
 * @brief 默认的时分秒之间的分割
 *
 */
static const char* const MFMT_DEFAULT_CHRONO_TIME_SPLIT = ":";
static const usize_t MFMT_DEFAULT_CHRONO_TIME_SPLIT_LENGTH = 1;

/**
 * @brief 默认的时间日期项之间的分割
 *
 */
static const char* const MFMT_DEFAULT_CHRONO_ITEM_SPLIT = " ";
static const usize_t MFMT_DEFAULT_CHRONO_ITEM_SPLIT_LENGTH = 1;

/**
 * @brief 默认的毫秒项之间的分割
 *
 */
static const char* const MFMT_DEFAULT_CHRONO_SUBSEC_SPLIT = ".";
static const usize_t MFMT_DEFAULT_CHRONO_SUBSEC_SPLIT_LENGTH = 1;

//
// public functions:
//

MSTR_EXPORT_API(void)
mstr_fmt_parser_init(
    byte_t* mem, const char* inp, MStrFmtParserState** ppstate
)
{
    if (MFMT_PARSER_STATE_SIZE < sizeof(MStrFmtParserState)) {
        mstr_unreachable();
    }
    else {
        MStrFmtParserState* pstate = (MStrFmtParserState*)mem;
        pstate->current = inp;
        pstate->stage = ParserStage_Normal;
        // ret
        *ppstate = pstate;
    }
}

MSTR_EXPORT_API(mstr_result_t)
mstr_fmt_parse_goal(
    MStrFmtParserState* state, MStrFmtParseResult* result
)
{
    mstr_result_t ret = MStr_Ok;
    // 第一个token
    ret = parser_next_token(state);
    if (MSTR_FAILED(ret)) {
        return ret;
    }
    // else:
    if (LEX_CURRENT_TOKEN(state).type == TokenType_LeftBraceBrace) {
        result->arg_class = MStrFmtArgClass_EscapeChar;
        result->val.escape_char = '{';
        // ret: MStr_Ok
    }
    else if (LEX_CURRENT_TOKEN(state).type == TokenType_RightBraceBrace) {
        result->arg_class = MStrFmtArgClass_EscapeChar;
        result->val.escape_char = '}';
        // ret: MStr_Ok
    }
    else {
        // 尝试解析格式化串
        ret = parse_replacement_field(state, result);
    }
    return ret;
}

MSTR_EXPORT_API(usize_t)
mstr_fmt_parser_end_position(
    MStrFmtParserState* state, const char* pbeg
)
{
    return (usize_t)(state->current - pbeg);
}

/**
 * @brief 解析 `replacement_field`
 *
 */
static mstr_result_t parse_replacement_field(
    MStrFmtParserState* state, MStrFmtParseResult* parser_result
)
{
    if (LEX_CURRENT_TOKEN(state).type == TokenType_LeftBrace) {
        mstr_result_t result = MStr_Ok;
        parser_result->arg_class = MStrFmtArgClass_Value;
        // '{'
        MSTR_AND_THEN(
            result,
            parse_one_token(
                state, TokenType_LeftBrace, MStr_Err_MissingReplacement
            )
        );
        // simple_field
        MSTR_AND_THEN(result, parse_simple_field(state, parser_result));
        // 闭合: '}'
        MSTR_AND_THEN(
            result,
            parse_end_token(
                state, TokenType_RightBrace, MStr_Err_MissingRightBrace
            )
        );
        // ret
        return result;
    }
    else if (LEX_CURRENT_TOKEN(state).type == TokenType_LeftBraceBracket) {
        mstr_result_t result = MStr_Ok;
        parser_result->arg_class = MStrFmtArgClass_Array;
        // '{['
        MSTR_AND_THEN(
            result,
            parse_one_token(
                state,
                TokenType_LeftBraceBracket,
                MStr_Err_MissingReplacement
            )
        );
        // array_field
        MSTR_AND_THEN(result, parse_array_field(state, parser_result));
        // 闭合: ']}'
        MSTR_AND_THEN(
            result,
            parse_end_token(
                state,
                TokenType_RightBracketBrace,
                MStr_Err_MissingRightBrace
            )
        );
        // ret
        return result;
    }
    else {
        // ERR
        return MStr_Err_MissingReplacement;
    }
}

/**
 * @brief 解析 数组变量的格式化标记
 *
 */
static mstr_result_t parse_array_field(
    MStrFmtParserState* state, MStrFmtParseResult* parser_result
)
{
    mstr_result_t result = MStr_Ok;
    uint32_t arg_id = 0;
    MStrFmtArgType arg_type = MStrFmtArgType_Unknown;
    MStrFmtArgProperty arg_prop = {0, 0};
    MStrFmtFormatDescript format_spec = {0};
    const char* split_beg = NULL;
    const char* split_end = NULL;
    // Arg ID
    MSTR_AND_THEN(result, parse_arg_id(state, &arg_id));
    // `:` arg_type
    // 为了方便lex这两个作为1个token被match
    // 需要格式化的值类型
    MSTR_AND_THEN(result, parse_arg_type(state, &arg_type, &arg_prop));
    // 可选的 `|:` split_ch
    MSTR_AND_THEN(
        result, parse_opt_split_chars(state, &split_beg, &split_end)
    );
    // 可选的format_spec
    MSTR_AND_THEN(
        result,
        parse_opt_formatfield_spec(state, arg_type, &format_spec)
    );
    // 赋值给结果
    if (MSTR_SUCC(result)) {
        parser_result->val.arr.id = arg_id;
        parser_result->val.arr.ele_typ = arg_type;
        parser_result->val.arr.ele_prop = arg_prop;
        parser_result->val.arr.split_beg = split_beg;
        parser_result->val.arr.split_end = split_end;
        parser_result->val.arr.spec = format_spec;
    }
    // 返回
    return result;
}

/**
 * @brief 解析可选的split char
 *
 */
static mstr_result_t parse_opt_split_chars(
    MStrFmtParserState* state, const char** ppbeg, const char** ppend
)
{
    mstr_result_t result = MStr_Ok;
    if (LEX_CURRENT_TOKEN(state).type == TokenType_VlineColon) {
        // `|:`
        // 该分支不会返回 `MStr_Err_UndefinedParserError`,
        // 因为已经做了判断了
        MSTR_AND_THEN(
            result,
            parse_one_token(
                state,
                TokenType_VlineColon,
                MStr_Err_UndefinedParserError
            )
        );
        // split chars
        MSTR_AND_THEN(result, parse_split_chars(state, ppbeg, ppend));
    }
    else {
        // 给定默认值
        *ppbeg = MFMT_DEFAULT_SPLIT;
        *ppend = MFMT_DEFAULT_SPLIT + MFMT_DEFAULT_SPLIT_LENGTH;
    }
    return result;
}

/**
 * @brief 解析split chars
 *
 */
static mstr_result_t parse_split_chars(
    MStrFmtParserState* state, const char** ppbeg, const char** ppend
)
{
    mstr_result_t result = MStr_Ok;
    const Token* cur_token = &LEX_CURRENT_TOKEN(state);
    // 起始位置为beg
    *ppbeg = cur_token->beg;
    while (MSTR_SUCC(result) && cur_token->type != TokenType_Colon &&
           cur_token->type != TokenType_RightBrace &&
           cur_token->type != TokenType_RightBracketBrace) {
        result = parser_next_token(state);
        cur_token = &LEX_CURRENT_TOKEN(state);
    }
    // 如果最后一次peek是fmt_ok, 那么end在此的前一个字符,
    // 开区间, 因此直接assign beg.
    if (MSTR_SUCC(result)) {
        *ppend = cur_token->beg;
        return MStr_Ok;
    }
    else {
        // 返回
        return result;
    }
}

/**
 * @brief 解析 单个变量的格式化标记
 *
 */
static mstr_result_t parse_simple_field(
    MStrFmtParserState* state, MStrFmtParseResult* parser_result
)
{
    uint32_t arg_id = 0;
    MStrFmtArgType arg_type = MStrFmtArgType_Unknown;
    MStrFmtArgProperty arg_prop = {0, 0};
    MStrFmtFormatDescript format_spec = {0};
    mstr_result_t result = MStr_Ok;
    // 默认为未指定
    format_spec.fmt_spec.fmt_type = MStrFmtFormatType_UnSpec;
    state->stage =
        PARSER_STAGE_BEGIN(state->stage, ParserStage_NeedEnd);
    // Arg ID
    MSTR_AND_THEN(result, parse_arg_id(state, &arg_id));
    // `:` arg_type
    // 为了方便lex这两个作为1个token被match
    // 需要格式化的值类型
    MSTR_AND_THEN(result, parse_arg_type(state, &arg_type, &arg_prop));
    // 可选的 `:` format_spec
    MSTR_AND_THEN(
        result,
        parse_opt_formatfield_spec(state, arg_type, &format_spec)
    );
    // 赋值给结果
    if (MSTR_SUCC(result)) {
        parser_result->val.val.id = arg_id;
        parser_result->val.val.typ = arg_type;
        parser_result->val.val.prop = arg_prop;
        parser_result->val.val.spec = format_spec;
    }
    // 返回
    state->stage = PARSER_STAGE_END(state->stage, ParserStage_NeedEnd);
    return result;
}

/**
 * @brief 解析可选的格式化描述: `:` [format_spec | chrono_spec]
 *
 */
static mstr_result_t parse_opt_formatfield_spec(
    MStrFmtParserState* state,
    MStrFmtArgType arg_type,
    MStrFmtFormatDescript* spec
)
{
    int32_t width = -1;
    char fill_char = ' ';
    MStrFmtAlign align = MStrFmtAlign_Right;
    MStrFmtSignDisplay sign_display = MStrFmtSignDisplay_NegOnly;
    MStrFmtFormatSpec fmt_spec = {0};
    mstr_result_t result = MStr_Ok;
    // arg_type可能会在未来用到, 不移除
    (void)arg_type;
    // chrono parse stage
    state->stage =
        PARSER_STAGE_BEGIN(state->stage, ParserStage_MatchChronoToken);
    if (LEX_CURRENT_TOKEN(state).type == TokenType_Colon) {
        // `:`
        // 该分支不会返回 `MStr_Err_UndefinedParserError`,
        // 因为已经做了判断了
        MSTR_AND_THEN(
            result,
            parse_one_token(
                state, TokenType_Colon, MStr_Err_UndefinedParserError
            )
        );
        // 可选的: fill char, align
        // fmt type,
        // sign_display在这里也进行了解析是因为这个文法在这里需要判断一下first
        // set, 然后对应着2个不同的rule, 做了点ad-hoc的事情
        MSTR_AND_THEN(
            result,
            parse_opt_items(
                state, &fill_char, &align, &fmt_spec, &sign_display
            )
        );
        if (MSTR_SUCC(result) &&
            fmt_spec.fmt_type == MStrFmtFormatType_UnSpec) {
            // 可选的sign
            MSTR_AND_THEN(result, parse_opt_sign(state, &sign_display));
            // 可选的width
            MSTR_AND_THEN(result, parse_opt_width(state, &width));
            // 可选的进行格式化的方式
            MSTR_AND_THEN(
                result, parse_opt_format_chrono_spec(state, &fmt_spec)
            );
        }
    }
    // if false: 默认值, if true: 或者true后进行解析完
    // 赋值给结果
    if (MSTR_SUCC(result)) {
        spec->fmt_align = align;
        spec->fmt_spec = fmt_spec;
        spec->fill_char = fill_char;
        spec->sign_display = sign_display;
        spec->width = width;
    }
    state->stage =
        PARSER_STAGE_END(state->stage, ParserStage_MatchChronoToken);
    return result;
}

/**
 * @brief 解析格式化的方式 `[format_spec | chrono_spec]`
 *
 * @note 这个东东被设计为上下文相关的, 会匹配有关的字符,
 * 尽管它们不在lex里面
 *
 * @attention 该函数不设置默认值
 */
static mstr_result_t parse_opt_format_chrono_spec(
    MStrFmtParserState* state, MStrFmtFormatSpec* spec
)
{
    const Token* cur_token = &LEX_CURRENT_TOKEN(state);
    TokenType type = cur_token->type;
    if (type == TokenType_OtherChar) {
        // 一般的值, 非日期和时间
        return parse_format_spec(state, spec);
    }
    else if (IS_CHRONO_TOKEN_TYPE(type)) {
        // 时间和日期的格式化描述
        return parse_chrono_spec(state, spec);
    }
    else {
        return MStr_Ok;
    }
}

/**
 * @brief 解析一般的值格式化的方式(format_spec)
 *
 */
static mstr_result_t parse_format_spec(
    MStrFmtParserState* state, MStrFmtFormatSpec* spec
)
{
    const Token* cur_token = &LEX_CURRENT_TOKEN(state);
    // assert: cur_token->type == TokenType_OtherChar
    mstr_bool_t be_matched;
    char ch = *cur_token->beg;
    switch (ch) {
    case 'b':
        be_matched = True;
        spec->fmt_type = MStrFmtFormatType_Binary;
        break;
    case 'd':
        be_matched = True;
        spec->fmt_type = MStrFmtFormatType_Deciaml;
        break;
    case 'o':
        be_matched = True;
        spec->fmt_type = MStrFmtFormatType_Oct;
        break;
    case 'h':
        be_matched = True;
        spec->fmt_type = MStrFmtFormatType_Hex;
        break;
    case 'H':
        be_matched = True;
        spec->fmt_type = MStrFmtFormatType_Hex_UpperCase;
        break;
    case 'x':
        be_matched = True;
        spec->fmt_type = MStrFmtFormatType_Hex_WithPrefix;
        break;
    case 'X':
        be_matched = True;
        spec->fmt_type = MStrFmtFormatType_Hex_UpperCase_WithPrefix;
        break;
    default: be_matched = False; break;
    }
    if (be_matched) {
        // 类型正确
        return parser_next_token(state);
    }
    else {
        // 不支持的formatter type
        // 因为 `}` 不会被识别为 TokenType_OtherChar,
        // 因此这里为err
        return MStr_Err_UnsupportFormatType;
    }
}

/**
 * @brief 解析时间日期的值格式化的方式(chrono_spec)
 *
 */
static mstr_result_t parse_chrono_spec(
    MStrFmtParserState* state, MStrFmtFormatSpec* spec
)
{
    usize_t index = 0;
    mstr_result_t result = MStr_Ok;
    MStrFmtChronoFormatSpec* chrono_spec = &spec->spec.chrono;
    // 默认值的lut
    // 它依赖于 MStrFmtChronoValueType 的顺序
    const uint8_t packed_lut[][4] = {
        // MStrFmtChronoValueType_Year:
        {(uint8_t)MStrFmtChronoValueType_Year,
         (uint8_t)True,
         4,
         (uint8_t)MFMT_DEFAULT_CHRONO_DATE_SPLIT_LENGTH},
        // MStrFmtChronoValueType_Month:
        {(uint8_t)MStrFmtChronoValueType_Month,
         (uint8_t)True,
         2,
         (uint8_t)MFMT_DEFAULT_CHRONO_DATE_SPLIT_LENGTH},
        // MStrFmtChronoValueType_Day:
        {(uint8_t)MStrFmtChronoValueType_Day,
         (uint8_t)True,
         2,
         (uint8_t)MFMT_DEFAULT_CHRONO_ITEM_SPLIT_LENGTH},
        // MStrFmtChronoValueType_Hour24
        {(uint8_t)MStrFmtChronoValueType_Hour24,
         (uint8_t)True,
         2,
         (uint8_t)MFMT_DEFAULT_CHRONO_TIME_SPLIT_LENGTH},
        // MStrFmtChronoValueType_Minute
        {(uint8_t)MStrFmtChronoValueType_Minute,
         (uint8_t)True,
         2,
         (uint8_t)MFMT_DEFAULT_CHRONO_TIME_SPLIT_LENGTH},
        // MStrFmtChronoValueType_Second
        {(uint8_t)MStrFmtChronoValueType_Second,
         (uint8_t)True,
         2,
         (uint8_t)MFMT_DEFAULT_CHRONO_SUBSEC_SPLIT_LENGTH},

    };
    const iptr_t packed_lut_ptr[] = {
        // MStrFmtChronoValueType_Year:
        (iptr_t)MFMT_DEFAULT_CHRONO_DATE_SPLIT,
        // MStrFmtChronoValueType_Month:
        (iptr_t)MFMT_DEFAULT_CHRONO_DATE_SPLIT,
        // MStrFmtChronoValueType_Day:
        (iptr_t)MFMT_DEFAULT_CHRONO_ITEM_SPLIT,
        // MStrFmtChronoValueType_Hour24:
        (iptr_t)MFMT_DEFAULT_CHRONO_TIME_SPLIT,
        // MStrFmtChronoValueType_Minute:
        (iptr_t)MFMT_DEFAULT_CHRONO_TIME_SPLIT,
        // MStrFmtChronoValueType_Second:
        (iptr_t)MFMT_DEFAULT_CHRONO_SUBSEC_SPLIT,
    };
    const usize_t lut_size =
        sizeof(packed_lut_ptr) / sizeof(packed_lut_ptr[0]);
    switch (LEX_CURRENT_TOKEN(state).type) {
    case TokenType_ChronoPredef_g:
    case TokenType_ChronoPredef_f:
        for (usize_t i = 0; i < lut_size; i += 1) {
            const uint8_t* values = packed_lut[i];
            const char* split_beg = (const char*)packed_lut_ptr[i];
            MStrFmtChronoItemFormatSpec format_spec = {
                .value_type = (MStrFmtChronoValueType)values[0],
                .chrono_spec =
                    {(mstr_bool_t)values[1], (uint8_t)values[2]},
                .split_beg = split_beg,
                .split_end = split_beg + (usize_t)values[3],
            };
            chrono_spec->items[i] = format_spec;
        }
        if (LEX_CURRENT_TOKEN(state).type == TokenType_ChronoPredef_f) {
            // 携带星期
            MStrFmtChronoItemFormatSpec sub_sec = {
                .value_type = MStrFmtChronoValueType_SubSecond,
                .chrono_spec = {True, 4},
                .split_beg = MFMT_DEFAULT_CHRONO_ITEM_SPLIT,
                .split_end = MFMT_DEFAULT_CHRONO_ITEM_SPLIT +
                             MFMT_DEFAULT_CHRONO_ITEM_SPLIT_LENGTH,
            };
            MStrFmtChronoItemFormatSpec week = {
                .value_type = MStrFmtChronoValueType_Week,
                .chrono_spec = {False, 0},
                .split_beg = NULL,
                .split_end = NULL,
            };
            chrono_spec->items[6] = sub_sec;
            chrono_spec->items[7] = week;
            chrono_spec->item_cnt = 8;
        }
        else {
            // 正常长度
            MStrFmtChronoItemFormatSpec r = {
                .value_type = MStrFmtChronoValueType_SubSecond,
                .chrono_spec = {True, 4},
                .split_beg = NULL,
                .split_end = NULL,
            };
            chrono_spec->items[6] = r;
            chrono_spec->item_cnt = 7;
        }
        MSTR_AND_THEN(result, parser_next_token(state));
        break;
    default:
        // 其余情况当成用户自定义的格式化内容
        while (MSTR_SUCC(result)) {
            if (index >= MFMT_CHRONO_PLACE_MAX_NUM) {
                result = MStr_Err_TooMoreChronoItem;
                continue; // break
            }
            // 解析项
            result = parse_chrono_spec_item(
                state, &chrono_spec->items[index]
            );
            // index ++
            index += 1;
            // 确认还可以继续
            if (MSTR_SUCC(result)) {
                const Token* cur_token = &LEX_CURRENT_TOKEN(state);
                if (!IS_CHRONO_TOKEN_TYPE(cur_token->type)) {
                    break;
                }
            }
        }
        chrono_spec->item_cnt = index;
        break;
    }
    return result;
}

/**
 * @brief 解析时间日期的值格式化的方式项
 *
 */
static mstr_result_t parse_chrono_spec_item(
    MStrFmtParserState* state, MStrFmtChronoItemFormatSpec* item
)
{
    mstr_result_t result = MStr_Ok;
    const Token* cur_token = &LEX_CURRENT_TOKEN(state);
    // lut, 注意依赖于token顺序
    static const uint8_t packed_lut[][3] = {
        // TokenType_Chrono_Year_1:
        // vaue_type, fixed_length, format_length  下同
        {(uint8_t)MStrFmtChronoValueType_Year, (uint8_t)False, 1},
        // TokenType_Chrono_Year_Fixed2:
        {(uint8_t)MStrFmtChronoValueType_Year, (uint8_t)True, 2},
        // TokenType_Chrono_Year_3:
        {(uint8_t)MStrFmtChronoValueType_Year, (uint8_t)False, 3},
        // TokenType_Chrono_Year_Fixed4:
        {(uint8_t)MStrFmtChronoValueType_Year, (uint8_t)True, 4},
        // TokenType_Chrono_Month_1:
        {(uint8_t)MStrFmtChronoValueType_Month, (uint8_t)False, 1},
        // TokenType_Chrono_Month_Fixed2:
        {(uint8_t)MStrFmtChronoValueType_Month, (uint8_t)True, 2},
        // TokenType_Chrono_Day_1:
        {(uint8_t)MStrFmtChronoValueType_Day, (uint8_t)False, 1},
        // TokenType_Chrono_Day_Fixed2:
        {(uint8_t)MStrFmtChronoValueType_Day, (uint8_t)True, 2},
        // TokenType_Chrono_Hour_1:
        {(uint8_t)MStrFmtChronoValueType_Hour, (uint8_t)False, 1},
        // TokenType_Chrono_Hour_Fixed2:
        {(uint8_t)MStrFmtChronoValueType_Hour, (uint8_t)True, 2},
        // TokenType_Chrono_Hour24_1:
        {(uint8_t)MStrFmtChronoValueType_Hour24, (uint8_t)False, 1},
        // TokenType_Chrono_Hour24_Fixed2:
        {(uint8_t)MStrFmtChronoValueType_Hour24, (uint8_t)True, 2},
        // TokenType_Chrono_Minute_1:
        {(uint8_t)MStrFmtChronoValueType_Minute, (uint8_t)False, 1},
        // TokenType_Chrono_Minute_Fixed2:
        {(uint8_t)MStrFmtChronoValueType_Minute, (uint8_t)True, 2},
        // TokenType_Chrono_Second_1:
        {(uint8_t)MStrFmtChronoValueType_Second, (uint8_t)False, 1},
        // TokenType_Chrono_Second_Fixed2:
        {(uint8_t)MStrFmtChronoValueType_Second, (uint8_t)True, 2},
        // TokenType_Chrono_SubSecond_Fixed1:
        {(uint8_t)MStrFmtChronoValueType_SubSecond, (uint8_t)True, 1},
        // TokenType_Chrono_SubSecond_Fixed2:
        {(uint8_t)MStrFmtChronoValueType_SubSecond, (uint8_t)True, 2},
        // TokenType_Chrono_SubSecond_Fixed3:
        {(uint8_t)MStrFmtChronoValueType_SubSecond, (uint8_t)True, 3},
        // TokenType_Chrono_SubSecond_Fixed4:
        {(uint8_t)MStrFmtChronoValueType_SubSecond, (uint8_t)True, 4},
        // TokenType_Chrono_WeekName
        {(uint8_t)MStrFmtChronoValueType_Week, (uint8_t)False, 0},
    };
    if (IS_CHRONO_USERDEF_TOKEN_TYPE(cur_token->type)) {
        uint32_t off = (uint32_t)TokenType_Chrono_Year_1;
        uint32_t idx = (uint32_t)cur_token->type - off;
        const uint8_t* values = packed_lut[idx];
        item->value_type = (MStrFmtChronoValueType)values[0];
        item->chrono_spec.fixed_length = (mstr_bool_t)values[1];
        item->chrono_spec.format_length = (uint8_t)values[2];
    }
    else {
        result = MStr_Err_MissingChronoItemType;
    }
    // 取得下一个token
    MSTR_AND_THEN(result, parser_next_token(state));
    // 解析可选的分隔符
    cur_token = &LEX_CURRENT_TOKEN(state);
    // 指向一块空的区域
    item->split_beg = cur_token->beg;
    item->split_end = item->split_beg;
    // 识别下一个字符
    while (MSTR_SUCC(result) &&
           !IS_CHRONO_TOKEN_TYPE(cur_token->type) &&
           cur_token->type != TokenType_RightBracketBrace &&
           cur_token->type != TokenType_RightBrace &&
           cur_token->type != TokenType_EOF) {
        // 取得下一个token
        // 只要不是}, ]}等结束符号均认为是分割字符
        MSTR_AND_THEN(result, parser_next_token(state));
        item->split_end = cur_token->beg;
    }
    return result;
}

/**
 * @brief 解析可选的宽度, 默认为-1
 *
 * @attention 该函数不设置默认值
 */
static mstr_result_t parse_opt_width(
    MStrFmtParserState* state, int32_t* width
)
{
    const Token* cur_token = &LEX_CURRENT_TOKEN(state);
    if (cur_token->type == TokenType_Digits) {
        uint32_t w = lex_atou(cur_token->beg, cur_token->len);
        if (w >= MFMT_PLACE_MAX_WIDTH) {
            *width = 0;
            return MStr_Err_WidthTooLarge;
        }
        else {
            *width = (int32_t)w;
            // 类型正确, 取得下一个token, 然后ret
            return parser_next_token(state);
        }
    }
    else {
        return MStr_Ok;
    }
}

/**
 * @brief 指定符号的显示方式
 *
 * @note 这个东东被设计为上下文相关的, 会匹配'+', '-', ' ',
 * 尽管它们不在lex里面
 *
 * @attention 该函数不设置默认值
 */
static mstr_result_t parse_opt_sign(
    MStrFmtParserState* state, MStrFmtSignDisplay* disp
)
{
    const Token* cur_token = &LEX_CURRENT_TOKEN(state);
    if (cur_token->type == TokenType_OtherChar) {
        int matched;
        char ch = *cur_token->beg;
        switch (ch) {
        case '+': matched = MStrFmtSignDisplay_Always; break;
        case '-': matched = MStrFmtSignDisplay_NegOnly; break;
        case ' ': matched = MStrFmtSignDisplay_Neg_Or_Space; break;
        default: matched = -1; break;
        }
        if (matched != -1) {
            *disp = (MStrFmtSignDisplay)matched;
            return parser_next_token(state);
        }
        else {
            return MStr_Ok;
        }
    }
    else {
        // 默认仅在存在小于0的情况时显示
        return MStr_Ok;
    }
}

/**
 * @brief 解析可选的fill char或者format type
 *
 * @note 因为fill char的后面其实除了[0-9], ], }, |都可以,
 * 因此它识别了一大堆的token
 *
 * @attention 该函数不设置默认值
 */
static mstr_result_t parse_opt_items(
    MStrFmtParserState* state,
    char* ch,
    MStrFmtAlign* align,
    MStrFmtFormatSpec* spec,
    MStrFmtSignDisplay* sign
)
{
    const Token* cur_token = &LEX_CURRENT_TOKEN(state);
    switch (cur_token->type) {
    case TokenType_Lt:
    case TokenType_Gt:
    case TokenType_Equ:
        // fill char为默认, 取得align
        return parse_align(state, align);
        // break
    case TokenType_Colon:
    case TokenType_Digits:
    case TokenType_LeftBrace:
    case TokenType_OtherChar:
    {
        // 这里存在着非LL(1)的情况: 必须知道next
        // token才能得到当前到底是fill char还是format_type
        Token next_token;
        mstr_result_t res;
        res = parser_peek_token(&next_token, state);
        if (MSTR_SUCC(res)) {
            if (next_token.type == TokenType_Lt ||
                next_token.type == TokenType_Gt ||
                next_token.type == TokenType_Equ) {
                // 此时只能是align了
                // 把第一个字符丢回去
                *ch = *cur_token->beg;
                // parse对齐描述
                res = parser_next_token(state);
                MSTR_AND_THEN(res, parse_align(state, align));
                return res;
            }
            else {
                // 后面应该是format_type或者sign
                if ((cur_token->type == TokenType_OtherChar &&
                     *cur_token->beg == ' ') ||
                    (cur_token->type == TokenType_OtherChar &&
                     *cur_token->beg == '+') ||
                    (cur_token->type == TokenType_OtherChar &&
                     *cur_token->beg == '-')) {
                    // 优先匹配sign
                    return parse_opt_sign(state, sign);
                }
                else {
                    // format_type
                    return parse_opt_format_chrono_spec(state, spec);
                }
            }
        }
        else {
            // peek寄啦, next也会寄, 所以返回err
            return res;
        }
    }
    // break
    default:
        // 默认情况, 不处理
        *ch = ' ';
        *align = MStrFmtAlign_Left;
        return MStr_Ok;
        // break
    }
}

/**
 * @brief 解析align
 *
 * @attention 该函数不指定默认值
 */
static mstr_result_t parse_align(
    MStrFmtParserState* state, MStrFmtAlign* align
)
{
    mstr_result_t res = MStr_Ok;
    MStrFmtAlign matched;
    const Token* cur_token = &LEX_CURRENT_TOKEN(state);
    switch (cur_token->type) {
    case TokenType_Lt: matched = MStrFmtAlign_Left; break;
    case TokenType_Gt: matched = MStrFmtAlign_Right; break;
    case TokenType_Equ: matched = MStrFmtAlign_Center; break;
    default:
        matched = MStrFmtAlign_Left;
        // ERROR
        res = MStr_Err_MissingAlignAfterFillChar;
        break;
    }
    *align = matched;
    MSTR_AND_THEN(res, parser_next_token(state));
    return res;
}

/**
 * @brief 解析格式化的值类型
 *
 */
static mstr_result_t parse_arg_type(
    MStrFmtParserState* state,
    MStrFmtArgType* arg_type,
    MStrFmtArgProperty* arg_prop
)
{
    mstr_bool_t valid_type = True;
    MStrFmtArgType typ;
    const Token* cur_token = &LEX_CURRENT_TOKEN(state);
    switch (cur_token->type) {
    case TokenType_Type_Int8: typ = MStrFmtArgType_Int8; break;
    case TokenType_Type_Int16: typ = MStrFmtArgType_Int16; break;
    case TokenType_Type_Int32: typ = MStrFmtArgType_Int32; break;
    case TokenType_Type_Uint8: typ = MStrFmtArgType_Uint8; break;
    case TokenType_Type_Uint16: typ = MStrFmtArgType_Uint16; break;
    case TokenType_Type_Uint32: typ = MStrFmtArgType_Uint32; break;
    case TokenType_Type_CString: typ = MStrFmtArgType_CString; break;
    case TokenType_Type_SysTime: typ = MStrFmtArgType_Time; break;
    case TokenType_Type_IQuant:
        typ = MStrFmtArgType_QuantizedValue;
        parse_arg_get_fixed_props(cur_token, arg_prop);
        break;
    case TokenType_Type_UQuant:
        typ = MStrFmtArgType_QuantizedUnsignedValue;
        parse_arg_get_fixed_props(cur_token, arg_prop);
        break;
    default:
        valid_type = False;
        typ = MStrFmtArgType_Unknown;
        break;
    }
    if (valid_type) {
        *arg_type = typ;
        // 类型正确, 取得下一个token, 然后ret
        return parser_next_token(state);
    }
    else {
        return MStr_Err_MissingArgumentType;
    }
}

/**
 * @brief 解析定点数token里面的信息, 填充 MStrFmtArgProperty
 *
 * @note 因为Token正确时其绝对是succ, 因此返回类型为void
 */
static void parse_arg_get_fixed_props(
    const Token* token, MStrFmtArgProperty* arg_prop
)
{
    // assert: token->type == TokenType_Type_IQuant
    //          ||  token->type == TokenType_Type_UQuant
    // 跳过 `:` `q`
    const char* beg = token->beg + 2;
    uint32_t v1 = (uint32_t)(beg[0] - '0');
    uint32_t v2 = (uint32_t)(beg[1] - '0');
    // 计算值
    arg_prop->a = v1 * 10 + v2;
    arg_prop->b = 0;
}

/**
 * @brief 解析arg_id
 *
 */
static mstr_result_t parse_arg_id(
    MStrFmtParserState* state, uint32_t* arg_id
)
{
    const Token* cur_token = &LEX_CURRENT_TOKEN(state);
    if (cur_token->type != TokenType_Digits) {
        *arg_id = 0;
        return MStr_Err_MissingArgumentID;
    }
    else {
        uint32_t id = lex_atou(cur_token->beg, cur_token->len);
        *arg_id = id;
        // 类型正确, 取得下一个token, 然后ret
        return parser_next_token(state);
    }
}

/**
 * @brief 表示当前需要token类型 tk, 不然返回 or_else
 *
 * @attention 这个东东没有正确的结束字符处理,
 * 不要在Goal的末尾用这个
 */
static mstr_result_t parse_one_token(
    MStrFmtParserState* state, TokenType tk, mstr_result_t or_else
)
{
    if (LEX_CURRENT_TOKEN(state).type != tk) {
        return or_else;
    }
    else {
        // 类型正确, 取得下一个token, 然后ret
        return parser_next_token(state);
    }
}

/**
 * @brief 表示当前需要token类型 tk, 不然返回 or_else
 *
 * @attention 这个东东有正确的结束字符处理, 在Goal的末尾用这个,
 * 这样能给出正确的offset
 */
static mstr_result_t parse_end_token(
    MStrFmtParserState* state, TokenType tk, mstr_result_t or_else
)
{
    if (LEX_CURRENT_TOKEN(state).type != tk) {
        return or_else;
    }
    else if (tk != TokenType_RightBrace && tk != TokenType_RightBracketBrace && tk != TokenType_EOF) {
        // 类型正确, 取得下一个token, 然后ret
        return parser_next_token(state);
    }
    else {
        // `}` 标记着formatter结束, 不做任何操作
        return MStr_Ok;
    }
}

/**
 * @brief 取得下一个token
 *
 * @param[inout] state: 分析器状态
 *
 */
static mstr_result_t parser_next_token(MStrFmtParserState* state)
{
    mstr_result_t result = MStr_Ok;
    result =
        lex_next_token(&state->token, state->current, 0, state->stage);
    if (MSTR_SUCC(result)) {
        // 更新state
        state->current += state->token.len;
    }
    return result;
}

/**
 * @brief 尝试看看下一个token
 *
 * @param[inout] state: 分析器状态
 *
 */
static mstr_result_t parser_peek_token(
    Token* token, MStrFmtParserState* state
)
{
    return lex_next_token(token, state->current, 0, state->stage);
}

/**
 * @brief 取得下一个token
 *
 * @param[out] token: Token输出
 * @param[in] str: 输入字符串, 会取得最前面的token
 * @param[in] offset: 输入str的偏移位置
 * @param[in] stage: 这个值可以告诉lex,
 * parser希望尝试识别什么样的token
 *
 * @return mstr_result_t: 解析结果
 */
static mstr_result_t lex_next_token(
    Token* token, const char* str, usize_t offset, ParserStage stage
)
{
    char ch = '\0';
    const char* pstr = str + offset;
    TokenType matched_type;
    // beg都是一样的
    token->beg = pstr;
    //
    // 咱在这里
    // 让token.type记录上一次accept的token类型
    // 而state硬编码为label
    // 这样子就方便回溯上一个accept state啦
    //
    goto s0;
s0:
    ch = LEX_PEEK_CHAR(pstr);
    switch (ch) {
    case '=': goto equ;
    case '>': goto gt;
    case '<': goto lt;
    case ':': goto colon;
    case '{': goto l_brace;
    case '}': goto r_brace;
    case ']': goto r_bracket;
    case '|': goto v_line;
    case '\0': goto eof;
    case '%':
        if ((stage & ParserStage_MatchChronoToken) ==
            ParserStage_MatchChronoToken) {
            goto chrono_percentage;
        }
        else {
            goto other_char;
        }
        // break
    default:
        if (IS_NUMBER(ch)) {
            goto digits_beg;
        }
        else {
            goto other_char;
        }
        // break
    }
digits_beg:
    LEX_MOVE_TO_NEXT(pstr);
    goto digits_body;
digits_body:
    ch = LEX_PEEK_CHAR(pstr);
    if (IS_NUMBER(ch)) {
        LEX_MOVE_TO_NEXT(pstr);
        goto digits_body;
    }
    else {
        goto digits_end;
    }
digits_end:
    LEX_ACCEPT_TOKEN(token, TokenType_Digits, pstr);
    goto acc;
eof:
    matched_type = TokenType_EOF;
    goto signed_char_acc;
gt:
    matched_type = TokenType_Gt;
    goto signed_char_acc;
lt:
    matched_type = TokenType_Lt;
    goto signed_char_acc;
equ:
    matched_type = TokenType_Equ;
    goto signed_char_acc;
colon:
    LEX_MOVE_TO_NEXT(pstr);
    LEX_ACCEPT_TOKEN(token, TokenType_Colon, pstr);
    // 检查后面跟的东东
    ch = LEX_PEEK_CHAR(pstr);
    switch (ch) {
    case 'i': goto type_ix;
    case 'u': goto type_ux;
    case 'q': goto type_quat;
    case 'F': goto type_fixed;
#if _MSTR_USE_FP
    case 'f': goto type_floating;
#endif // _MSTR_USE_FP
    case 's': goto type_str;
    case 't': goto type_time;
    default: goto acc;
    }
type_ix:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == '8') {
        matched_type = TokenType_Type_Int8;
        goto signed_char_acc;
    }
    else if (ch == '1') {
        goto type_i1x;
    }
    else if (ch == '3') {
        goto type_i3x;
    }
    else {
        // (roll back)
        goto acc;
    }
type_i1x:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == '6') {
        matched_type = TokenType_Type_Int16;
        goto signed_char_acc;
    }
    else {
        // roll back
        goto acc;
    }
type_i3x:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == '2') {
        matched_type = TokenType_Type_Int32;
        goto signed_char_acc;
    }
    else {
        // roll back
        goto acc;
    }
type_ux:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == '8') {
        matched_type = TokenType_Type_Uint8;
        goto signed_char_acc;
    }
    else if (ch == '1') {
        goto type_u1x;
    }
    else if (ch == '3') {
        goto type_u3x;
    }
    else {
        // (roll back)
        goto acc;
    }
type_u1x:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == '6') {
        matched_type = TokenType_Type_Uint16;
        goto signed_char_acc;
    }
    else {
        // roll back
        goto acc;
    }
type_u3x:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == '2') {
        matched_type = TokenType_Type_Uint32;
        goto signed_char_acc;
    }
    else {
        // roll back
        goto acc;
    }
type_fixed:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (IS_NUMBER(ch)) {
        goto type_fixed_ipart;
    }
    else {
        // (roll back)
        goto acc;
    }
type_fixed_ipart:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (IS_NUMBER(ch)) {
        goto type_fixed_ipart;
    }
    else {
        goto type_fixed_ipart_end;
    }
type_fixed_ipart_end:
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == '.') {
        LEX_MOVE_TO_NEXT(pstr);
        // { digit }+
        goto type_fixed_dpart;
    }
    else {
        goto type_fixed_end;
    }
type_fixed_dpart:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (IS_NUMBER(ch)) {
        goto type_fixed_dpart;
    }
    else {
        goto type_fixed_end;
    }
type_fixed_end:
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == 'u') {
        // UNSIGNED的
        matched_type = TokenType_Type_UFixedNumber;
        goto signed_char_acc;
    }
    else {
        // signed的
        LEX_ACCEPT_TOKEN(token, TokenType_Type_IFixedNumber, pstr);
        goto acc;
    }
type_quat:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (IS_NUMBER(ch)) {
        goto type_quat_bits;
    }
    else {
        // (roll back)
        goto acc;
    }
type_quat_bits:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (IS_NUMBER(ch) && token->beg - pstr <= 2) {
        goto type_quat_bits;
    }
    else {
        goto type_quat_end;
    }
type_quat_end:
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == 'u') {
        // UNSIGNED的
        matched_type = TokenType_Type_UQuant;
        goto signed_char_acc;
    }
    else {
        // signed的
        LEX_ACCEPT_TOKEN(token, TokenType_Type_IQuant, pstr);
        goto acc;
    }
#if _MSTR_USE_FP
type_floating:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == '1') {
        goto type_floating_1x;
    }
    else if (ch == '3') {
        goto type_floating_3x;
    }
    else if (ch == '6') {
        goto type_floating_6x;
    }
    else {
        // 默认fp32
        LEX_ACCEPT_TOKEN(token, TokenType_Type_Floating32, pstr);
        goto acc;
    }
type_floating_1x:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == '6') {
        LEX_MOVE_TO_NEXT(pstr);
        LEX_ACCEPT_TOKEN(token, TokenType_Type_Floating16, pstr);
    }
    else {
        goto err;
    }
type_floating_3x:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == '2') {
        LEX_MOVE_TO_NEXT(pstr);
        LEX_ACCEPT_TOKEN(token, TokenType_Type_Floating32, pstr);
    }
    else {
        goto err;
    }
type_floating_6x:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == '4') {
        LEX_MOVE_TO_NEXT(pstr);
        LEX_ACCEPT_TOKEN(token, TokenType_Type_Floating64, pstr);
    }
    else {
        goto err;
    }
#endif // _MSTR_USE_FP
type_str:
    matched_type = TokenType_Type_CString;
    goto signed_char_acc;
type_time:
    matched_type = TokenType_Type_SysTime;
    goto signed_char_acc;
v_line:
    LEX_MOVE_TO_NEXT(pstr);
    LEX_ACCEPT_TOKEN(token, TokenType_OtherChar, pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == ':') {
        matched_type = TokenType_VlineColon;
        goto signed_char_acc;
    }
    else {
        goto acc;
    }
l_brace:
    LEX_MOVE_TO_NEXT(pstr);
    LEX_ACCEPT_TOKEN(token, TokenType_LeftBrace, pstr);
    goto l_brace_opt;
l_brace_opt:
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == '[') {
        matched_type = TokenType_LeftBraceBracket;
        goto signed_char_acc;
    }
    else if (ch == '{') {
        matched_type = TokenType_LeftBraceBrace;
        goto signed_char_acc;
    }
    else {
        goto acc;
    }
r_brace:
    LEX_MOVE_TO_NEXT(pstr);
    LEX_ACCEPT_TOKEN(token, TokenType_RightBrace, pstr);
    // 如果parser想结束了, 那么优先尝试匹配`{`
    if ((stage & ParserStage_NeedEnd) == ParserStage_NeedEnd) {
        goto acc;
    }
    else {
        goto r_brace_opt;
    }
r_brace_opt:
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == '}') {
        matched_type = TokenType_RightBraceBrace;
        goto signed_char_acc;
    }
    else {
        goto acc;
    }
r_bracket:
    LEX_MOVE_TO_NEXT(pstr);
    LEX_ACCEPT_TOKEN(token, TokenType_OtherChar, pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == '}') {
        matched_type = TokenType_RightBracketBrace;
        goto signed_char_acc;
    }
    else {
        goto acc;
    }
chrono_percentage:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    switch (ch) {
    case '%': goto other_char;
    case 'f': goto chrono_predef_f;
    case 'g': goto chrono_predef_g;
    case 'y': goto chrono_userdef_y;
    case 'M': goto chrono_userdef_M;
    case 'd': goto chrono_userdef_d;
    case 'h': goto chrono_userdef_h;
    case 'H': goto chrono_userdef_H;
    case 'm': goto chrono_userdef_m;
    case 's': goto chrono_userdef_s;
    case 'x': goto chrono_userdef_x;
    case 'w': goto chrono_userdef_W;
    case '>':
    case '<':
    case '=':
        // 此类情况认为是fill align的部份, 识别成单个字符
        LEX_ACCEPT_TOKEN(token, TokenType_OtherChar, pstr);
        goto acc;
    default:
        // 其余情况不应该出现单个字符
        goto err;
    }
chrono_predef_f:
    matched_type = TokenType_ChronoPredef_f;
    goto signed_char_acc;
chrono_predef_g:
    matched_type = TokenType_ChronoPredef_g;
    goto signed_char_acc;
chrono_userdef_y:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == 'y') {
        goto chrono_userdef_yy;
    }
    else {
        LEX_ACCEPT_TOKEN(token, TokenType_Chrono_Year_1, pstr);
        goto acc;
    }
chrono_userdef_yy:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == 'y') {
        goto chrono_userdef_yyy;
    }
    else {
        LEX_ACCEPT_TOKEN(token, TokenType_Chrono_Year_Fixed2, pstr);
        goto acc;
    }
chrono_userdef_yyy:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == 'y') {
        goto chrono_userdef_yyyy;
    }
    else {
        LEX_ACCEPT_TOKEN(token, TokenType_Chrono_Year_3, pstr);
        goto acc;
    }
chrono_userdef_yyyy:
    matched_type = TokenType_Chrono_Year_Fixed4;
    goto signed_char_acc;
chrono_userdef_M:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == 'M') {
        goto chrono_userdef_MM;
    }
    else {
        LEX_ACCEPT_TOKEN(token, TokenType_Chrono_Month_1, pstr);
        goto acc;
    }
chrono_userdef_MM:
    matched_type = TokenType_Chrono_Month_Fixed2;
    goto signed_char_acc;
chrono_userdef_d:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == 'd') {
        goto chrono_userdef_dd;
    }
    else {
        LEX_ACCEPT_TOKEN(token, TokenType_Chrono_Day_1, pstr);
        goto acc;
    }
chrono_userdef_dd:
    matched_type = TokenType_Chrono_Day_Fixed2;
    goto signed_char_acc;
chrono_userdef_h:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == 'h') {
        goto chrono_userdef_hh;
    }
    else {
        LEX_ACCEPT_TOKEN(token, TokenType_Chrono_Hour_1, pstr);
        goto acc;
    }
chrono_userdef_hh:
    matched_type = TokenType_Chrono_Hour_Fixed2;
    goto signed_char_acc;
chrono_userdef_H:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == 'H') {
        goto chrono_userdef_HH;
    }
    else {
        LEX_ACCEPT_TOKEN(token, TokenType_Chrono_Hour24_1, pstr);
        goto acc;
    }
chrono_userdef_HH:
    matched_type = TokenType_Chrono_Hour24_Fixed2;
    goto signed_char_acc;
chrono_userdef_m:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == 'm') {
        goto chrono_userdef_mm;
    }
    else {
        LEX_ACCEPT_TOKEN(token, TokenType_Chrono_Minute_1, pstr);
        goto acc;
    }
chrono_userdef_mm:
    matched_type = TokenType_Chrono_Minute_Fixed2;
    goto signed_char_acc;
chrono_userdef_s:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == 's') {
        goto chrono_userdef_ss;
    }
    else {
        LEX_ACCEPT_TOKEN(token, TokenType_Chrono_Second_1, pstr);
        goto acc;
    }
chrono_userdef_ss:
    matched_type = TokenType_Chrono_Second_Fixed2;
    goto signed_char_acc;
chrono_userdef_x:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == 'x') {
        goto chrono_userdef_xx;
    }
    else {
        LEX_ACCEPT_TOKEN(
            token, TokenType_Chrono_SubSecond_Fixed1, pstr
        );
        goto acc;
    }
chrono_userdef_xx:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == 'x') {
        goto chrono_userdef_xxx;
    }
    else {
        LEX_ACCEPT_TOKEN(
            token, TokenType_Chrono_SubSecond_Fixed2, pstr
        );
        goto acc;
    }
chrono_userdef_xxx:
    LEX_MOVE_TO_NEXT(pstr);
    ch = LEX_PEEK_CHAR(pstr);
    if (ch == 'x') {
        goto chrono_userdef_xxxx;
    }
    else {
        LEX_ACCEPT_TOKEN(
            token, TokenType_Chrono_SubSecond_Fixed3, pstr
        );
        goto acc;
    }
chrono_userdef_xxxx:
    matched_type = TokenType_Chrono_SubSecond_Fixed4;
    goto signed_char_acc;
chrono_userdef_W:
    matched_type = TokenType_Chrono_WeekName;
    goto signed_char_acc;
other_char:
    // 默认情况, 识别为单个字符
    matched_type = TokenType_OtherChar;
    goto signed_char_acc;
signed_char_acc:
    // 按照 matched_type 接受token, 然后转移到 acc 状态
    LEX_MOVE_TO_NEXT(pstr);
    LEX_ACCEPT_TOKEN(token, matched_type, pstr);
    goto acc;
acc:
    return MStr_Ok;
err:
    return MStr_Err_UnrecognizedToken;
}

/**
 * @brief 将字符串转为无符号整数值
 *
 * @param str: 字符串
 * @param max_len: 最大读取的字符串长度
 *
 * @return uint32_t: 转换结果
 *
 * @attention 该函数假设值不超过u32
 */
static uint32_t lex_atou(const char* str, usize_t max_len)
{
    uint32_t r = 0;
    while (*str && max_len > 0) {
        uint32_t v = (uint32_t)(*str - '0');
        r = r * 10 + v;
        str += 1;
        max_len -= 1;
    }
    return r;
}
